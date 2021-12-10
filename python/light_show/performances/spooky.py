from typing import Iterator
import numpy as np

from light_show import Performance, Config, LedDisplay
from light_show import ColorHSV, BLACK
from light_show import ColorRGB

__all__ = ("Spooky",)


class Eyes:
    def __init__(self, left, right, color, blink, lifetime):
        self.left = left
        self.right = right
        self.color = color
        self.blink = blink
        self.lifetime = lifetime

        self.state = "closed"
        self.counter = 0

    def open(self, display):
        display.strip[self.left] = self.color
        display.strip[self.right] = self.color
        self.state = "open"

    def close(self, display):
        display.strip[self.left] = BLACK
        display.strip[self.right] = BLACK
        self.state = "closed"

    def next(self, display):
        self.counter += 1
        if self.counter % self.blink == 0:
            if self.state == "open":
                self.close(display)
            else:
                self.open(display)
        return self.counter < self.lifetime


class Spooky(Performance):
    def __init__(self, pause, blink, eyes, distance, lifetime, lightning):
        self.pause = pause
        self.blink = blink
        self.eyes = eyes
        self.distance = distance
        self.lifetime = lifetime
        self.lightning = lightning

    @classmethod
    def defaults(cls) -> Config:
        return dict(pause=0.1, blink=20, eyes=0.1, distance=2, lifetime=80, lightning=5.0e-3)

    def __call__(self, display: LedDisplay) -> Iterator[float]:
        length = len(display)
        display.fill(BLACK)

        eyes = []
        eyeMask = np.zeros(length, dtype=bool)
        while True:
            if np.random.uniform() < self.lightning:
                eyes = []  # Scared everyone away
                for vv in reversed(np.linspace(0, 255, 5, False, dtype=int)):
                    display.fill(ColorRGB(vv, vv, vv))
                    yield self.pause
                for vv in reversed(np.linspace(0, 255, 10, False, dtype=int)):
                    display.fill(ColorRGB(vv, vv, vv))
                    yield self.pause
                for vv in reversed(np.linspace(0, 255, 10, False, dtype=int)):
                    display.fill(ColorRGB(vv, vv, vv))
                    yield self.pause
                continue

            if np.random.uniform() < self.eyes:
                left = int(np.random.uniform()*(length - self.distance))
                right = left + self.distance
                if not np.any(eyeMask[left:right + 1]):
                    new = Eyes(left, right, ColorHSV(np.random.uniform(), 1.0, 1.0),
                               self.blink, self.lifetime)
                    eyes.append(new)
                    new.open(display)
                    eyeMask[left:right + 1] = True

            keep = []
            for ee in eyes:
                if ee.next(display):
                    keep.append(ee)
                else:
                    ee.close(display)
                    eyeMask[ee.left:ee.right + 1] = False
            eyes = keep

            yield self.pause
