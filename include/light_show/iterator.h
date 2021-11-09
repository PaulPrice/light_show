#ifndef LIGHT_SHOW_ITERATOR_H
#define LIGHT_SHOW_ITERATOR_H

#include <iterator>

namespace light_show {

template <typename ContainerT, typename ValueT, typename ReferenceT, typename IndexT>
struct IndexBasedIterator {
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = IndexT;
    using value_type = ValueT;
    using pointer = ValueT*;
    using reference = ReferenceT;

    IndexBasedIterator(ContainerT & container_, int index_) : container(container_), index(index_) {}
    ~IndexBasedIterator() {}
    IndexBasedIterator(IndexBasedIterator const&) = default;
    IndexBasedIterator(IndexBasedIterator &&) = default;
    IndexBasedIterator & operator=(IndexBasedIterator const&) = default;
    IndexBasedIterator & operator=(IndexBasedIterator &&) = default;

    reference operator*() const { return container[index]; }
    pointer operator->() const { return &container[index]; }

    IndexBasedIterator& operator++() { ++index; return *this; }
    IndexBasedIterator operator++(int) { IndexBasedIterator tmp = *this; ++index; return tmp; }
    IndexBasedIterator operator+(difference_type offset) const {
        return IndexBasedIterator(container, index + offset);
    }
    IndexBasedIterator& operator+=(difference_type offset) { index += offset; return *this; }
    IndexBasedIterator& operator--() { --index; return *this; }
    IndexBasedIterator operator--(int) { IndexBasedIterator tmp = *this; --index; return tmp; }
    IndexBasedIterator operator-(difference_type offset) const {
        return IndexBasedIterator(container, index - offset);
    }
    IndexBasedIterator& operator-=(difference_type offset) { index -= offset; return *this; }

    friend bool operator==(const IndexBasedIterator& a, const IndexBasedIterator& b) {
        return a.index == b.index;
    };
    friend bool operator!=(const IndexBasedIterator& a, const IndexBasedIterator& b) {
        return a.index != b.index;
    };

    ContainerT & container;
    IndexT index;
};


}  // namespace light_show


#endif  // include guard
