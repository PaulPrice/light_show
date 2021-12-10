from typing import Iterator
import numpy as np

from light_show import Performance, Config, LedDisplay
from light_show import ColorHSV

__all__ = ("Blink",)


class Blink(Performance):
    def __init__(self, pause=1.0):
        self.pause = pause

    @classmethod
    def defaults(cls) -> Config:
        return dict(pause=1.0)

    def __call__(self, display: LedDisplay) -> Iterator[float]:
        strip = display.strip
        display.clear()
        while True:
            strip.fill(ColorHSV(np.random.uniform(), 1.0, 1.0))
            yield self.pause
            strip.clear()
            yield self.pause
