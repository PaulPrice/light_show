#ifndef LIGHT_SHOW_INDEXING_H
#define LIGHT_SHOW_INDEXING_H

namespace light_show {


template <typename IndexT, typename SizeT>
IndexT checkIndex(IndexT index, SizeT const num) {
    if (index < 0) {
        index += num;
    }
    if (index < 0 || index >= IndexT(num)) {
        throw std::out_of_range("Index out of range");
    }
    return index;
}


}  // namespace light_show

#endif  // include guard
