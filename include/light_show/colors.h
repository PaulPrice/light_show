#ifndef LIGHT_SHOW_COLORS_H
#define LIGHT_SHOW_COLORS_H

#include <ostream>
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

    ColorRGB& operator=(ColorRGB const& other) {
        red = other.red;
        green = other.green;
        blue = other.blue;
        return *this;
    }
    ColorRGB& operator=(ColorRGBRef const& other);
    ColorRGB& operator=(ColorHSV const& hsv);

    Pixel max() const { return std::max({red, green, blue}); }
    Pixel min() const { return std::min({red, green, blue}); }

    friend std::ostream& operator<<(std::ostream& os, ColorRGB const& color);

    Pixel red;
    Pixel green;
    Pixel blue;
};


struct ColorRGBRef {
    ColorRGBRef(Pixel& red_, Pixel& green_, Pixel& blue_) : red(red_), green(green_), blue(blue_) {}
    ColorRGBRef& operator=(ColorRGB const& other) {
        red = other.red;
        green = other.green;
        blue = other.blue;
        return *this;
    }
    ColorRGBRef& operator=(ColorRGBRef const& other) {
        red = other.red;
        green = other.green;
        blue = other.blue;
        return *this;
    }
    ColorRGBRef& operator=(ColorHSV const& other);

    friend std::ostream& operator<<(std::ostream& os, ColorRGBRef const& color);

    Pixel & red;
    Pixel & green;
    Pixel & blue;
};


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
    ColorHSV(ColorRGB const& rgb);

    ColorRGB toRGB() const;

    void operator=(ColorHSV const& other) {
        hue = other.hue;
        saturation = other.saturation;
        value = other.value;
    }
    void operator=(ColorRGB const& other) { operator=(ColorHSV(other)); }

    friend std::ostream& operator<<(std::ostream& os, ColorHSV const& color);

    float hue;
    float saturation;
    float value;
};




}  // namespace light_show


#endif  // include guard
