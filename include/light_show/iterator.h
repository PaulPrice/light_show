#ifndef LIGHT_SHOW_ITERATOR_H
#define LIGHT_SHOW_ITERATOR_H

namespace light_show {

template <typename ContainerT, typename ValueT, typename ReferenceT, typename IndexT>
struct Iterator {
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = IndexT;
    using value_type = ValueT;
    using reference = ReferenceT;

    Iterator(ContainerT & container_, int index_) : container(container_), index(index_) {}
    ~Iterator() {}
    Iterator(Iterator const&) = default;
    Iterator(Iterator &&) = default;
    Iterator & operator=(Iterator const&) = default;
    Iterator & operator=(Iterator &&) = default;

    reference operator*() const { return container[index]; }

    Iterator& operator++() { ++index; return *this; }
    Iterator operator++(int) { Iterator tmp = *this; ++index; return tmp; }
    Iterator operator+(difference_type offset) const { return Iterator(container, index + offset); }
    Iterator& operator+=(difference_type offset) { index += offset; return *this; }
    Iterator& operator--() { --index; return *this; }
    Iterator operator--(int) { Iterator tmp = *this; --index; return tmp; }
    Iterator operator-(difference_type offset) const { return Iterator(container, index - offset); }
    Iterator& operator-=(difference_type offset) { index -= offset; return *this; }

    friend bool operator==(const Iterator& a, const Iterator& b) { return a.index == b.index; };
    friend bool operator!=(const Iterator& a, const Iterator& b) { return a.index != b.index; };

    ContainerT & container;
    IndexT index;
};


}  // namespace light_show


#endif  // include guard
