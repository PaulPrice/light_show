#ifndef LIGHT_SHOW_COLORS_H
#define LIGHT_SHOW_COLORS_H

#include "light_show/config.h"

namespace light_show {


struct RGB;
struct HSV;

struct RGBRef {
    RGBRef(Pixel& red_, Pixel& green_, Pixel& blue_) : red(red_), green(green_), blue(blue_) {}
    void operator=(RGB const& other);
    void operator=(RGBRef const& other) {
        red = other.red;
        green = other.green;
        blue = other.blue;
    }
    void operator=(HSV const& other) { operator=(other.toRGB()); }
    Pixel & red;
    Pixel & green;
    Pixel & blue;
};


struct RGB {
    RGB(Pixel red_, Pixel green_, Pixel blue_) : red(red_), green(green_), blue(blue_) {}
    RGB(RGBRef other) : red(other.red), green(other.green), blue(other.blue) {}
    void operator=(RGBRef const& other) {
        red = other.red;
        green = other.green;
        blue = other.blue;
    }
    void operator=(HSV const& other) { operator=(other.toRGB()); }
    Pixel max() const { return std::max({red, green, blue}); }
    Pixel min() const { return std::min({red, green, blue}); }

    Pixel red;
    Pixel green;
    Pixel blue;
};


void RGBRef::operator=(RGB const& other) {
    red = other.red;
    green = other.green;
    blue = other.blue;
}


RGB const WHITE{255, 255, 255};
RGB const BLACK{0, 0, 0};
RGB const RED{255, 0, 0};
RGB const ORANGE{255, 165, 0};
RGB const YELLOW{255, 255, 0};
RGB const GREEN{0, 255, 0};
RGB const BLUE{0, 0, 255};
RGB const MAGENTA{255, 0, 255};
RGB const CYAN{0, 255, 255};
RGB const PINK{255, 20, 147};
RGB const BROWN{210, 105, 30};
RGB const SKY{0, 191, 255};
RGB const AQUA{127, 255, 212};
RGB const CRIMSON{220, 20, 60};
RGB const GOLD{255, 215, 0};


struct HSV {
    HSV(float hue_, float saturation_, float value_) : hue(hue_), saturation(saturation_), value(value_) {}
    HSV(RGB rgb) {
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
        } elif (rgb.green == value) {
            hue = (2.0 + rr - bb)/6.0;
        } else {
            hue = (4.0 + gg - rr)/6.0;
        }
    }

    RGB toRGB() const {
        if (saturation == 0.0) {
            return RGB(value, value, value);
        }
        int ii = hue*6.0;  // truncate
        float const ff = hue*6.0 - ii;  // remainder
        float const pp = value*(1.0 - saturation);
        float const qq = value*(1.0 - saturation*ff);
        float const tt = value*(1.0 - saturation*(1.0 - ff));
        ii = ii % 6;
        switch (ii) {
          case 0: return RGB(value, tt, pp);
          case 1: return RGB(qq, value, pp);
          case 2: return RGB(pp, value, tt);
          case 3: return RGB(pp, qq, value);
          case 4: return RGB(tt, pp, value);
          case 5: return RGB(value, pp, qq);
          default:
            abort();
        }
    }

    float hue;
    float saturation;
    float value;
};


}  // namespace light_show


#endif  // include guard
