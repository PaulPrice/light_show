#include "light_show/LedStrip.h"

namespace light_show {

LedStrip::LedStrip(Array red, Array green, Array blue)
  : _num(red.size()),
    _blue(std::move(blue)),
    _green(std::move(green)),
    _red(std::move(red)) {
        if (_green.size() != _num || _blue.size() != _num) {
            throw std::length_error("Size mismatch");
        }
    }


LedStrip::LedStrip(Size num, ColorPixel * data
  ) : _num(num) {
    auto const shape = ndarray::makeVector(num);
    auto const stride = ndarray::makeVector(4);
    Pixel * pixels = reinterpret_cast<Pixel*>(data);
    _blue = ndarray::external(pixels + 0, shape, stride);
    _green = ndarray::external(pixels + 1, shape, stride);
    _red = ndarray::external(pixels + 2, shape, stride);
    clear();
}


LedStrip LedStrip::slice(Index start, Index stop, Index step) {
    return LedStrip(
        _red[ndarray::view(start, stop, step)],
        _green[ndarray::view(start, stop, step)],
        _blue[ndarray::view(start, stop, step)]
    );
}


bool LedStrip::isOn() const {
    bool const red = std::any_of(_red.begin(), _red.end(), [](Pixel pp) { return pp > 0; });
    bool const green = std::any_of(_green.begin(), _green.end(), [](Pixel pp) { return pp > 0; });
    bool const blue = std::any_of(_blue.begin(), _blue.end(), [](Pixel pp) { return pp > 0; });
    return red || green || blue;
}


void LedStrip::left(Size num, ColorRGB const& fillColor) {
    if (num >= _num) {
        fill(fillColor);
        return;
    }
    std::copy(cbegin() + num, cend(), begin());
    std::fill(end() - num, end(), fillColor);
}


void LedStrip::right(Size num, ColorRGB const& fillColor) {
    if (num >= _num) {
        fill(fillColor);
        return;
    }
    std::copy(std::make_reverse_iterator(cend() - num), std::make_reverse_iterator(cbegin()),
              std::make_reverse_iterator(end()));
    std::fill(begin(), begin() + num, fillColor);
}


ndarray::Array<float, 1, 1> LedStrip::brightness() const {
    ndarray::Array<float, 1, 1> brightness = ndarray::allocate(_num);
    float const norm = 1.0/std::numeric_limits<Pixel>::max();
    for (Size ii = 0; ii < _num; ++ii) {
        brightness[ii] = std::max({_red[ii], _green[ii], _blue[ii]})*norm;
    }
    return brightness;
}


ndarray::Array<float, 2, 1> LedStrip::getHSV() const {
    ndarray::Array<float, 2, 1> result = ndarray::allocate(_num, 3);
    for (Size ii = 0; ii < _num; ++ii) {
        ColorHSV hsv{operator[](ii)};
        result[ii][0] = hsv.hue;
        result[ii][1] = hsv.saturation;
        result[ii][2] = hsv.value;
    }
    return result;
}


void LedStrip::setHSV(ndarray::Array<float, 2, 1> const& hsv) {
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


void LedStrip::setHSV(
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


}  // namespace light_show
