#ifndef LIGHT_SHOW_LEDSTRIP_H
#define LIGHT_SHOW_LEDSTRIP_H

#include "ndarray.h"
#include "ndarray/Vector.h"

#include "light_show/config.h"
#include "light_show/colors.h"
#include "light_show/iterator.h"

namespace light_show {


class LedStrip final {
  public:
    using Array = ndarray::Array<Pixel, 1, 1>;
    using iterator = Iterator<LedStrip, ColorRGB, ColorRGBRef, int>;
    using const_iterator = Iterator<LedStrip const, ColorRGB const, ColorRGBRef const, int>;

    LedStrip(int num, ColorPixel * data
        ) : _num(num), _data(data) {
        auto const shape = ndarray::makeVector(num);
        auto const stride = ndarray::makeVector(4);
        _blue = ndarray::external(data + 0, shape, stride);
        _green = ndarray::external(data + 1, shape, stride);
        _red = ndarray::external(data + 2, shape, stride);
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

    ColorRGB get(int index) const { return operator[](index); }
    ColorRGBRef get(int index) { return operator[](index); }
    Array & getRed() { return _red; }
    Array const& getRed() const { return _red; }
    Array & getGreen() { return _green; }
    Array const& getGreen() const { return _green; }
    Array & getBlue() { return _blue; }
    Array const& getBlue() const { return _blue; }

    void set(int index, ColorRGB rgb) { operator[](index) = rgb; }
    void set(int index, ColorHSV hsv) { operator[](index) = hsv.toRGB(); }
    void set(int index, Pixel red, Pixel green, Pixel blue) {
        operator[](index) = ColorRGB(red, green, blue);
    }
    void setRed(Array const& array) { _red.deep() = array; }
    void setGreen(Array const& array) { _green.deep() = array; }
    void setBlue(Array const& array) { _blue.deep() = array; }

    int size() const {
        return _num;
    }

    ColorRGB operator[](int index) const {
        if (index < 0) {
            index += _num;
        }
        return ColorRGB(_red[index], _green[index], _blue[index]);
    }
    ColorRGBRef operator[](int index) {
        if (index < 0) {
            index += _num;
        }
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
        for (int ii = 0; ii < _num; ++ii) {
            brightness[ii] = std::max({_red[ii], _green[ii], _blue[ii]})/255.0;
        }
        return brightness;
    }

    // HSV
    ColorHSV getHSV(int index) const { return ColorHSV(operator[](index)); }
    ndarray::Array<float, 2, 1> getHSV() const {
        ndarray::Array<float, 2, 1> result = ndarray::allocate(_num, 3);
        for (int ii = 0; ii < _num; ++ii) {
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
        for (int ii = 0; ii < _num; ++ii) {
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
        for (int ii = 0; ii < _num; ++ii) {
            operator[](ii) = ColorHSV(hue[ii], saturation[ii], value[ii]).toRGB();
        }
    }

  private:
    int _num;
    ColorPixel * _data;
    Array _blue, _green, _red;
};


}  // namespace light_show

#endif  // include guard
