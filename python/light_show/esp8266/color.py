from .utils import randFloat
from .hsv import rgb_to_hsv, hsv_to_rgb

__all__ = ("RGB", "HSV", "randomColor", "WHITE", "BLACK", "RED", "ORANGE", "YELLOW", "GREEN", "BLUE",
           "MAGENTA", "CYAN", "PINK", "BROWN", "SKY", "AQUA", "CRIMSON", "GOLD",)


class RGB:
    def __init__(self, red, green, blue):
        self.red = red
        self.green = green
        self.blue = blue

    def __iter__(self):
        return iter([self.red, self.green, self.blue])

    def copy(self):
        return RGB(self.red, self.green, self.blue)

    def toRgb(self, copy=False):
        return self.copy() if copy else self

    def toHsv(self, copy=False):
        hue, sat, value = rgb_to_hsv(self.red/255, self.green/255, self.blue/255)
        return HSV(hue, sat, value)

    def __repr__(self):
        return "RGB(" + ",".join([str(self.red), str(self.green), str(self.blue)]) + ")"


class HSV:
    def __init__(self, hue, sat, value):
        self.hue = hue
        self.sat = sat
        self.value = value

    def copy(self):
        return HSV(self.hue, self.sat, self.value)

    def toRgb(self, copy=False):
        red, green, blue = hsv_to_rgb(self.hue, self.sat, self.value)
        return RGB(int(red*255), int(green*255), int(blue*255))

    def toHsv(self, copy=False):
        return self.copy() if copy else self

    def __repr__(self):
        return "HSV(" + ",".join([str(self.hue), str(self.sat), str(self.value)]) + ")"

    def dimmer(self, value):
        self.value = max(self.value - value, 0.0)

    def brighter(self, value):
        self.value = min(self.value + value, 1.0)


def randomColor():
    return HSV(randFloat(), 1.0, 1.0)


WHITE = RGB(255, 255, 255)
BLACK = RGB(0, 0, 0)
RED = RGB(255, 0, 0)
ORANGE = RGB(255, 165, 0)
YELLOW = RGB(255, 255, 0)
GREEN = RGB(0, 255, 0)
BLUE = RGB(0, 0, 255)
MAGENTA = RGB(255, 0, 255)
CYAN = RGB(0, 255, 255)
PINK = RGB(255, 20, 147)
BROWN = RGB(210, 105, 30)
SKY = RGB(0, 191, 255)
AQUA = RGB(127, 255, 212)
CRIMSON = RGB(220, 20, 60)
GOLD = RGB(255, 215, 0)
