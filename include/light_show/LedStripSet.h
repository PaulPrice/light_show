#ifndef LIGHT_SHOW_LEDSTRIPSET_H
#define LIGHT_SHOW_LEDSTRIPSET_H

#include <algorithm>
#include <exception>
#include <numeric>

#include "ndarray.h"

#include "light_show/LedStrip.h"
#include "light_show/ConcatenatedArrays.h"

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

class LedStripSet final {
    template <typename ArrayIterator>
    class LedStripSetIterator {
      public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = ArrayIterator::difference_type;
        using value_type = ColorRGB;
        using reference = ColorRGBRef;

        LedStripSetIterator(ArrayIterator red, ArrayIterator green, ArrayIterator blue)
          : _red(red), _green(green), _blue(blue) {}
        reference operator*() const { return ColorRGBRef(*_red, *_green, *_blue); }
        LedStripSetIterator& operator++() {
            ++_red;
            ++_green;
            ++_blue;
            return *this;
        }
        LedStripSetIterator operator++(int) { Iterator tmp = *this; ++index; return tmp; }

        friend bool operator==(const LedStripSetIterator& lhs, const LedStripSetIterator& rhs) {
            return lhs._red == rhs._red && lhs._green == rhs._green && lhs._blue == rhs._blue;
        }
        friend bool operator!=(const LedStripSetIterator& lhs, const LedStripSetIterator& rhs) {
            return !(lhs == rhs);
        }

      private:
        Array::iterator _red;
        Array::iterator _green;
        Array::iterator _blue;
    };

  public:
    using Collection = std::vector<LedStrip>;
    using Array = ConcatenatedArrays<Pixel>;
    using iterator = LedStripSetIterator<Array::iterator>;
    using const_iterator = LedStripSetIterator<Array::const_iterator>;

    LedStripSet(Collection & strips)
      : _strips(strips),
        _numPixels(std::accumulate(strips.begin(), strips.end(), 0UL,
                                   [](std::size_t num, auto const& ss) { return num + ss.size(); })),
        _numStrips(strips.size()),
        _red(transform(strips, [](LedStrip const& strip) { return strip.getRed(); })),
        _green(transform(strips, [](LedStrip const& strip) { return strip.getGreen(); })),
        _blue(transform(strips, [](LedStrip const& strip) { return strip.getBlue(); })) {
            if (strips.size() == 0) {
                throw std::runtime_error("No strips provided");
            }
        }

    ~LedStripSet() {}
    LedStripSet(LedStripSet const&) = default;
    LedStripSet(LedStripSet &&) = default;
    LedStripSet & operator=(LedStripSet const&) = default;
    LedStripSet & operator=(LedStripSet &&) = default;

    iterator begin() { return iterator(_red.begin(), _green.begin(), _blue.begin()); }
    const_iterator begin() const { return const_iterator(_red.begin(), _green.begin(), _blue.begin()); }
    iterator end() { return iterator(_red.end(), _green.end(), _blue.end()); }
    const_iterator end() const { return const_iterator(_red.end(), _green.end(), _blue.end()); }

    ColorRGB get(int index) const { return operator[](index); }
    ColorRGBRef get(int index) { return operator[](index); }
    Array & getRed() { return _red; }
    Array const& getRed() const { return _red; }
    Array & getGreen() { return _green; }
    Array const& getGreen() const { return _green; }
    Array & getBlue() { return _blue; }
    Array const& getBlue() const { return _blue; }

    void set(int index, ColorRGB const& rgb) { operator[](index) = rgb; }
    void set(int index, ColorHSV const& hsv) { operator[](index) = hsv.toRGB(); }
    void set(int index, Pixel red, Pixel green, Pixel blue) {
        operator[](index) = ColorRGB(red, green, blue);
    }
    void setRed(Array const& array) { _red.deep() = array; }
    void setGreen(Array const& array) { _green.deep() = array; }
    void setBlue(Array const& array) { _blue.deep() = array; }

    int size() const {
        return _numPixels;
    }

    ColorRGB operator[](int index) const {
        index = checkIndex(index, _numPixels);
        return ColorRGB(_red[index], _green[index], _blue[index]);
    }
    ColorRGBRef operator[](int index) {
        index = checkIndex(index, _numPixels);
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
        ndarray::Array<float, 1, 1> brightness = ndarray::allocate(_numPixels);
        auto iter = begin();
        for (int ii = 0; ii < _numPixels; ++ii, ++iter) {
            brightness[ii] = std::max({(*iter).red, (*iter).green, (*iter).blue})/255.0;
        }
        return brightness;
    }

    // HSV
    ColorHSV getHSV(int index) const { return ColorHSV(operator[](index)); }
    ndarray::Array<float, 2, 1> getHSV() const {
        ndarray::Array<float, 2, 1> result = ndarray::allocate(_numPixels, 3);
        auto iter = begin();
        for (int ii = 0; ii < _numPixels; ++ii, ++iter) {
            ColorHSV hsv{*iter};
            result[ii][0] = hsv.hue;
            result[ii][1] = hsv.saturation;
            result[ii][2] = hsv.value;
        }
        return result;
    }
    void setHSV(ndarray::Array<float, 2, 1> const& hsv) {
        if (hsv.getShape()[0] != _numPixels) {
            throw std::length_error("Incorrect length");
        }
        if (hsv.getShape()[1] != 3) {
            throw std::length_error("Incorrect width");
        }
        auto iter = begin();
        for (int ii = 0; ii < _numPixels; ++ii, ++iter) {
            *iter = ColorHSV(hsv[ii][0], hsv[ii][1], hsv[ii][2]).toRGB();
        }
    }
    void setHSV(
        ndarray::Array<float, 1, 1> const& hue,
        ndarray::Array<float, 1, 1> const& saturation,
        ndarray::Array<float, 1, 1> const& value
    ) {
        if (hue.size() != _numPixels || saturation.size() != _numPixels || value.size() != _numPixels) {
            throw std::length_error("Incorrect length");
        }
        auto iter = begin();
        for (int ii = 0; ii < _numPixels; ++ii, ++iter) {
            *iter = ColorHSV(hue[ii], saturation[ii], value[ii]).toRGB();
        }
    }

  private:
    Collection _strips;
    std::size_t _numPixels;
    std::size_t _numStrips;
    Array _red;
    Array _green;
    Array _blue;
};

}  // namespace light_show

#endif  // include guard
