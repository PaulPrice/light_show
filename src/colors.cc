#include "light_show/colors.h"

namespace light_show {


constexpr Pixel PIXEL_MAX = std::numeric_limits<Pixel>::max();

ColorRGB::ColorRGB(ColorRGBRef other) : red(other.red), green(other.green), blue(other.blue) {}

ColorRGB::ColorRGB(ColorHSV hsv) : ColorRGB(hsv.toRGB()) {}

ColorRGB& ColorRGB::operator=(ColorHSV const& hsv) { return operator=(hsv.toRGB()); }

std::ostream& operator<<(std::ostream& os, ColorRGB const& color) {
    os << "ColorRGB(" << int(color.red) << "," << int(color.green) << "," << int(color.blue) << ")";
    return os;
}


ColorRGBRef& ColorRGBRef::operator=(ColorHSV const& other) { return operator=(other.toRGB()); }

std::ostream& operator<<(std::ostream& os, ColorRGBRef const& color) {
    os << "ColorRGBRef(" << int(color.red) << "," << int(color.green) << "," << int(color.blue) << ")";
    return os;
}


ColorHSV::ColorHSV(ColorRGB const& rgb) {
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


ColorRGB ColorHSV::toRGB() const {
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

std::ostream& operator<<(std::ostream& os, ColorHSV const& color) {
    os << "ColorHSV(" << color.hue << "," << color.saturation << "," << color.value << ")";
    return os;
}


}  // namespace light_show
