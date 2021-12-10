from typing import Iterator
import numpy as np

from light_show import Performance, Config, LedDisplay
from light_show import ColorHSV, BLACK

__all__ = ("Chase",)


class Chase(Performance):
    def __init__(self, pause, multiple, gap):
        self.pause = pause
        self.multiple = multiple
        self.gap = gap

    @classmethod
    def defaults(cls) -> Config:
        return dict(pause=0.01, multiple=2, gap=1)

    def __call__(self, display: LedDisplay) -> Iterator[float]:
        length = len(display)
        display.fill(BLACK)

        while True:
            # Chase right
            for _ in range(self.multiple*length):
                display.strip.right(fill=ColorHSV(np.random.uniform(), 1.0, 1.0).toRGB())
                yield self.pause
                for _ in range(self.gap):
                    display.strip.right()
                    yield self.pause

            # Chase left
            for _ in range(self.multiple*length):
                display.strip.left(fill=ColorHSV(np.random.uniform(), 1.0, 1.0).toRGB())
                yield self.pause
                for _ in range(self.gap):
                    display.strip.left()
                    yield self.pause

