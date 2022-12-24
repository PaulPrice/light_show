from typing import Iterator, Union
from itertools import cycle

import numpy as np

from .colors import ColorRGB, ColorRGBRef, ColorHSV

Color = Union[ColorRGB, ColorRGBRef, ColorHSV]
ColorGenerator = Iterator[Color]

__all__ = ("RandomHSV", "Rainbow", "Christmas")


class RandomHSV:
    def __init__(self, *, seed: int = 0, value: float = 1.0) -> None:
        self.rng = np.random.default_rng(seed)
        self.value = value

    def __iter__(self) -> "RandomHSV":
        return self

    def __next__(self) -> ColorHSV:
        return ColorHSV(hue=self.rng.uniform(), saturation=1.0, value=self.value)


class Rainbow:
    def __init__(self, num: int, *, start: int = 0, value: float = 1.0) -> None:
        self.num = num
        self.value = value
        self.index = start
        self._colors = [ColorHSV(hh, 1.0, value) for hh in np.linspace(0, 1, num, endpoint=False)]

    def __iter__(self) -> "Rainbow":
        return self

    def __next__(self) -> ColorHSV:
        color = self._colors[self.index]
        self.index = (self.index + 1) % len(self.num)
        return color


class Christmas:
    def __init__(self) -> None:
        self.colors = [
            ColorRGB(255, 0, 0),
            ColorRGB(0, 255, 0),
        ]
        self._iter = cycle(self.colors)

    def __iter__(self) -> "Christmas":
        return self

    def __next__(self) -> ColorRGB:
        return next(self._iter)
