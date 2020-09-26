from light_show.talent import register
from light_show.color import randomColor, BLACK, HSV

__all__ = ("Strings", "UpAndDown", "Swipe",)


class Strings:
    """Light up strings one after another"""
    def __init__(self, num=7):
        self.num = num

    def __call__(self, tree):
        while True:
            color = randomColor()
            for _ in range(self.num):
                for ii in range(len(tree)):
                    tree[ii][:] = color
                    yield
                    tree[ii][:] = BLACK
            for _ in range(self.num):
                for ii in reversed(range(len(tree))):
                    tree[ii][:] = color
                    yield
                    tree[ii][:] = BLACK


class UpAndDown:
    """Lights go up one string and down the next"""
    def __init__(self, gap=3):
        self.gap = gap

    def __call__(self, tree):
        goingUp = [ii % 2 == 0 for ii in range(len(tree))]
        while True:
            tree[0][0 if goingUp[0] else -1] = randomColor()
            for _ in range(self.gap):
                yield
                last = BLACK
                for ii, string in enumerate(tree):
                    if goingUp[ii]:
                        nextLast = string[-1]
                        string.right()
                        string[0] = last
                        last = nextLast
                    else:
                        nextLast = string[0]
                        string.left()
                        string[-1] = last
                        last = nextLast


class Swipe:
    """Strings swipe on and off"""
    def __call__(self, tree):
        numStrings = len(tree)
        colors = [HSV(ii/numStrings, 1.0, 1.0).toRgb() for ii in range(numStrings)]
        while True:
            for ii, col in enumerate(colors):
                tree[ii][:] = col
                yield
            for ii in range(numStrings):
                tree[ii][:] = BLACK
                yield
            for ii, col in enumerate(reversed(colors)):
                tree[numStrings - ii - 1][:] = col
                yield
            for ii in reversed(range(numStrings)):
                tree[ii][:] = BLACK
                yield


register(Strings, num=7)
register(UpAndDown, gap=3)
register(Swipe)
