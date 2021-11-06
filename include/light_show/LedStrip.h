#ifndef LIGHT_SHOW_LEDSTRIP_H
#define LIGHT_SHOW_LEDSTRIP_H

#include <limits>

#include "ndarray.h"
#include "ndarray/Vector.h"

#include "light_show/config.h"
#include "light_show/indexing.h"
#include "light_show/colors.h"
#include "light_show/iterator.h"

namespace light_show {


class LedStrip final {
  public:
    using Size = std::size_t;
    using Index = std::ptrdiff_t;
    using Array = ndarray::Array<Pixel, 1, 0>;
    using iterator = IndexBasedIterator<LedStrip, ColorRGB, ColorRGBRef, int>;
    using const_iterator = IndexBasedIterator<LedStrip const, ColorRGB const, ColorRGBRef const, int>;

    LedStrip(Array & red, Array & green, Array & blue);

    LedStrip(Size num, ColorPixel * data);

    ~LedStrip() {}
    LedStrip(LedStrip const&) = default;
    LedStrip(LedStrip &&) = default;
    LedStrip & operator=(LedStrip const&) = default;
    LedStrip & operator=(LedStrip &&) = default;

    iterator begin() { return iterator(*this, 0); }
    const_iterator begin() const { return const_iterator(*this, 0); }
    iterator end() { return iterator(*this, _num); }
    const_iterator end() const { return const_iterator(*this, _num); }

    ColorRGB get(Index index) const { return operator[](index); }
    ColorRGBRef get(Index index) { return operator[](index); }
    Array & getRed() { return _red; }
    Array const& getRed() const { return _red; }
    Array & getGreen() { return _green; }
    Array const& getGreen() const { return _green; }
    Array & getBlue() { return _blue; }
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
        return _num;
    }

    ColorRGB operator[](Index index) const {
        index = checkIndex(index, _num);
        return ColorRGB(_red[index], _green[index], _blue[index]);
    }
    ColorRGBRef operator[](Index index) {
        index = checkIndex(index, _num);
        return ColorRGBRef(_red[index], _green[index], _blue[index]);
    }

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

  private:
    std::size_t _num;
    Array _blue, _green, _red;
};


}  // namespace light_show

#endif  // include guard
