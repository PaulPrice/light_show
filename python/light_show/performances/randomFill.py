from typing import Iterator
import numpy as np

from light_show import Performance, Config, LedDisplay
from light_show import ColorHSV, BLACK

__all__ = ("RandomFill",)


class RandomFill(Performance):
    def __init__(self, pause):
        self.pause = pause

    @classmethod
    def defaults(cls) -> Config:
        return dict(pause=0.01)

    def __call__(self, display: LedDisplay) -> Iterator[float]:
        num = len(display)
        indices = np.arange(num, dtype=int)
        while True:
            np.random.shuffle(indices)
            display.clear()
            for ii in indices:
                display.strip[ii] = ColorHSV(np.random.uniform(), 1.0, 1.0)
                yield self.pause
            for ii in indices:
                display.strip[ii] = BLACK
                yield self.pause
