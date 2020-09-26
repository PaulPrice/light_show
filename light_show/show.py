try:
    import utime as time
    haveEsp = True
except ImportError:
    import time
    haveEsp = False

import gc
from .talent import getPerformanceNames, generatePerformances


class Show:
    def __init__(self, leds, *disabled):
        self.leds = leds
        self.enabled = set(getPerformanceNames()) - set(disabled)

    def start(self, limit, defaultPause=0.1):
        perform = self.perform if haveEsp else self.demo
        for perf in generatePerformances():
            print(f"Performing {perf}")
            perform(perf, limit, defaultPause=defaultPause)
            del perf
            gc.collect()

    def perform(self, performance, limit, defaultPause=0.1):
        timer = 0.0
        self.leds.clear()
        action = performance(self.leds)
        while timer < limit:
            try:
                pause = next(action)
            except StopIteration:
                break
            if pause is None:
                pause = defaultPause
            self.leds.write()
            time.sleep(pause)
            timer += pause

    def demo(self, performance, limit, defaultPause=0.1, interval=0.1):
        import matplotlib.pyplot as plt
        from matplotlib.animation import ArtistAnimation
        figure = plt.figure()
        artists = []
        timer = 0.0
        self.leds.clear()
        action = performance(self.leds)
        while timer < limit:
            try:
                pause = next(action)
            except StopIteration:
                break
            if pause is None:
                pause = defaultPause
            art = self.leds.plot(show=False)
            num = int((pause)/interval)
            if num == 0:
                continue
            artists += [[art]]*num
            timer += pause

        ani = ArtistAnimation(figure, artists, interval=interval*1000, repeat=False, blit=True)  # noqa
        plt.show()
