import time

from .LedDisplay import LedDisplay
from .talent import Performance, generatePerformances


class Show:
    def __init__(self, display: LedDisplay, demo: bool = False):
        self._display = display
        self._demo = demo
        if demo:
            import matplotlib.pyplot as plt
            plt.ion()
            self._figure, self._axes = plt.subplots(facecolor="black")
            self._figure.show()
            plt.axis("off")

    def start(self, limit: float, defaultPause: float = 0.05):
        for perf in generatePerformances():
            print(f"Performing {perf}")
            self.perform(perf, limit, defaultPause=defaultPause)
            del perf

    def perform(self, performance: Performance, limit: float, defaultPause: float = 0.05):
        timer = 0.0
        self._display.clear()
        action = performance(self._display)
        if self._demo:
            self.plot()
        else:
            self._display.render()
        while timer < limit:
            try:
                pause = next(action)
            except StopIteration:
                break
            if pause is None:
                pause = defaultPause
            self.render(pause)
            timer += pause

    def plot(self):
        self._axes.cla()
        self._display.plot(self._axes)
        self._figure.canvas.draw()

    def render(self, pause):
        if self._demo:
            import matplotlib.pyplot as plt
            self.plot()
            plt.pause(pause)
        else:
            self._display.render()
            time.sleep(pause)

    def __del__(self):
        if self._demo:
            import matplotlib.pyplot as plt
            plt.show()
