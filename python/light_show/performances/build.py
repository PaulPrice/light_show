from typing import Iterator
import numpy as np

from light_show import Performance, Config, LedDisplay
from light_show import ColorHSV, BLACK

__all__ = ("SolidBuild", "BuildLeftRight")


class SolidBuild(Performance):
    def __init__(self, pause):
        self.pause = pause

    @classmethod
    def defaults(cls) -> Config:
        return dict(pause=0.01)

    def __call__(self, display: LedDisplay) -> Iterator[float]:
        length = max(len(gg) for gg in display.groups)

        while True:
            display.fill(BLACK)
            color = ColorHSV(np.random.uniform(), 1.0, 1.0)
            # Build up
            for level in range(length):
                strips = [ss[level:] if rr else ss[:length - level]
                          for ss, rr in zip(display.groups, display.reversed)]
                for ss, rr in zip(strips, display.reversed):
                    ss[-1 if rr else 0] = color
                yield self.pause
                for _ in range(length - level - 1):
                    for ss, rr in zip(strips, display.reversed):
                        if rr:
                            ss.left()
                        else:
                            ss.right()
                    yield self.pause
            # Fall off
            for level in range(length):
                strips = [ss[:level + 1] if rr else ss[length - level - 1:]
                          for ss, rr in zip(display.groups, display.reversed)]
                for _ in range(level + 1):
                    for ss, rr in zip(strips, display.reversed):
                        if rr:
                            ss.left()
                        else:
                            ss.right()
                    yield self.pause


class BuildLeftRight(Performance):
    def __init__(self, pause):
        self.pause = pause

    @classmethod
    def defaults(cls) -> Config:
        return dict(pause=0.01)

    def __call__(self, display: LedDisplay) -> Iterator[float]:
        length = len(display)
        while True:
            display.fill(BLACK)
            color = ColorHSV(np.random.uniform(), 1.0, 1.0)
            # Build left
            for level in range(length):
                strip = display.strip[level:]
                strip[-1] = color
                yield self.pause
                for _ in range(length - level - 1):
                    strip.left()
                    yield self.pause
            # Fall left
            for level in range(length):
                strip = display.strip[:level + 1]
                for _ in range(level + 1):
                    strip.left()
                    yield self.pause
            # Build right
            for level in range(length):
                strip = display.strip[:length - level]
                strip[0] = color
                yield self.pause
                for _ in range(length - level - 1):
                    strip.right()
                    yield self.pause
            # Fall right
            for level in range(length):
                strip = display.strip[length - level - 1:]
                for _ in range(level + 1):
                    strip.right()
                    yield self.pause
