from light_show.talent import register
from light_show.color import randomColor, BLACK, HSV

__all__ = ("Ring", "DoubleRing", "SolidRingBuild", "RainbowRingBuild")


class Ring:
    """Ring of random color goes down then up"""
    def __call__(self, tree):
        while True:
            tree.set("TOP", randomColor().toRgb())
            yield
            for _ in range(tree.length - 1):
                tree.down()
                yield
            for _ in range(tree.length - 1):
                tree.up()
                yield


class DoubleRing:
    """Two rings of random color go up and down"""
    def __init__(self, num=7):
        self.num = num

    def __call__(self, tree):
        while True:
            color = randomColor()
            for _ in range(self.num):
                for ii in range(tree.length - 1):
                    tree.set(ii, color)
                    tree.set(tree.length - 1 - ii, color)
                    yield
                    tree.set(ii, BLACK)
                    tree.set(tree.length - 1 - ii, BLACK)


class SolidRingBuild:
    """Rings fall down, building up from the bottom, and then fall off"""
    def __call__(self, tree):
        while True:
            tree.fill(BLACK)
            color = randomColor().toRgb()
            for ring in range(tree.length):
                for ii in range(tree.length - ring - 1):
                    tree.set(tree.length - 1 - ii, color)
                    yield
                    tree.set(tree.length - 1 - ii, BLACK)
                tree.set(ring, color)
                yield
            for ring in range(tree.length):
                for ii in range(1, ring + 1):
                    tree.set(ring - ii, color)
                    yield
                    tree.set(ring - ii, BLACK)


class RainbowRingBuild:
    """Rainbow rings fall down, building up from the bottom, then fall off"""
    def __call__(self, tree):
        while True:
            tree.fill(BLACK)
            for ring in range(tree.length):
                color = HSV(ring/tree.length, 1.0, 1.0).toRgb()
                for ii in range(tree.length - ring - 1):
                    tree.set(tree.length - 1 - ii, color)
                    yield
                    tree.set(tree.length - 1 - ii, BLACK)
                tree.set(ring, color)
                yield
            for ring in range(tree.length):
                color = HSV(ring/tree.length, 1.0, 1.0).toRgb()
                for ii in range(1, ring + 1):
                    tree.set(ring - ii, color)
                    yield
                    tree.set(ring - ii, BLACK)


register(Ring)
register(DoubleRing, num=7)
register(SolidRingBuild)
register(RainbowRingBuild)