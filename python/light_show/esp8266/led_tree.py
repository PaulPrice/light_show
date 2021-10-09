from .color import BLACK

__all__ = ("LedTree", "speedTest")


class LedTree:
    """LedStrings are oriented vertically as a tree"""
    def __init__(self, strings, bottomUp=True):
        self._strings = strings
        self._bottomUp = bottomUp
        self.length = max(len(ss) for ss in strings)
        for ss in strings:
            ss.setPosition("TOP", -1 if bottomUp else 0)
            ss.setPosition("BOTTOM", 0 if bottomUp else -1)

    def __len__(self):
        return len(self._strings)

    def __getitem__(self, index):
        return self._strings[index]

    def set(self, index, value):
        for ss in self._strings:
            ss[index] = value

    def __iter__(self):
        return iter(self._strings)

    def write(self):
        for ss in self:
            ss.write()

    def plot(self, show=True):
        import matplotlib.pyplot as plt
        xx = []
        yy = []
        cc = []
        for ii, ss in enumerate(self._strings):
            length = len(ss)
            yy += list(range(length))
            xx += [ii]*length
            cc += [(rr/255, gg/255, bb/255) for rr, gg, bb in zip(ss.red, ss.green, ss.blue)]
        artist = plt.scatter(xx, yy, marker="o", c=cc)
        plt.axis("off")
        if show:
            plt.show()
        return artist

    def isOn(self):
        return any(ss.brightness > 0 for ss in self)

    def off(self):
        self.clear()
        self.write()

    def clear(self):
        self.fill(BLACK)

    def fill(self, color):
        rgb = color.toRgb()
        for ss in self:
            ss[:] = rgb

    @property
    def brightness(self):
        return max(ss.brightness for ss in self)

    def dimmer(self, fraction):
        raise NotImplementedError("Will implement this when I figure out how best to do it")

    def left(self, num=1, fill=None):
        if fill is None:
            fill = BLACK
        for ii in range(len(self) - num):
            self._strings[ii][:] = self._strings[ii + num][:]
        for ii in range(num):
            self._strings[-ii][:] = fill

    def right(self, num=1, fill=None):
        if fill is None:
            fill = BLACK
        for ii in reversed(range(len(self) - num)):
            self._strings[ii + num][:] = self._strings[ii][:]
        for ii in range(num):
            self._strings[ii][:] = fill

    def _up(self, num=1, fill=None):
        if fill is None:
            fill = BLACK
        for ss in self._strings:
            ss.right(num=num, fill=fill)

    def _down(self, num=1, fill=None):
        if fill is None:
            fill = BLACK
        for ss in self._strings:
            ss.left(num=num, fill=fill)

    def up(self, num=1, fill=None):
        if self._bottomUp:
            self._up(num=num, fill=fill)
        else:
            self._down(num=num, fill=fill)

    def down(self, num=1, fill=None):
        if self._bottomUp:
            self._down(num=num, fill=fill)
        else:
            self._up(num=num, fill=fill)
