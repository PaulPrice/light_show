#ifndef LIGHT_SHOW_LEDSTRIP_H
#define LIGHT_SHOW_LEDSTRIP_H

#include "light_show/config.h"
#include "light_show/colors.h"

namespace light_show {


class LedStrip final {
  public:
    using Array = ndarray::Array<Pixel, 1, 1>;

    LedStrip(int num, ws2811_led_t * data
        ) : _num(num), _data(data) {
        auto const shape = ndarray::makeVector(num);
        auto const stride = ndarray::makeVector(4);
        _blue = ndarray::external(data + 0, shape, stride);
        _green = ndarray::external(data + 1, shape, stride);
        _red = ndarray::external(data + 2, shape, stride);
        off();
    }

    ~LedStrip() {}
    LedStrip(LedStrip const&) = default;
    LedStrip(LedStrip &&) = default;
    LedStrip & operator=(LedStrip const&) = default;
    LedStrip & operator=(LedStrip &&) = default;

    struct Iterator {
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = int;
        using value_type = RGB;
        using pointer = RGB;
        using reference = RGBRef;

        Iterator(LedStrip & strip_, int index_) : strip(strip_), index(index) {}
        reference operator*() const { return strip[index]; }
        pointer operator->() { return strip[index]; }

        // Prefix increment
        Iterator& operator++() { ++index; return *this; }

        // Postfix increment
        Iterator operator++(int) { Iterator tmp = *this; ++index; return tmp; }

        friend bool operator==(const Iterator& a, const Iterator& b) { return a.index == b.index; };
        friend bool operator!=(const Iterator& a, const Iterator& b) { return a.index != b.index; };

        LedStrip & strip;
        int index;
    };
    Iterator begin() { return Iterator(*this, 0); }
    Iterator end() { return Iterator(*this, _num); }

    RGB get(int index) const { return operator[](index); }
    RGBRef get(int index) { return operator[](index); }
    Array & getRed() { return _red; }
    Array const& getRed() const { return _red; }
    Array & getGreen() { return _green; }
    Array const& getGreen() const { return _green; }
    Array & getBlue() { return _blue; }
    Array const& getBlue() const { return _blue; }

    void set(int index, RGB rgb) { operator[](index) = rgb; }
    void set(int index, HSV hsv) { operator[](index) = hsv.toRGB(); }
    void set(int index, Pixel red, Pixel green, Pixel blue) {
        operator[](index) = RGB(red, green, blue);
    }
    void setRed(Array const& array) { _red.deep() = array; }
    void setGreen(Array const& array) { _green.deep() = array; }
    void setBlue(Array const& array) { _blue.deep() = array; }

    int size() const {
        return _num;
    }

    RGB operator[](int index) const {
        if (index < 0) {
            index += _num;
        }
        return RGB(_red[index], _green[index], _blue[index]);
    }
    RGBRef operator[](int index) {
        if (index < 0) {
            index += _num;
        }
        return RGBRef(_red[index], _green[index], _blue[index]);
    }

    bool isOn() const {
        bool const red = std::any_of(_red.begin(), _red.end(), [](Pixel pp) { return pp > 0; });
        bool const green = std::any_of(_green.begin(), _green.end(), [](Pixel pp) { return pp > 0; });
        bool const blue = std::any_of(_blue.begin(), _blue.end(), [](Pixel pp) { return pp > 0; });
        return red || green || blue;
    }

    void fill(Pixel red, Pixel green, Pixel blue) {
        fill(RGB(red, green, blue));
    }
    void fill(RGB const& rgb) {
        _red.deep() = rgb.red;
        _green.deep() = rgb.green;
        _blue.deep() = rgb.blue;
    }

    void clear() {
        fill(0, 0, 0);
    }

    ndarray::Array<float, 1, 1> brightness() const {
        ndarray::Array<float, 1, 1> brightess = ndarray::allocate(_num);
        for (int ii = 0; ii < _num; ++ii) {
            brightness[ii] = std::max({_red[ii], _green[ii], _blue[ii]})/255.0;
        }
        return brightness;
    }

    // HSV
    HSV getHSV(int index) const { return HSV(operator[](index)); }
    ndarray::Array<float, 2, 1> getHSV() const {
        ndarray::Array<Pixel, 2, 1> result = ndarray::allocate(_num, 3);
        for (Iterator iter = begin(); iter != end(); ++iter) {
            HSV hsv{*iter};
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
            operator[](ii) = HSV(hsv[ii][0], hsv[ii][1], hsv[ii][2]).toRGB();
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
            operator[](ii) = HSV(hue[ii], saturation[ii], value[ii]).toRGB();
        }
    }

  private:
    int _num;
    ColorPixel * _data;
    Array _blue, _green, _red;
};


}  // namespace light_show

#endif  // include guard
