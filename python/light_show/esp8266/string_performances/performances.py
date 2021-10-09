try:
    import ulab as np
    from uos import urandom
    rng = lambda: int.from_bytes(urandom(1), "little")
except ImportError:
    import numpy as np
    import random
    rng = lambda: random.randrange(2**8)


from color import randomColor, WHITE, BLACK
from hsv import hsv2rgb


class Fade:
    def __init__(self, interval, pause):
        self.pause = pause
        self.change = pause/interval

    def __call__(self, leds):
        while leds.isOn():
            value = leds.brightness
            leds.dimmer(value - self.change)
            yield self.pause


def fadeOut(leds, interval=5.0, pause=0.1):
    fade = Fade(interval, pause)
    while True:
        color = randomColor()
        leds.fill(color)
        yield
        yield from fade(leds)


class BackAndForth:
    def __init__(self, pause):
        self.pause = pause

    def __call__(self, leds):
        num = len(leds) - 1
        color = randomColor()
        leds[0] = color
        yield self.pause
        while True:
            for _ in range(num):
                leds.right()
                yield self.pause
            for _ in range(num):
                leds.left()
                yield self.pause
            leds[0] = randomColor()


class KnightRider:
    def __init__(self, color, pause, length):
        hsv = color.toHsv()
        self.hue = hsv.hue
        self.sat = hsv.sat
        self.pause = pause
        self.length = length

    def __call__(self, leds):
        num = len(leds)

        left = np.zeros(num, dtype=np.uint8)
        right = np.zeros(num, dtype=np.uint8)
        right[:self.length] = np.linspace(0, 255, self.length)

        while True:
            # Shift left/right
            offLeft = left[0]
            offRight = right[num - 1]
            left[:-1] = left[1:]
            right[1:] = right[:-1]
            right[0] = offLeft
            left[num - 1] = offRight

            # Display
            value = np.clip*((left + right)/255, 0.0, 1.0)
            leds[:] = hsv2rgb(self.hue, self.sat, value)
            yield self.pause


class StreamOfColor:
    def __init__(self, pause, multiple=5):
        self.pause = pause
        self.multiple = multiple

    def __call__(self, leds):
        num = len(leds)
        while True:
            for _ in range(self.multiple*num):
                leds.right(fill=randomColor())
                yield self.pause
            for _ in range(self.multiple*num):
                leds.left(fill=randomColor())
                yield self.pause


class BuildFill:
    def __init__(self, pause, fadeInterval):
        self.pause = pause
        self.fader = Fade(fadeInterval, pause)

    def __call__(self, leds):
        while True:
            yield from self.right(leds)
            yield from self.left(leds)

    def right(self, leds):
        num = len(leds)
        color = randomColor()
        end = num
        for end in reversed(range(num)):
            leds[0] = color
            yield self.pause
            for _ in range(end):
                leds.right()
                if end + 1 < num:
                    leds[end + 1] = color
                yield self.pause
        yield from self.fader(leds)

    def left(self, leds):
        num = len(leds)
        color = randomColor()
        end = 0
        for end in range(num):
            leds[num - 1] = color
            yield self.pause
            for _ in range(num - end - 1):
                leds.left()
                if end > 0:
                    leds[end - 1] = color
                yield self.pause
        yield from self.fader(leds)


class Lightning:
    def __init__(self, pause=0.02, dimming=0.3):
        self.pause = pause
        self.dimming = dimming

    def __call__(self, leds):
        leds[:] = WHITE
        yield self.pause
        for _ in range(5):
            leds.dimmer(leds.brightness - self.dimming)
            yield self.pause
        leds[:] = WHITE
        for _ in range(10):
            leds.dimmer(leds.brightness - self.dimming)
            yield self.pause
        leds[:] = WHITE
        for _ in range(10):
            leds.dimmer(leds.brightness - self.dimming)
            yield self.pause


class SpookyEyes:
    def __init__(self, distance=4, blink=5, pause=0.1, fade=3):
        self.distance = distance
        self.blink = blink
        self.pause = pause
        self.fader = Fade(fade, self.pause)

    def __call__(self, leds):
        color = randomColor()
        leftEye = int(rng()/255*(len(leds) - self.distance))
        rightEye = leftEye + self.distance
        leds[leftEye] = color
        leds[rightEye] = color
        yield self.pause*self.blink
        leds[leftEye] = BLACK
        leds[rightEye] = BLACK
        yield self.pause*self.blink
        leds[leftEye] = color
        leds[rightEye] = color
        yield self.pause*self.blink
        yield from self.fader(leds)


class Robbie2:
    def __init__(self, pause=0.1, fadeInterval=3.0):
        self.pause = pause
        self.fader = Fade(fadeInterval, pause)

    def __call__(self, leds):
        num = len(leds)
        while True:
            color = randomColor()
            for ii in range(0, 2*(num//2), 2):
                for cc in (color, BLACK, color):
                    leds[ii] = cc
                    leds[ii + 2] = cc
                    yield self.pause

            color = randomColor()
            for ii in range(1, 2*(num//2 - 1), 2):
                for cc in (color, BLACK, color):
                    leds[ii] = cc
                    leds[ii + 2] = cc
                    yield self.pause

            yield from self.fader(leds)


#from led_string import LedString
#from light_show import LightShow
#leds = LedString(11)
#show = LightShow(leds)
#show.demo(BuildFill(0.1, 3.0), 60)
#show.demo(Robbie2(1.0, 3.0), 60)
