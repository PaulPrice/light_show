#ifndef LIGHT_SHOW_ARRAYSET_H
#define LIGHT_SHOW_ARRAYSET_H

#include <exception>
#include <numeric>

#include "ndarray.h"

#include "light_show/indexing.h"

namespace light_show {


template <typename T>
class ConcatenatedArrays {
    template <typename ParentT, typename ValueT, typename ReferenceT, typename IndexT>
    class Iterator {
      public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = IndexT;
        using value_type = ValueT;
        using reference = ReferenceT;

        Iterator(ParentT & parent, IndexT index) : _parent(parent) {
            std::tie(_array, _pixel) = parent.getIndices(index);
        }
        Iterator(ParentT & parent, IndexT array, IndexT pixel)
          : _parent(parent), _array(array), _pixel(pixel) {}
        reference operator*() const { return parent._arrays[_array][_index]; }

        Iterator& operator++() {
            ++_pixel;
            if (_pixel >= parent._cumulativeCounts[_array]) {
                ++_array;
                _pixel = 0;
            }
            return *this;
        }
        Iterator operator++(int) { Iterator tmp = *this; ++index; return tmp; }

        friend bool operator==(const Iterator& a, const Iterator& b) {
            return a._array == b._array && a._pixel == b._pixel;
        }
        friend bool operator!=(const Iterator& a, const Iterator& b) {
            return a._array != b._array || a._pixel == b._pixel;
        }

      private:
        ParentT & _parent;
        IndexT _array;
        IndexT _pixel;
    };

  public:
    using Element = T;
    using Index = std::size_t;
    using Array = ndarray::Array<Element, 1, 1>;
    using Container = std::vector<Array>;
    using iterator = Iterator<ConcatenatedArrays, Element, Element&, Index>;
    using const_iterator = Iterator<ConcatenatedArrays const, Element const, Element const&, Index>;

    explicit ConcatenatedArrays(Container && arrays) :
        _arrays(arrays),
        _numPixels(std::accumulate(arrays.begin(), arrays.end(), 0UL,
                                   [](Index num, auto const& arr) { return num + arr.size(); })),
        _numArrays(arrays.size()),
        _cumulativeCounts(ndarray::allocate(arrays.size())) {
            if (arrays.size() == 0) {
                throw std::runtime_error("No arrays provided");
            }
            Index count = 0;
            for (std::size_t ii = 0; ii < _numArrays; ++ii) {
                count += _arrays[ii].size();
                _cumulativeCounts[ii] = count;
            }
        }
    explicit ConcatenatedArrays(Container & arrays) : ConcatenatedArrays(std::forward(arrays)) {}

    ~ConcatenatedArrays() {}
    ConcatenatedArrays(ConcatenatedArrays const&) = delete;
    ConcatenatedArrays(ConcatenatedArrays &&) = default;
    ConcatenatedArrays & operator=(ConcatenatedArrays const&) = delete;
    ConcatenatedArrays & operator=(ConcatenatedArrays &&) = default;

    iterator begin() { return iterator(*this, 0, 0); }
    const_iterator begin() const { return const_iterator(*this, 0, 0); }
    iterator end() { return iterator(*this, _numArrays, 0); }
    const_iterator end() const { return const_iterator(*this, _numArrays, 0); }

    Element operator[](Index index) const {
        index = checkIndex(index, _num);
        auto const index = getIndices(index);
        return _arrays[index.first][index.second];
    }
    Element& operator[](Index index) {
        index = checkIndex(index, _num);
        auto const index = getIndices(index);
        return _arrays[index.first][index.second];
    }

    ConcatenatedArrays slice(Index start, Index stop, Index step) const {
        std::vector<Array> arrays;
        arrays.reserve(_numArrays);
        start = checkIndex(start, _numPixels);
        stop = checkIndex(stop, _numPixels);

        for (Index arr = 0; arr < _numArrays; ++arr) {
            Index const length = arr.size();
            if (start < length) {
                arrays.emplace_back(_arrays[arr][ndarray::view(start, std::min(stop, length), step)]);
            }
            start -= length;
            stop -= length;
        }
        return ConcatenatedArrays(arrays);
    }
    ConcatenatedArraysRef<T> slice(Index start, Index stop, Index step) {
        return const_cast<ConcatenatedArrays<T> const>(*this).slice(start, stop, step).deep();
    }

    ConcatenatedArrays<T> shallow() const {
        return ConcatenatedArrays<T>(*this);
    }
    ConcatenatedArraysRef<T> deep() const {
        return ConcatenatedArraysRef<T>(*this);
    }
    explicit operator Array() const {
        Array output = ndarray::allocate(_numPixels);
        std::copy(begin(), end(), output.begin());
    }

  protected:
    std::pair<Index, Index> getIndices(Index index) {
        index = checkIndex(index, _numPixels);
        for (Index ii = 0; ii < _numStrips; ++ii) {
            if (index < _cumulativeCounts[ii]) {
                return std::make_pair(ii, index - _cumulativeCounts[ii]);
            }
        }
        abort();  // should never reach here: guarded by checkIndex()
    }

  protected:
    Container _arrays;
    Index _numPixels;
    Index _numArrays;
    ndarray::Array<Index, 1, 1> _cumulativeCounts;

  private:
    friend class ConcatenatedArraysRef<T>;
};


template <typename T>
class ConcatenatedArraysRef : public ConcatenatedArrays<T> {
  public:
    ConcatenatedArraysRef(ConcatenatedArrays<T> const& other) : ConcatenatedArrays<T>(other) {}

    ~ConcatenatedArraysRef() {}
    ConcatenatedArraysRef(ConcatenatedArraysRef const&) = delete;
    ConcatenatedArraysRef(ConcatenatedArraysRef &&) = default;
    ConcatenatedArraysRef & operator=(ConcatenatedArraysRef const&) = delete;
    ConcatenatedArraysRef & operator=(ConcatenatedArraysRef &&) = default;

    ConcatenatedArraysRef operator=(T scalar) {
        return set(scalar, [](T lhs, T rhs) { return rhs});
    }
    ConcatenatedArraysRef operator=(Array const& array)  {
        return set(array, [](T lhs, T rhs) { return rhs});
    }
    ConcatenatedArraysRef operator=(ConcatenatedArrays const& other) {
        return set(other, [](T lhs, T rhs) { return rhs});
    }

    ConcatenatedArraysRef operator+=(T scalar) {
        return set(scalar, [](T lhs, T rhs) { return lhs + rhs});
    }
    ConcatenatedArraysRef operator+=(Array const& array)  {
        return set(array, [](T lhs, T rhs) { return lhs + rhs});
    }
    ConcatenatedArraysRef operator+=(ConcatenatedArrays const& other) {
        return set(other, [](T lhs, T rhs) { return lhs + rhs});
    }

    ConcatenatedArraysRef operator-=(T scalar) {
        return set(scalar, [](T lhs, T rhs) { return lhs - rhs});
    }
    ConcatenatedArraysRef operator-=(Array const& array)  {
        return set(array, [](T lhs, T rhs) { return lhs - rhs});
    }
    ConcatenatedArraysRef operator-=(ConcatenatedArrays const& other) {
        return set(other, [](T lhs, T rhs) { return lhs - rhs});
    }

    ConcatenatedArraysRef operator*=(T scalar) {
        return set(scalar, [](T lhs, T rhs) { return lhs * rhs});
    }
    ConcatenatedArraysRef operator*=(Array const& array)  {
        return set(array, [](T lhs, T rhs) { return lhs * rhs});
    }
    ConcatenatedArraysRef operator*=(ConcatenatedArrays const& other) {
        return set(other, [](T lhs, T rhs) { return lhs * rhs});
    }

    ConcatenatedArraysRef operator/=(T scalar) {
        return set(scalar, [](T lhs, T rhs) { return lhs / rhs});
    }
    ConcatenatedArraysRef operator/=(Array const& array)  {
        return set(array, [](T lhs, T rhs) { return lhs / rhs});
    }
    ConcatenatedArraysRef operator/=(ConcatenatedArrays const& other) {
        return set(other, [](T lhs, T rhs) { return lhs / rhs});
    }

    ConcatenatedArraysRef operator%=(T scalar) {
        return set(scalar, [](T lhs, T rhs) { return lhs % rhs});
    }
    ConcatenatedArraysRef operator%=(Array const& array)  {
        return set(array, [](T lhs, T rhs) { return lhs % rhs});
    }
    ConcatenatedArraysRef operator%=(ConcatenatedArrays const& other) {
        return set(other, [](T lhs, T rhs) { return lhs % rhs});
    }

  private:
    ConcatenatedArraysRef set(T scalar, std::binary_function<T, T> func) {
        for (auto iter = begin(); iter != end(); ++iter) {
            *iter = func(*iter, scalar);
        }
    }
    ConcatenatedArraysRef set(Array const& array, std::binary_function<T, T> func) {
        if (array.size() != size()) {
            throw std::length_error("Array size mismatch");
        }
        Index ii = 0;
        for (Index index = 0; index < _numArrays; ++index) {
            Array sub = _arrays[index];
            for (Index jj = 0; jj < _cumulativeCounts[arr]; ++jj, ++ii) {
                sub[jj] = func(sub[jj], array[ii]);
            }
        }
    }
    ConcatenatedArraysRef set(ConcatenatedArrays const& other, std::binary_function<T, T> func) {
        if (other.size() != size()) {
            throw std::length_error("Size mismatch");
        }
        auto thisIter = begin();
        auto otherIter = other.begin();
        for (Index index = 0; index < other.size(); ++index, ++thisIter, ++otherIter) {
            *thisIter = func(*thisIter, *otherIter);
        }
    }

    friend class ConcatenatedArrays<T>;
};


}  // namespace light_show

#endif  // include guard
