#include "light_show/config.h"
#include "light_show/ConcatenatedArrays.h"

namespace light_show {
namespace detail {

template <typename ParentT, typename ValueT>
ConcatenatedArraysIterator<ParentT, ValueT>&
ConcatenatedArraysIterator<ParentT, ValueT>::operator++() {
    ++_pixel;
    if (_pixel >= typename ParentT::Index(_parent._arrays[_array].size())) {
        ++_array;
        _pixel = 0;
    }
    return *this;
}


template <typename ParentT, typename ValueT>
ConcatenatedArraysIterator<ParentT, ValueT>&
ConcatenatedArraysIterator<ParentT, ValueT>::operator--() {
    --_pixel;
    if (_pixel < 0) {
        --_array;
        _pixel = _parent._arrays[_array].size() - 1;
    }
    return *this;
}


template <typename ParentT, typename ValueT>
ConcatenatedArraysIterator<ParentT, ValueT>&
ConcatenatedArraysIterator<ParentT, ValueT>::operator+=(difference_type offset) {
    if (offset == 1) {
        return ++*this;
    }
    std::tie(_array, _pixel) = _parent.getIndices(_parent.getIndex(_array, _pixel + offset));
    return *this;
}


template <typename ParentT, typename ValueT>
ConcatenatedArraysIterator<ParentT, ValueT>&
ConcatenatedArraysIterator<ParentT, ValueT>::operator-=(difference_type offset) {
    if (offset == 1) {
        return --*this;
    }
    std::tie(_array, _pixel) = _parent.getIndices(_parent.getIndex(_array, _pixel - offset));
    return *this;
}


}  // namespace detail


template <typename T>
ConcatenatedArrays<T>::ConcatenatedArrays(Container arrays)
  : _arrays(std::move(arrays)),
    _numPixels(std::accumulate(_arrays.begin(), _arrays.end(), 0UL,
                                [](Index num, auto const& arr) { return num + arr.size(); })),
    _numArrays(_arrays.size()),
    _cumulativeCounts(ndarray::allocate(_numArrays)) {
        if (_numArrays == 0) {
            throw std::runtime_error("No arrays provided");
        }
        Index count = 0;
        for (Size ii = 0; ii < _numArrays; ++ii) {
            count += _arrays[ii].size();
            _cumulativeCounts[ii] = count;
        }
    }


template <typename T>
typename ConcatenatedArrays<T>::Element ConcatenatedArrays<T>::operator[](Index index) const {
    auto const indices = getIndices(index);
    return _arrays[indices.first][indices.second];
}


template <typename T>
typename ConcatenatedArrays<T>::Element& ConcatenatedArrays<T>::operator[](Index index) {
    auto const indices = getIndices(index);
    return _arrays[indices.first][indices.second];
}


template <typename T>
ndarray::Array<T, 1, 1> ConcatenatedArrays<T>::operator[](ndarray::Array<Index, 1, 1> const& indices) {
    ndarray::Array<T, 1, 1> result = ndarray::allocate(indices.size());
    std::transform(indices.begin(), indices.end(), result.begin(),
                    [this](Index index) { return (*this)[index]; });
    return result;
}


template <typename T>
ConcatenatedArraysRef<T> ConcatenatedArrays<T>::slice(Index start, Index stop, Index step) {
    // No python-like indexing here: we've already done it in the pybind layer.
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


template <typename T>
ConcatenatedArrays<T>::operator ConcatenatedArrays<T>::Array() const {
    Array output = ndarray::allocate(_numPixels);
    std::copy(begin(), end(), output.begin());
    return output;
}


template <typename T>
std::ostream& operator<<(std::ostream& os, ConcatenatedArrays<T> const& arrays) {
    os << "ConcatenatedArrays(";
    for (typename ConcatenatedArrays<T>::Size ii = 0; ii < arrays._numArrays; ++ii) {
        os << arrays._arrays[ii];
        if (ii != arrays._numArrays - 1) {
            os << ", ";
        }
    }
    os << ")";
    return os;
}


template <typename T>
std::pair<typename ConcatenatedArrays<T>::Index, typename ConcatenatedArrays<T>::Index>
ConcatenatedArrays<T>::getIndices(Index index) const {
    index = checkIndex(index, _numPixels);
    for (Size ii = 0; ii < _numArrays; ++ii) {
        if (index < _cumulativeCounts[ii]) {
            Index const offset = ii > 0 ? _cumulativeCounts[ii - 1] : 0;
            return std::make_pair(ii, index - offset);
        }
    }
    abort();  // should never reach here: guarded by checkIndex()
}


#define INSTANTIATE(TYPE) \
template class ConcatenatedArrays<TYPE>; \
template class ConcatenatedArraysRef<TYPE>; \
template class detail::ConcatenatedArraysIterator<ConcatenatedArrays<TYPE>, TYPE>; \
template class detail::ConcatenatedArraysIterator<ConcatenatedArrays<TYPE> const, TYPE const>; \
template std::ostream& operator<<(std::ostream&, ConcatenatedArrays<TYPE> const&);

INSTANTIATE(Pixel);

}  // namespace light_show
