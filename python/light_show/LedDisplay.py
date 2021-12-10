__all__ = ("LedDisplay",)

from typing import Iterable

import numpy as np
from numpy.typing import ArrayLike

from .LedStripSet import LedStripSet
from .LedController import LedController


# Want to be able to:
# * Select groups of strips, e.g., either side of a chevron
# * Know 3D position of all LEDs, e.g., top-to-bottom or left-to-right effects
# * Have a single continuous strip, e.g., chasing


__all__ = ("LedDisplay",)


class LedDisplay:
    def __init__(self, controller: LedController, groups: Iterable[slice],
                 reversed: Iterable[bool], xyz: ArrayLike, strip: Iterable[slice]):
        self.controller = controller
        self.pixels = controller.getAll()
        self.groups = [self.pixels[ss] for ss in groups]
        self.reversed = np.array(reversed)
        self.xyz = xyz
        self.strip = LedStripSet([self.pixels[ss] for ss in strip])

    def __len__(self):
        return len(self.pixels)

    def plot(self, axes, show=True):
        import matplotlib.pyplot as plt
        color = np.vstack([self.pixels.red.array/255.0, self.pixels.green.array/255.0,
                           self.pixels.blue.array/255.0]).T
        scatter = axes.scatter(self.xyz[:, 0], self.xyz[:, 1], marker=".", c=color)
        plt.axis("off")
        return scatter

    def brightness(self):
        return self.pixels.brightness()

    def isOn(self):
        return self.pixels.isOn()

    def off(self):
        self.clear()
        self.render()

    def clear(self):
        self.pixels.clear()

    def fill(self, color):
        self.pixels.fill(color)

    def render(self):
        self.controller.render()

    def left(self):
        for gg, rr in zip(self.groups, self.reversed):
            if rr:
                gg.right()
            else:
                gg.left()

    def right(self):
        for gg, rr in zip(self.groups, self.reversed):
            if rr:
                gg.left()
            else:
                gg.right()
