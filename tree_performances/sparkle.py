import gc
from light_show.talent import register
from light_show.color import randomColor, HSV
from light_show.utils import randFloat

__all__ = ("Sparkle", "SingleSparkle",)


class Sparkle:
    """Random lights come on and fade away"""
    def __init__(self, fade=50):
        self.fade = fade

    def __call__(self, tree):
        active = set()
        dimStep = 1.0/self.fade
        while True:
            # Fade everything that's on
            deleteMe = []
            for ss, ii in active:
                color = tree[ss][ii].toHsv()
                color.dimmer(dimStep)
                if color.value == 0:
                    deleteMe.append((ss, ii))
                else:
                    tree[ss][ii] = color
            for key in deleteMe:
                active.remove(key)
            gc.collect()

            # Add a new one
            string = int(randFloat()*len(tree))
            index = int(randFloat()*len(tree[string]))
            key = (string, index)
            if key in active:
                continue
            active.add(key)
            tree[string][index] = randomColor()
            yield


class SingleSparkle:
    """Tree is solid color, and individual lights sparkle"""
    def __init__(self, fade=50):
        self.fade = fade

    def __call__(self, tree):
        active = set()
        satStep = 1.0/self.fade
        color = randomColor()
        hue = color.hue
        tree.fill(color)
        while True:
            # Increase saturation of everything that's on
            deleteMe = []
            for ss, ii in active:
                color = tree[ss][ii].toHsv()
                sat = color.sat + satStep
                if sat > 1.0:
                    deleteMe.append((ss, ii))
                else:
                    tree[ss][ii] = HSV(hue, sat, 1.0)
            for key in deleteMe:
                active.remove(key)
            gc.collect()

            # Add a new one
            string = int(randFloat()*len(tree))
            index = int(randFloat()*len(tree[string]))
            key = (string, index)
            if key in active:
                continue
            active.add(key)
            tree[string][index] = HSV(hue, 0.0, 1.0)
            yield


register(Sparkle, fade=50)
register(SingleSparkle, fade=50)