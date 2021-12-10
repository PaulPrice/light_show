from typing import Iterator
import numpy as np

from light_show import Performance, Config, LedDisplay

__all__ = ("RainbowCycle",)


class RainbowCycle(Performance):
    def __init__(self, pause, period):
        self.pause = pause
        self.period = period

    @classmethod
    def defaults(cls) -> Config:
        return dict(pause=0.1, period=10.0)

    def __call__(self, display: LedDisplay) -> Iterator[float]:
        num = len(display)
        hue = np.linspace(0.0, 1.0, num, dtype=np.float32)
        saturation = value = np.ones(num, dtype=np.float32)
        dHue = self.pause/self.period
        while True:
            timer = 0.0
            while timer < self.period:
                display.strip.setHSV(hue, saturation, value)
                hue += dHue
                yield self.pause
                timer += self.pause
            dHue *= -1
