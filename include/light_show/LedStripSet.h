#ifndef LIGHT_SHOW_LEDSTRIPSET_H
#define LIGHT_SHOW_LEDSTRIPSET_H

#include <algorithm>
#include <exception>
#include <numeric>
#include <iterator>

#include "ndarray.h"

#include "light_show/LedStrip.h"
#include "light_show/ConcatenatedArrays.h"

namespace light_show {

namespace detail {

template <typename ArrayIterator, typename ReferenceT>
class LedStripSetIterator {
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = typename ArrayIterator::difference_type;
    using value_type = ColorRGB;
    using pointer = ReferenceT*;
    using reference = ReferenceT;

    LedStripSetIterator(ArrayIterator red, ArrayIterator green, ArrayIterator blue)
        : _red(red), _green(green), _blue(blue), _color(BLACK) {}

    LedStripSetIterator(LedStripSetIterator const&) = default;
    LedStripSetIterator(LedStripSetIterator &&) = default;
    LedStripSetIterator & operator=(LedStripSetIterator const&) = default;
    LedStripSetIterator & operator=(LedStripSetIterator &&) = default;

    reference operator*() const { return ReferenceT(*_red, *_green, *_blue); }
    pointer operator->() const { _color = ReferenceT(*_red, *_green, *_blue); return &_color; }

    LedStripSetIterator& operator++() {
        ++_red;
        ++_green;
        ++_blue;
        return *this;
    }
    LedStripSetIterator operator++(int) { LedStripSetIterator tmp = *this; ++*this; return tmp; }
    LedStripSetIterator operator+(difference_type offset) const {
        return LedStripSetIterator(_red + offset, _green + offset, _blue + offset);
    }
    LedStripSetIterator& operator+=(difference_type offset) {
        _red += offset;
        _green += offset;
        _blue += offset;
        return *this;
    }
    LedStripSetIterator& operator--() {
        --_red;
        --_green;
        --_blue;
        return *this;
    }
    LedStripSetIterator operator--(int) { LedStripSetIterator tmp = *this; --*this; return tmp; }
    LedStripSetIterator operator-(difference_type offset) const {
        return LedStripSetIterator(_red - offset, _green - offset, _blue - offset);
    }
    LedStripSetIterator& operator-=(difference_type offset) {
        _red -= offset;
        _green -= offset;
        _blue -= offset;
        return *this;
    }

    friend bool operator==(const LedStripSetIterator& lhs, const LedStripSetIterator& rhs) {
        return lhs._red == rhs._red && lhs._green == rhs._green && lhs._blue == rhs._blue;
    }
    friend bool operator!=(const LedStripSetIterator& lhs, const LedStripSetIterator& rhs) {
        return !(lhs == rhs);
    }

    private:
    ArrayIterator _red;
    ArrayIterator _green;
    ArrayIterator _blue;
    ColorRGB _color;
};


}  // namespace detail


class LedStripSet final {
  public:
    using Size = std::size_t;
    using Index = std::ptrdiff_t;
    using Collection = std::vector<LedStrip>;
    using Array = ConcatenatedArrays<Pixel>;
    using ArrayRef = ConcatenatedArraysRef<Pixel>;

  private:

  public:
    using iterator = detail::LedStripSetIterator<Array::iterator, ColorRGBRef>;
    using const_iterator = detail::LedStripSetIterator<Array::const_iterator, ColorRGB>;

    LedStripSet(Collection strips);
    LedStripSet(std::vector<LedStripSet> const& stripSets);

    ~LedStripSet() {}
    LedStripSet(LedStripSet const&) = default;
    LedStripSet(LedStripSet &&) = default;
    LedStripSet & operator=(LedStripSet const&) = default;
    LedStripSet & operator=(LedStripSet &&) = default;

    iterator begin() { return iterator(_red.begin(), _green.begin(), _blue.begin()); }
    const_iterator begin() const { return const_iterator(_red.begin(), _green.begin(), _blue.begin()); }
    iterator end() { return iterator(_red.end(), _green.end(), _blue.end()); }
    const_iterator end() const { return const_iterator(_red.end(), _green.end(), _blue.end()); }
    const_iterator cbegin() const { return const_iterator(_red.begin(), _green.begin(), _blue.begin()); }
    const_iterator cend() const { return const_iterator(_red.end(), _green.end(), _blue.end()); }

    ColorRGB get(Index index) const { return operator[](index); }
    ColorRGBRef get(Index index) { return operator[](index); }
    ArrayRef getRed() { return _red.deep(); }
    Array const& getRed() const { return _red; }
    ArrayRef getGreen() { return _green.deep(); }
    Array const& getGreen() const { return _green; }
    ArrayRef getBlue() { return _blue.deep(); }
    Array const& getBlue() const { return _blue; }

    void set(Index index, ColorRGB const& rgb) { operator[](index) = rgb; }
    void set(Index index, ColorHSV const& hsv) { operator[](index) = hsv.toRGB(); }
    void set(Index index, Pixel red, Pixel green, Pixel blue) {
        operator[](index) = ColorRGB(red, green, blue);
    }
    void setRed(Array const& array) { _red.deep() = array; }
    void setGreen(Array const& array) { _green.deep() = array; }
    void setBlue(Array const& array) { _blue.deep() = array; }

    Size size() const {
        return _numPixels;
    }

    ColorRGB operator[](Index index) const {
        index = checkIndex(index, _numPixels);
        return ColorRGB(_red[index], _green[index], _blue[index]);
    }
    ColorRGBRef operator[](Index index) {
        index = checkIndex(index, _numPixels);
        return ColorRGBRef(_red[index], _green[index], _blue[index]);
    }

    LedStripSet slice(Index start, Index stop, Index step);

    bool isOn() const;

    void fill(Pixel red, Pixel green, Pixel blue) {
        fill(ColorRGB(red, green, blue));
    }
    void fill(ColorRGB const& rgb) {
        _red.deep() = rgb.red;
        _green.deep() = rgb.green;
        _blue.deep() = rgb.blue;
    }

    void clear() {
        fill(0, 0, 0);
    }

    void shift(Index num, ColorRGB const& fill=BLACK);
    void left(Size num=1, ColorRGB const& fill=BLACK);
    void right(Size num=1, ColorRGB const& fill=BLACK);

    ndarray::Array<float, 1, 1> brightness() const;

    // HSV
    ColorHSV getHSV(Index index) const { return ColorHSV(operator[](index)); }
    ndarray::Array<float, 2, 1> getHSV() const;
    void setHSV(ndarray::Array<float, 2, 1> const& hsv);
    void setHSV(
        ndarray::Array<float, 1, 1> const& hue,
        ndarray::Array<float, 1, 1> const& saturation,
        ndarray::Array<float, 1, 1> const& value
    );

    Collection& getStrips() { return _strips; }
    Collection getStrips() const { return _strips; }

  private:
    Collection _strips;
    Size _numPixels;
    Size _numStrips;
    Array _red;
    Array _green;
    Array _blue;
};

}  // namespace light_show

#endif  // include guard
