#ifndef LIGHT_SHOW_CONCATENATEDARRAYS_H
#define LIGHT_SHOW_CONCATENATEDARRAYS_H

#include <utility>
#include <exception>
#include <numeric>

#include "ndarray.h"

#include "light_show/indexing.h"

namespace light_show {


template <typename T> class ConcatenatedArraysRef;

template <typename T>
class ConcatenatedArrays {
    template <typename ParentT, typename ValueT, typename ReferenceT, typename IndexT>
    class Iterator {
      public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = IndexT;
        using value_type = ValueT;
        using reference = ReferenceT;

        Iterator(ParentT & parent, IndexT index) : Iterator(parent, parent.getIndices(index)) {}
        Iterator(ParentT & parent, std::pair<IndexT, IndexT> const& indices)
          : Iterator(parent, indices.first, indices.second) {}
        Iterator(ParentT & parent, IndexT array, IndexT pixel)
          : _parent(parent), _array(array), _pixel(pixel) {}

        reference operator*() const { return _parent._arrays[_array][_pixel]; }

        Iterator& operator++() {
            ++_pixel;
            if (_pixel >= _parent._arrays[_array].size()) {
                ++_array;
                _pixel = 0;
            }
            return *this;
        }
        Iterator operator++(int) { Iterator tmp = *this; ++*this; return tmp; }

        Iterator operator+(difference_type offset) const {
            return Iterator(_parent, _parent.getIndices(_parent.getIndex(_array, _pixel + offset)));
        }
        Iterator operator-(difference_type offset) const {
            return Iterator(_parent, _parent.getIndices(_parent.getIndex(_array, _pixel - offset)));
        }

        friend bool operator==(const Iterator& a, const Iterator& b) {
            return a._array == b._array && a._pixel == b._pixel;
        }
        friend bool operator!=(const Iterator& a, const Iterator& b) {
            return !(a == b);
        }

      private:
        ParentT & _parent;
        IndexT _array;
        IndexT _pixel;
    };

  public:
    using Element = T;
    using Index = std::ptrdiff_t;  // Note: signed, so we can do python-style indexing
    using Array = ndarray::ArrayRef<Element, 1, 0>;  // non-sequential, so we can have stride != 1
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

    ~ConcatenatedArrays() {}
    ConcatenatedArrays(ConcatenatedArrays const&) = default;
    ConcatenatedArrays(ConcatenatedArrays &&) = default;
    ConcatenatedArrays & operator=(ConcatenatedArrays const&) = default;
    ConcatenatedArrays & operator=(ConcatenatedArrays &&) = default;

    iterator begin() { return iterator(*this, 0, 0); }
    const_iterator begin() const { return const_iterator(*this, 0, 0); }
    iterator end() { return iterator(*this, _numArrays, 0); }
    const_iterator end() const { return const_iterator(*this, _numArrays, 0); }

    std::size_t size() const {
        return _cumulativeCounts[_numArrays - 1];
    }

    Element operator[](Index index) const {
        auto const indices = getIndices(index);
        return _arrays[indices.first][indices.second];
    }
    Element& operator[](Index index) {
        auto const indices = getIndices(index);
        return _arrays[indices.first][indices.second];
    }

    ConcatenatedArraysRef<T> slice(Index start, Index stop, Index step) {
        Container arrays;
        arrays.reserve(_numArrays);
        start = checkIndex(start, _numPixels);
        stop = checkIndex(stop, _numPixels);

        for (Index ii = 0; ii < _numArrays; ++ii) {
            Index const length = _arrays[ii].size();
            if (start < length) {
                arrays.push_back(_arrays[ii][ndarray::view(start, std::min(stop, length), step)].deep());
            }
            start -= length;
            stop -= length;
        }
        return ConcatenatedArraysRef<T>(std::move(arrays));
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
        return output;
    }

    friend class ConcatenatedArraysRef<T>;
    friend std::ostream& operator<<(std::ostream& os, ConcatenatedArrays<T> const& arrays) {
        os << "ConcatenatedArrays(";
        for (Index ii = 0; ii < arrays._numArrays; ++ii) {
            os << arrays._arrays[ii];
            if (ii != arrays._numArrays - 1) {
                os << ", ";
            }
        }
        os << ")";
        return os;
    }

  protected:
    std::pair<Index, Index> getIndices(Index index) const {
        index = checkIndex(index, _numPixels);
        for (Index ii = 0; ii < _numArrays; ++ii) {
            if (index < _cumulativeCounts[ii]) {
                Index const offset = ii > 0 ? _cumulativeCounts[ii - 1] : 0;
                return std::make_pair(ii, index - offset);
            }
        }
        abort();  // should never reach here: guarded by checkIndex()
    }
    Index getIndex(Index array, Index pixel) const {
        return (array == 0 ? 0 : _cumulativeCounts[array - 1]) + pixel;
    }

    Container _arrays;
    Index _numPixels;
    Index _numArrays;
    ndarray::Array<Index, 1, 1> _cumulativeCounts;
};


template <typename T>
class ConcatenatedArraysRef : public ConcatenatedArrays<T> {
  public:
    explicit ConcatenatedArraysRef(ConcatenatedArrays<T> const& other) : ConcatenatedArrays<T>(other) {}
    explicit ConcatenatedArraysRef(typename ConcatenatedArrays<T>::Container && arrays)
      : ConcatenatedArrays<T>(std::move(arrays)) {}

    ~ConcatenatedArraysRef() {}
    ConcatenatedArraysRef(ConcatenatedArraysRef const&) = default;
    ConcatenatedArraysRef(ConcatenatedArraysRef &&) = default;
    ConcatenatedArraysRef & operator=(ConcatenatedArraysRef const&) = default;
    ConcatenatedArraysRef & operator=(ConcatenatedArraysRef &&) = default;

    ConcatenatedArraysRef operator=(T scalar) {
        return set(scalar, [](T lhs, T rhs) { return rhs; });
    }
    ConcatenatedArraysRef operator=(typename ConcatenatedArrays<T>::Array const& array)  {
        return set(array, [](T lhs, T rhs) { return rhs; });
    }
    ConcatenatedArraysRef operator=(ConcatenatedArrays<T> const& other) {
        return set(other, [](T lhs, T rhs) { return rhs; });
    }

    ConcatenatedArraysRef operator+=(T scalar) {
        return set(scalar, [](T lhs, T rhs) { return lhs + rhs; });
    }
    ConcatenatedArraysRef operator+=(typename ConcatenatedArrays<T>::Array const& array)  {
        return set(array, [](T lhs, T rhs) { return lhs + rhs; });
    }
    ConcatenatedArraysRef operator+=(ConcatenatedArrays<T> const& other) {
        return set(other, [](T lhs, T rhs) { return lhs + rhs; });
    }

    ConcatenatedArraysRef operator-=(T scalar) {
        return set(scalar, [](T lhs, T rhs) { return lhs - rhs; });
    }
    ConcatenatedArraysRef operator-=(typename ConcatenatedArrays<T>::Array const& array)  {
        return set(array, [](T lhs, T rhs) { return lhs - rhs; });
    }
    ConcatenatedArraysRef operator-=(ConcatenatedArrays<T> const& other) {
        return set(other, [](T lhs, T rhs) { return lhs - rhs; });
    }

    ConcatenatedArraysRef operator*=(T scalar) {
        return set(scalar, [](T lhs, T rhs) { return lhs * rhs; });
    }
    ConcatenatedArraysRef operator*=(typename ConcatenatedArrays<T>::Array const& array)  {
        return set(array, [](T lhs, T rhs) { return lhs * rhs; });
    }
    ConcatenatedArraysRef operator*=(ConcatenatedArrays<T> const& other) {
        return set(other, [](T lhs, T rhs) { return lhs * rhs; });
    }

    ConcatenatedArraysRef operator/=(T scalar) {
        return set(scalar, [](T lhs, T rhs) { return lhs / rhs; });
    }
    ConcatenatedArraysRef operator/=(typename ConcatenatedArrays<T>::Array const& array)  {
        return set(array, [](T lhs, T rhs) { return lhs / rhs; });
    }
    ConcatenatedArraysRef operator/=(ConcatenatedArrays<T> const& other) {
        return set(other, [](T lhs, T rhs) { return lhs / rhs; });
    }

    ConcatenatedArraysRef operator%=(T scalar) {
        return set(scalar, [](T lhs, T rhs) { return lhs % rhs; });
    }
    ConcatenatedArraysRef operator%=(typename ConcatenatedArrays<T>::Array const& array)  {
        return set(array, [](T lhs, T rhs) { return lhs % rhs; });
    }
    ConcatenatedArraysRef operator%=(ConcatenatedArrays<T> const& other) {
        return set(other, [](T lhs, T rhs) { return lhs % rhs; });
    }

  private:
    template <typename BinaryFunction>
    ConcatenatedArraysRef set(T scalar, BinaryFunction func) {
        for (auto iter = this->begin(); iter != this->end(); ++iter) {
            *iter = func(*iter, scalar);
        }
        return *this;
    }
    template <typename BinaryFunction>
    ConcatenatedArraysRef set(typename ConcatenatedArrays<T>::Array const& array, BinaryFunction func) {
        if (array.size() != this->size()) {
            throw std::length_error("Array size mismatch");
        }
        typename ConcatenatedArrays<T>::Index ii = 0;
        for (typename ConcatenatedArrays<T>::Index index = 0; index < this->_numArrays; ++index) {
            typename ConcatenatedArrays<T>::Array sub = this->_arrays[index];
            for (typename ConcatenatedArrays<T>::Index jj = 0; jj < this->_cumulativeCounts[index];
                 ++jj, ++ii) {
                sub[jj] = func(sub[jj], array[ii]);
            }
        }
        return *this;
    }
    template <typename BinaryFunction>
    ConcatenatedArraysRef set(ConcatenatedArrays<T> const& other, BinaryFunction func) {
        if (other.size() != this->size()) {
            throw std::length_error("Size mismatch");
        }
        auto thisIter = this->begin();
        auto otherIter = other.begin();
        for (typename ConcatenatedArrays<T>::Index index = 0; index < other.size();
             ++index, ++thisIter, ++otherIter) {
            *thisIter = func(*thisIter, *otherIter);
        }
        return *this;
    }

    friend class ConcatenatedArrays<T>;
};


}  // namespace light_show

#endif  // include guard
