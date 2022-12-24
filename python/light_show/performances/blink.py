from typing import Iterator
import numpy as np

from light_show import Performance, Config, LedDisplay
from ..colorSet import ColorGenerator, RandomHSV

__all__ = ("Blink",)


class Blink(Performance):
    def __init__(self, pause=1.0, colors: ColorGenerator = RandomHSV()):
        self.pause = pause
        self._colors = colors

    @classmethod
    def defaults(cls) -> Config:
        return dict(pause=1.0)

    def __call__(self, display: LedDisplay) -> Iterator[float]:
        strip = display.strip
        display.clear()
        while True:
            strip.fill(next(self._colors))
            yield self.pause
            strip.clear()
            yield self.pause
