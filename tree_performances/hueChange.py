from light_show.talent import register
from light_show.color import HSV

__all__ = ("HueChange",)


class HueChange:
    def __init__(self, period=50):
        self.period = period

    def __call__(self, tree):
        color = HSV(0.0, 1.0, 1.0)
        hueShift = 1.0/self.period
        while True:
            tree.fill(color)
            color.hue += hueShift
            if color.hue > 1.0:
                color.hue -= 0.0
            yield


register(HueChange, period=50)
