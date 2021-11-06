#ifndef LIGHT_SHOW_CONCATENATEDARRAYS_H
#define LIGHT_SHOW_CONCATENATEDARRAYS_H

#include <utility>
#include <exception>
#include <numeric>
#include <iterator>

#include "ndarray.h"

#include "light_show/indexing.h"

namespace light_show {

namespace detail {

template <typename ParentT, typename ValueT>
class ConcatenatedArraysIterator {
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = typename ParentT::Index;
    using value_type = ValueT;
    using pointer = ValueT*;
    using reference = ValueT&;

    // We use two indices to track which array and which pixel in that array is in view.
    // We could use the individual array iterators, but this works for now.
    ConcatenatedArraysIterator(ParentT & parent, difference_type index) : ConcatenatedArraysIterator(parent, parent.getIndices(index)) {}
    ConcatenatedArraysIterator(ParentT & parent, std::pair<difference_type, difference_type> const& indices)
        : ConcatenatedArraysIterator(parent, indices.first, indices.second) {}
    ConcatenatedArraysIterator(ParentT & parent, difference_type array, difference_type pixel)
        : _parent(parent), _array(array), _pixel(pixel) {}

    ConcatenatedArraysIterator(ConcatenatedArraysIterator const&) = default;
    ConcatenatedArraysIterator(ConcatenatedArraysIterator &&) = default;
    ConcatenatedArraysIterator & operator=(ConcatenatedArraysIterator const&) = default;
    ConcatenatedArraysIterator & operator=(ConcatenatedArraysIterator &&) = default;

    reference operator*() const { return _parent._arrays[_array][_pixel]; }
    pointer operator->() const { return &_parent._arrays[_array][_pixel]; }

    ConcatenatedArraysIterator& operator++();
    ConcatenatedArraysIterator& operator--();

    ConcatenatedArraysIterator operator++(int) {
        ConcatenatedArraysIterator tmp = *this; ++*this; return tmp;
    }
    ConcatenatedArraysIterator operator--(int) {
        ConcatenatedArraysIterator tmp = *this; --*this; return tmp;
    }

    ConcatenatedArraysIterator operator+(difference_type offset) const {
        return ConcatenatedArraysIterator(
            _parent,
            _parent.getIndices(_parent.getIndex(_array, _pixel + offset))
        );
    }
    ConcatenatedArraysIterator operator-(difference_type offset) const {
        return ConcatenatedArraysIterator(
            _parent,
            _parent.getIndices(_parent.getIndex(_array, _pixel - offset))
        );
    }

    ConcatenatedArraysIterator& operator+=(difference_type offset);
    ConcatenatedArraysIterator& operator-=(difference_type offset);

    friend bool operator==(const ConcatenatedArraysIterator& a, const ConcatenatedArraysIterator& b) {
        return a._array == b._array && a._pixel == b._pixel;
    }
    friend bool operator!=(const ConcatenatedArraysIterator& a, const ConcatenatedArraysIterator& b) {
        return !(a == b);
    }

    private:
    ParentT & _parent;
    difference_type _array;
    difference_type _pixel;
};

}  // namespace detail

template <typename T> class ConcatenatedArraysRef;

template <typename T>
class ConcatenatedArrays {
  public:
    using Element = T;
    using Size = std::size_t;
    using Index = std::ptrdiff_t;  // Note: signed, so we can do python-style indexing
    using Array = ndarray::ArrayRef<Element, 1, 0>;  // non-sequential, so we can have stride != 1
    using Container = std::vector<Array>;
    using iterator = detail::ConcatenatedArraysIterator<ConcatenatedArrays, Element>;
    using const_iterator = detail::ConcatenatedArraysIterator<ConcatenatedArrays const, Element const>;
    friend iterator;
    friend const_iterator;

    explicit ConcatenatedArrays(Container && arrays);

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

    Element operator[](Index index) const;
    Element& operator[](Index index);
    ndarray::Array<T, 1, 1> operator[](ndarray::Array<Index, 1, 1> const& indices);
    ConcatenatedArraysRef<T> slice(Index start, Index stop, Index step);

    ConcatenatedArrays<T> shallow() const {
        return ConcatenatedArrays<T>(*this);
    }
    ConcatenatedArraysRef<T> deep() const {
        return ConcatenatedArraysRef<T>(*this);
    }
    explicit operator Array() const;

    friend class ConcatenatedArraysRef<T>;

    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, ConcatenatedArrays<U> const& arrays);

  protected:
    std::pair<Index, Index> getIndices(Index index) const;
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

    ConcatenatedArraysRef& operator=(T scalar) {
        return set(scalar, [](T, T rhs) { return rhs; });
    }
    ConcatenatedArraysRef& operator=(typename ConcatenatedArrays<T>::Array::Shallow const& array)  {
        return set(array, [](T, T rhs) { return rhs; });
    }
    ConcatenatedArraysRef& operator=(ConcatenatedArrays<T> const& other) {
        return set(other, [](T, T rhs) { return rhs; });
    }

    ConcatenatedArraysRef& operator+=(T scalar) {
        return set(scalar, [](T lhs, T rhs) { return lhs + rhs; });
    }
    ConcatenatedArraysRef& operator+=(typename ConcatenatedArrays<T>::Array::Shallow const& array)  {
        return set(array, [](T lhs, T rhs) { return lhs + rhs; });
    }
    ConcatenatedArraysRef& operator+=(ConcatenatedArrays<T> const& other) {
        return set(other, [](T lhs, T rhs) { return lhs + rhs; });
    }

    ConcatenatedArraysRef& operator-=(T scalar) {
        return set(scalar, [](T lhs, T rhs) { return lhs - rhs; });
    }
    ConcatenatedArraysRef& operator-=(typename ConcatenatedArrays<T>::Array::Shallow const& array)  {
        return set(array, [](T lhs, T rhs) { return lhs - rhs; });
    }
    ConcatenatedArraysRef& operator-=(ConcatenatedArrays<T> const& other) {
        return set(other, [](T lhs, T rhs) { return lhs - rhs; });
    }

    ConcatenatedArraysRef& operator*=(T scalar) {
        return set(scalar, [](T lhs, T rhs) { return lhs * rhs; });
    }
    ConcatenatedArraysRef& operator*=(typename ConcatenatedArrays<T>::Array::Shallow const& array)  {
        return set(array, [](T lhs, T rhs) { return lhs * rhs; });
    }
    ConcatenatedArraysRef& operator*=(ConcatenatedArrays<T> const& other) {
        return set(other, [](T lhs, T rhs) { return lhs * rhs; });
    }

    ConcatenatedArraysRef& operator/=(T scalar) {
        return set(scalar, [](T lhs, T rhs) { return lhs / rhs; });
    }
    ConcatenatedArraysRef& operator/=(typename ConcatenatedArrays<T>::Array::Shallow const& array)  {
        return set(array, [](T lhs, T rhs) { return lhs / rhs; });
    }
    ConcatenatedArraysRef& operator/=(ConcatenatedArrays<T> const& other) {
        return set(other, [](T lhs, T rhs) { return lhs / rhs; });
    }

    ConcatenatedArraysRef& operator%=(T scalar) {
        return set(scalar, [](T lhs, T rhs) { return lhs % rhs; });
    }
    ConcatenatedArraysRef& operator%=(typename ConcatenatedArrays<T>::Array::Shallow const& array)  {
        return set(array, [](T lhs, T rhs) { return lhs % rhs; });
    }
    ConcatenatedArraysRef& operator%=(ConcatenatedArrays<T> const& other) {
        return set(other, [](T lhs, T rhs) { return lhs % rhs; });
    }

  private:
    template <typename BinaryFunction>
    ConcatenatedArraysRef& set(T scalar, BinaryFunction func);
    template <typename BinaryFunction>
    ConcatenatedArraysRef& set(typename ConcatenatedArrays<T>::Array::Shallow const& array,
                              BinaryFunction func);
    template <typename BinaryFunction>
    ConcatenatedArraysRef& set(ConcatenatedArrays<T> const& other, BinaryFunction func);

    friend class ConcatenatedArrays<T>;
};


template <typename T>
template <typename BinaryFunction>
ConcatenatedArraysRef<T>& ConcatenatedArraysRef<T>::set(T scalar, BinaryFunction func) {
    for (auto iter = this->begin(); iter != this->end(); ++iter) {
        *iter = func(*iter, scalar);
    }
    return *this;
}


template <typename T>
template <typename BinaryFunction>
ConcatenatedArraysRef<T>& ConcatenatedArraysRef<T>::set(
    typename ConcatenatedArrays<T>::Array::Shallow const& array,
    BinaryFunction func
) {
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


template <typename T>
template <typename BinaryFunction>
ConcatenatedArraysRef<T>& ConcatenatedArraysRef<T>::set(
    ConcatenatedArrays<T> const& other,
    BinaryFunction func
) {
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


}  // namespace light_show

#endif  // include guard
