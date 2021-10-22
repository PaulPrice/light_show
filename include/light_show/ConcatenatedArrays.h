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

        // We use two indices to track which array and which pixel in that array is in view.
        // We could use an index and an iterator, but this works for now.
        Iterator(ParentT & parent, IndexT index) : Iterator(parent, parent.getIndices(index)) {}
        Iterator(ParentT & parent, std::pair<IndexT, IndexT> const& indices)
          : Iterator(parent, indices.first, indices.second) {}
        Iterator(ParentT & parent, IndexT array, IndexT pixel)
          : _parent(parent), _array(array), _pixel(pixel) {}

        Iterator(Iterator const&) = default;
        Iterator(Iterator &&) = default;
        Iterator & operator=(Iterator const&) = default;
        Iterator & operator=(Iterator &&) = default;

        reference operator*() const { return _parent._arrays[_array][_pixel]; }

        Iterator& operator++() {
            ++_pixel;
            if (_pixel >= Index(_parent._arrays[_array].size())) {
                ++_array;
                _pixel = 0;
            }
            return *this;
        }
        Iterator operator--() {
            --_pixel;
            if (_pixel < 0) {
                --_array;
                _pixel = _parent._arrays[_array].size() - 1;
            }
        }

        Iterator operator++(int) { Iterator tmp = *this; ++*this; return tmp; }
        Iterator operator--(int) { Iterator tmp = *this; --*this; return tmp; }

        Iterator operator+(difference_type offset) const {
            return Iterator(_parent, _parent.getIndices(_parent.getIndex(_array, _pixel + offset)));
        }
        Iterator operator-(difference_type offset) const {
            return Iterator(_parent, _parent.getIndices(_parent.getIndex(_array, _pixel - offset)));
        }

        Iterator& operator+=(difference_type offset) {
            if (offset == 1) {
                return ++*this;
            }
            std::tie(_array, _pixel) = _parent.getIndices(_parent.getIndex(_array, _pixel + offset));
            return *this;
        }
        Iterator& operator-=(difference_type offset) {
            if (offset == 1) {
                return --*this;
            }
            std::tie(_array, _pixel) = _parent.getIndices(_parent.getIndex(_array, _pixel - offset));
            return *this;
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
    using Size = std::size_t;
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
            for (Size ii = 0; ii < _numArrays; ++ii) {
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

    Size size() const {
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
    ndarray::Array<T, 1, 1> operator[](ndarray::Array<Index, 1, 1> const& indices) {
        ndarray::Array<T, 1, 1> result = ndarray::allocate(indices.size());
        std::transform(indices.begin(), indices.end(), result.begin(),
                       [this](Index index) { return (*this)[index]; });
        return result;
    }

    ConcatenatedArraysRef<T> slice(Index start, Index stop, Index step) {
        // No python-like indexing: we've already done it in the layer above.
        Container arrays;
        arrays.reserve(_numArrays);
        for (Size ii = 0; ii < _numArrays && stop >= 0; ++ii) {
            Index const length = _arrays[ii].size();
            if (start < length) {
                arrays.push_back(_arrays[ii][ndarray::view(start, std::min(stop + 1, length), step)].deep());
                Index const num = (std::min(stop, length) - start + 1)/step;
                start = start - length + num*step;
            } else {
                start -= length;
            }
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
        for (Size ii = 0; ii < arrays._numArrays; ++ii) {
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
        for (Size ii = 0; ii < _numArrays; ++ii) {
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
    Size _numPixels;
    Size _numArrays;
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
        return set(scalar, [](T, T rhs) { return rhs; });
    }
    ConcatenatedArraysRef operator=(typename ConcatenatedArrays<T>::Array::Shallow const& array)  {
        return set(array, [](T, T rhs) { return rhs; });
    }
    ConcatenatedArraysRef operator=(ConcatenatedArrays<T> const& other) {
        return set(other, [](T, T rhs) { return rhs; });
    }

    ConcatenatedArraysRef operator+=(T scalar) {
        return set(scalar, [](T lhs, T rhs) { return lhs + rhs; });
    }
    ConcatenatedArraysRef operator+=(typename ConcatenatedArrays<T>::Array::Shallow const& array)  {
        return set(array, [](T lhs, T rhs) { return lhs + rhs; });
    }
    ConcatenatedArraysRef operator+=(ConcatenatedArrays<T> const& other) {
        return set(other, [](T lhs, T rhs) { return lhs + rhs; });
    }

    ConcatenatedArraysRef operator-=(T scalar) {
        return set(scalar, [](T lhs, T rhs) { return lhs - rhs; });
    }
    ConcatenatedArraysRef operator-=(typename ConcatenatedArrays<T>::Array::Shallow const& array)  {
        return set(array, [](T lhs, T rhs) { return lhs - rhs; });
    }
    ConcatenatedArraysRef operator-=(ConcatenatedArrays<T> const& other) {
        return set(other, [](T lhs, T rhs) { return lhs - rhs; });
    }

    ConcatenatedArraysRef operator*=(T scalar) {
        return set(scalar, [](T lhs, T rhs) { return lhs * rhs; });
    }
    ConcatenatedArraysRef operator*=(typename ConcatenatedArrays<T>::Array::Shallow const& array)  {
        return set(array, [](T lhs, T rhs) { return lhs * rhs; });
    }
    ConcatenatedArraysRef operator*=(ConcatenatedArrays<T> const& other) {
        return set(other, [](T lhs, T rhs) { return lhs * rhs; });
    }

    ConcatenatedArraysRef operator/=(T scalar) {
        return set(scalar, [](T lhs, T rhs) { return lhs / rhs; });
    }
    ConcatenatedArraysRef operator/=(typename ConcatenatedArrays<T>::Array::Shallow const& array)  {
        return set(array, [](T lhs, T rhs) { return lhs / rhs; });
    }
    ConcatenatedArraysRef operator/=(ConcatenatedArrays<T> const& other) {
        return set(other, [](T lhs, T rhs) { return lhs / rhs; });
    }

    ConcatenatedArraysRef operator%=(T scalar) {
        return set(scalar, [](T lhs, T rhs) { return lhs % rhs; });
    }
    ConcatenatedArraysRef operator%=(typename ConcatenatedArrays<T>::Array::Shallow const& array)  {
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
    ConcatenatedArraysRef set(typename ConcatenatedArrays<T>::Array::Shallow const& array,
                              BinaryFunction func) {
        if (array.size() != this->size()) {
            throw std::length_error("Array size mismatch");
        }
        typename ConcatenatedArrays<T>::Index ii = 0;
        for (typename ConcatenatedArrays<T>::Size index = 0; index < this->_numArrays; ++index) {
            typename ConcatenatedArrays<T>::Array sub = this->_arrays[index];
            for (typename ConcatenatedArrays<T>::Size jj = 0; jj < sub.size(); ++jj, ++ii) {
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
        for (typename ConcatenatedArrays<T>::Size index = 0; index < other.size();
             ++index, ++thisIter, ++otherIter) {
            *thisIter = func(*thisIter, *otherIter);
        }
        return *this;
    }

    friend class ConcatenatedArrays<T>;
};


}  // namespace light_show

#endif  // include guard
