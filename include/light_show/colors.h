#ifndef LIGHT_SHOW_COLORS_H
#define LIGHT_SHOW_COLORS_H

#include <limits>
#include <algorithm>
#include "light_show/config.h"

namespace light_show {


struct ColorRGBRef;
struct ColorHSV;

struct ColorRGB {
    ColorRGB(Pixel red_, Pixel green_, Pixel blue_) : red(red_), green(green_), blue(blue_) {}
    ColorRGB(ColorRGBRef other);
    ColorRGB(ColorHSV hsv);

    void operator=(ColorRGB const& other) {
        red = other.red;
        green = other.green;
        blue = other.blue;
    }
    void operator=(ColorHSV const& hsv);

    Pixel max() const { return std::max({red, green, blue}); }
    Pixel min() const { return std::min({red, green, blue}); }

    friend std::ostream& operator<<(std::ostream& os, ColorRGB const& color) {
        os << "ColorRGB(" << int(color.red) << "," << int(color.green) << "," << int(color.blue) << ")";
        return os;
    }

    Pixel red;
    Pixel green;
    Pixel blue;
};


struct ColorRGBRef {
    ColorRGBRef(Pixel& red_, Pixel& green_, Pixel& blue_) : red(red_), green(green_), blue(blue_) {}
    void operator=(ColorRGB const& other) {
        red = other.red;
        green = other.green;
        blue = other.blue;
    }
    void operator=(ColorRGBRef const& other) {
        red = other.red;
        green = other.green;
        blue = other.blue;
    }
    void operator=(ColorHSV const& other);

    friend std::ostream& operator<<(std::ostream& os, ColorRGBRef const& color) {
        os << "ColorRGBRef(" << int(color.red) << "," << int(color.green) << "," << int(color.blue) << ")";
        return os;
    }

    Pixel & red;
    Pixel & green;
    Pixel & blue;
};

constexpr Pixel PIXEL_MAX = std::numeric_limits<Pixel>::max();

ColorRGB const WHITE{255, 255, 255};
ColorRGB const BLACK{0, 0, 0};
ColorRGB const RED{255, 0, 0};
ColorRGB const ORANGE{255, 165, 0};
ColorRGB const YELLOW{255, 255, 0};
ColorRGB const GREEN{0, 255, 0};
ColorRGB const BLUE{0, 0, 255};
ColorRGB const MAGENTA{255, 0, 255};
ColorRGB const CYAN{0, 255, 255};
ColorRGB const PINK{255, 20, 147};
ColorRGB const BROWN{210, 105, 30};
ColorRGB const SKY{0, 191, 255};
ColorRGB const AQUA{127, 255, 212};
ColorRGB const CRIMSON{220, 20, 60};
ColorRGB const GOLD{255, 215, 0};


struct ColorHSV {
    ColorHSV(float hue_, float saturation_, float value_)
        : hue(hue_), saturation(saturation_), value(value_) {}
    ColorHSV(ColorRGB rgb) {
        value = rgb.max();
        Pixel const min = rgb.min();
        if (min == value) {
            hue = 0.0;
            saturation = 0.0;
            return;
        }
        float const diff = value - min;
        saturation = diff/value;
        float const rr = (value - rgb.red)/diff;
        float const gg = (value - rgb.green)/diff;
        float const bb = (value - rgb.blue)/diff;
        if (rgb.red == value) {
            hue = (bb - gg)/6.0;
        } else if (rgb.green == value) {
            hue = (2.0 + rr - bb)/6.0;
        } else {
            hue = (4.0 + gg - rr)/6.0;
        }
        value /= float(PIXEL_MAX);
    }

    ColorRGB toRGB() const {
        if (saturation == 0.0) {
            return ColorRGB(value, value, value);
        }
        int ii = hue*6.0;  // truncate
        float const ff = hue*6.0 - ii;  // remainder
        float const pixelValue = value*PIXEL_MAX;
        float const pp = pixelValue*(1.0 - saturation);
        float const qq = pixelValue*(1.0 - saturation*ff);
        float const tt = pixelValue*(1.0 - saturation*(1.0 - ff));
        ii = ii % 6;
        switch (ii) {
          case 0: return ColorRGB(pixelValue, tt, pp);
          case 1: return ColorRGB(qq, pixelValue, pp);
          case 2: return ColorRGB(pp, pixelValue, tt);
          case 3: return ColorRGB(pp, qq, pixelValue);
          case 4: return ColorRGB(tt, pp, pixelValue);
          case 5: return ColorRGB(pixelValue, pp, qq);
          default:
            abort();
        }
    }

    void operator=(ColorHSV const& other) {
        hue = other.hue;
        saturation = other.saturation;
        value = other.value;
    }
    void operator=(ColorRGB const& other) { operator=(ColorHSV(other)); }

    friend std::ostream& operator<<(std::ostream& os, ColorHSV const& color) {
        os << "ColorHSV(" << color.hue << "," << color.saturation << "," << color.value << ")";
        return os;
    }

    float hue;
    float saturation;
    float value;
};



inline
ColorRGB::ColorRGB(ColorRGBRef other) : red(other.red), green(other.green), blue(other.blue) {}

inline
ColorRGB::ColorRGB(ColorHSV hsv) : ColorRGB(hsv.toRGB()) {}

inline
void ColorRGB::operator=(ColorHSV const& hsv) { operator=(hsv.toRGB()); }

inline
void ColorRGBRef::operator=(ColorHSV const& other) { operator=(other.toRGB()); }

}  // namespace light_show


#endif  // include guard
