#include "light_show/LedStripSet.h"

namespace light_show {

namespace {

template <typename Container, typename UnaryFunction>
auto transform(Container const& container, UnaryFunction func) {
    std::vector<decltype(func(*container.begin()))> result;
    result.reserve(container.size());
    std::transform(container.begin(), container.end(), std::back_inserter(result), func);
    return result;
}

}  // anonymous namespace


LedStripSet::LedStripSet(Collection & strips)
  : _strips(strips),
    _numPixels(std::accumulate(strips.begin(), strips.end(), 0UL,
                                [](std::size_t num, auto const& ss) { return num + ss.size(); })),
    _numStrips(strips.size()),
    _red(transform(strips, [](LedStrip const& strip) {
        return Array::Array(strip.getRed().deep()); })),
    _green(transform(strips, [&](LedStrip const& strip) {
        return Array::Array(strip.getGreen().deep()); })),
    _blue(transform(strips, [&](LedStrip const& strip) {
        return Array::Array(strip.getBlue().deep()); })) {
        if (strips.size() == 0) {
            throw std::runtime_error("No strips provided");
        }
    }


bool LedStripSet::isOn() const {
    bool const red = std::any_of(_red.begin(), _red.end(), [](Pixel pp) { return pp > 0; });
    bool const green = std::any_of(_green.begin(), _green.end(), [](Pixel pp) { return pp > 0; });
    bool const blue = std::any_of(_blue.begin(), _blue.end(), [](Pixel pp) { return pp > 0; });
    return red || green || blue;
}


ndarray::Array<float, 1, 1> LedStripSet::brightness() const {
    ndarray::Array<float, 1, 1> brightness = ndarray::allocate(_numPixels);
    auto iter = begin();
    for (Size ii = 0; ii < _numPixels; ++ii, ++iter) {
        brightness[ii] = std::max({(*iter).red, (*iter).green, (*iter).blue})/255.0;
    }
    return brightness;
}


ndarray::Array<float, 2, 1> LedStripSet::getHSV() const {
    ndarray::Array<float, 2, 1> result = ndarray::allocate(_numPixels, 3);
    auto iter = begin();
    for (Size ii = 0; ii < _numPixels; ++ii, ++iter) {
        ColorHSV hsv{*iter};
        result[ii][0] = hsv.hue;
        result[ii][1] = hsv.saturation;
        result[ii][2] = hsv.value;
    }
    return result;
}


void LedStripSet::setHSV(ndarray::Array<float, 2, 1> const& hsv) {
    if (hsv.getShape()[0] != _numPixels) {
        throw std::length_error("Incorrect length");
    }
    if (hsv.getShape()[1] != 3) {
        throw std::length_error("Incorrect width");
    }
    auto iter = begin();
    for (Size ii = 0; ii < _numPixels; ++ii, ++iter) {
        *iter = ColorHSV(hsv[ii][0], hsv[ii][1], hsv[ii][2]).toRGB();
    }
}


void LedStripSet::setHSV(
    ndarray::Array<float, 1, 1> const& hue,
    ndarray::Array<float, 1, 1> const& saturation,
    ndarray::Array<float, 1, 1> const& value
) {
    if (hue.size() != _numPixels || saturation.size() != _numPixels || value.size() != _numPixels) {
        throw std::length_error("Incorrect length");
    }
    auto iter = begin();
    for (Size ii = 0; ii < _numPixels; ++ii, ++iter) {
        *iter = ColorHSV(hue[ii], saturation[ii], value[ii]).toRGB();
    }
}


}  // namespace light_show
