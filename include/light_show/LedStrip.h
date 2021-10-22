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
    using iterator = Iterator<LedStrip, ColorRGB, ColorRGBRef, int>;
    using const_iterator = Iterator<LedStrip const, ColorRGB const, ColorRGBRef const, int>;

    LedStrip(Array & red, Array & green, Array & blue)
      : _num(red.size()),
        _blue(blue),
        _green(green),
        _red(red) {
          if (green.size() != _num || blue.size() != _num) {
              throw std::length_error("Size mismatch");
          }
      }

    LedStrip(Size num, ColorPixel * data
        ) : _num(num) {
        auto const shape = ndarray::makeVector(num);
        auto const stride = ndarray::makeVector(4);
        Pixel * pixels = reinterpret_cast<Pixel*>(data);
        _blue = ndarray::external(pixels + 0, shape, stride);
        _green = ndarray::external(pixels + 1, shape, stride);
        _red = ndarray::external(pixels + 2, shape, stride);
        clear();
    }

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

    bool isOn() const {
        bool const red = std::any_of(_red.begin(), _red.end(), [](Pixel pp) { return pp > 0; });
        bool const green = std::any_of(_green.begin(), _green.end(), [](Pixel pp) { return pp > 0; });
        bool const blue = std::any_of(_blue.begin(), _blue.end(), [](Pixel pp) { return pp > 0; });
        return red || green || blue;
    }

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

    ndarray::Array<float, 1, 1> brightness() const {
        ndarray::Array<float, 1, 1> brightness = ndarray::allocate(_num);
        float const norm = 1.0/std::numeric_limits<Pixel>::max();
        for (Size ii = 0; ii < _num; ++ii) {
            brightness[ii] = std::max({_red[ii], _green[ii], _blue[ii]})*norm;
        }
        return brightness;
    }

    // HSV
    ColorHSV getHSV(Index index) const { return ColorHSV(operator[](index)); }
    ndarray::Array<float, 2, 1> getHSV() const {
        ndarray::Array<float, 2, 1> result = ndarray::allocate(_num, 3);
        for (Size ii = 0; ii < _num; ++ii) {
            ColorHSV hsv{operator[](ii)};
            result[ii][0] = hsv.hue;
            result[ii][1] = hsv.saturation;
            result[ii][2] = hsv.value;
        }
        return result;
    }
    void setHSV(ndarray::Array<float, 2, 1> const& hsv) {
        if (hsv.getShape()[0] != _num) {
            throw std::length_error("Incorrect length");
        }
        if (hsv.getShape()[1] != 3) {
            throw std::length_error("Incorrect width");
        }
        for (Size ii = 0; ii < _num; ++ii) {
            operator[](ii) = ColorHSV(hsv[ii][0], hsv[ii][1], hsv[ii][2]).toRGB();
        }
    }
    void setHSV(
        ndarray::Array<float, 1, 1> const& hue,
        ndarray::Array<float, 1, 1> const& saturation,
        ndarray::Array<float, 1, 1> const& value
    ) {
        if (hue.size() != _num || saturation.size() != _num || value.size() != _num) {
            throw std::length_error("Incorrect length");
        }
        for (Size ii = 0; ii < _num; ++ii) {
            operator[](ii) = ColorHSV(hue[ii], saturation[ii], value[ii]).toRGB();
        }
    }

  private:
    std::size_t _num;
    Array _blue, _green, _red;
};


}  // namespace light_show

#endif  // include guard
