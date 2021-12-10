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


LedStripSet::Collection extractStrips(std::vector<LedStripSet> const& stripSets) {
    std::size_t const num = std::accumulate(
        stripSets.begin(), stripSets.end(), 0UL,
        [](std::size_t num, LedStripSet const& ss) { return num + ss.size(); });
    LedStripSet::Collection strips;
    strips.reserve(num);
    for (auto const& ss : stripSets) {
        auto const tt = ss.getStrips();
        strips.insert(strips.end(), tt.begin(), tt.end());
    }
    return strips;
}


}  // anonymous namespace


LedStripSet::LedStripSet(Collection strips)
  : _strips(std::move(strips)),
    _numPixels(std::accumulate(_strips.begin(), _strips.end(), 0UL,
                                [](std::size_t num, auto const& ss) { return num + ss.size(); })),
    _numStrips(_strips.size()),
    _red(transform(_strips, [](LedStrip const& strip) {
        return Array::Array(strip.getRed().deep()); })),
    _green(transform(_strips, [&](LedStrip const& strip) {
        return Array::Array(strip.getGreen().deep()); })),
    _blue(transform(_strips, [&](LedStrip const& strip) {
        return Array::Array(strip.getBlue().deep()); })) {
        if (_strips.size() == 0) {
            throw std::runtime_error("No strips provided");
        }
    }


LedStripSet::LedStripSet(std::vector<LedStripSet> const& stripSets)
  : LedStripSet(LedStripSet::Collection(extractStrips(stripSets))) {}


LedStripSet LedStripSet::slice(Index start, Index stop, Index step) {
    Collection strips;
    strips.reserve(_numStrips);
    for (Size ii = 0; ii < _numStrips; ++ii) {
        LedStrip & ss = _strips[ii];
        Index const size = ss.size();
        if (start < size) {
            strips.emplace_back(ss.slice(start, std::min(stop, Index(ss.size())), step));
        }
        start -= size;
        if (start < 0) {
            break;
        }
        stop -= size;
    }
    return LedStripSet(strips);
}


bool LedStripSet::isOn() const {
    bool const red = std::any_of(_red.begin(), _red.end(), [](Pixel pp) { return pp > 0; });
    bool const green = std::any_of(_green.begin(), _green.end(), [](Pixel pp) { return pp > 0; });
    bool const blue = std::any_of(_blue.begin(), _blue.end(), [](Pixel pp) { return pp > 0; });
    return red || green || blue;
}


void LedStripSet::left(Size num, ColorRGB const& fillColor) {
    if (num >= _numPixels) {
        fill(fillColor);
        return;
    }
    std::copy(cbegin() + num, cend(), begin());
    std::fill(end() - num, end(), fillColor);
}


void LedStripSet::right(Size num, ColorRGB const& fillColor) {
    if (num >= _numPixels) {
        fill(fillColor);
        return;
    }
    std::copy(std::make_reverse_iterator(cend() - num), std::make_reverse_iterator(cbegin()),
              std::make_reverse_iterator(end()));
    std::fill(begin(), begin() + num, fillColor);
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
