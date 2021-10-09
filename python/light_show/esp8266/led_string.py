try:
    # Micropython
    from esp import neopixel_write
    import ulab as np
    from ulab import numerical
    clip = np.compare.clip
    maxFunc = numerical.max
except ImportError:
    # Regular python
    esp = None
    import numpy as np
    clip = np.clip
    maxFunc = np.max

from .color import RGB, BLACK

__all__ = ("LedString")


class LedString:
    def __init__(self, num, pin=None, order="grb", positions=None):
        self.pin = pin
        if pin is not None:
            pin.init(pin.OUT)
        self.order = order
        if positions is None:
            positions = {}
        self.positions = positions

        self.num = num
        self._red0 = order.lower().find("r")
        self._green0 = order.lower().find("g")
        self._blue0 = order.lower().find("b")

        self._state = np.zeros((num, 3), dtype=np.uint8)

    @property
    def red(self):
        return self._state[:, self._red0]

    @property
    def green(self):
        return self._state[:, self._green0]

    @property
    def blue(self):
        return self._state[:, self._blue0]

    def copy(self):
        copy = LedString(self.num, self.pin)
        copy[:] = self[:]
        return copy

    def __len__(self):
        return self.num

    def __setitem__(self, index, color):
        try:
            red, green, blue = color
        except Exception:
            rgb = color.toRgb()
            red = rgb.red
            green = rgb.green
            blue = rgb.blue
        if isinstance(index, str):
            index = self.positions[index]
        self._state[index, self._red0] = red
        self._state[index, self._green0] = green
        self._state[index, self._blue0] = blue

    def __getitem__(self, index):
        if isinstance(index, int):
            return RGB(self.red[index], self.green[index], self.blue[index])
        if isinstance(index, slice):
            return self.red[index], self.green[index], self.blue[index]
        if isinstance(index, str):
            index = self.positions[index]
            return self.red[index], self.green[index], self.blue[index]
        raise IndexError("Unrecognised index (not int or slice or str)")

    def write(self):
        neopixel_write(self.pin, self._state.flatten(), True)

    def plot(self, show=True):
        import matplotlib.pyplot as plt
        xx = list(range(self.num))
        yy = [0]*self.num
        cc = [(rr/255, gg/255, bb/255) for rr, gg, bb in zip(self.red, self.green, self.blue)]

        artist = plt.scatter(xx, yy, marker="o", c=cc)
        plt.axis("off")
        if show:
            plt.show()
        return artist

    def isOn(self):
        return self.brightness > 0

    def off(self):
        self.clear()
        self.write()

    def fill(self, color):
        self[:] = color.toRgb()

    def clear(self):
        self[:] = BLACK

    @property
    def brightness(self):
        return maxFunc(self._state)/255

    def dimmer(self, fraction):
        brightness = self.brightness
        if brightness == 0:
            return
        # XXX add gamma correction
        # multiplier = fraction**2/brightness ???
        multiplier = fraction/brightness
        self._state[:, self._red0] = clip(self.red*multiplier, 0, 255)
        self._state[:, self._green0] = clip(self.green*multiplier, 0, 255)
        self._state[:, self._blue0] = clip(self.blue*multiplier, 0, 255)

    def left(self, num=1, fill=None):
        if fill is None:
            fill = BLACK
        self[:-num] = self[num:]
        self[-num:] = fill

    def right(self, num=1, fill=None):
        if fill is None:
            fill = BLACK
        self[num:] = self[:-num]
        self[:num] = fill

    def setPosition(self, name, index):
        self.positions[name] = index
