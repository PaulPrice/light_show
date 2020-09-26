try:
    import ulab as np
except ImportError:
    import numpy as np

from light_show.talent import register
from light_show.color import HSV

__all__ = ("BouncingRainbow",)


class BouncingRainbow:
    """Rainbow goes up and down"""
    def __init__(self, length=5):
        self.length = length

    def __call__(self, tree):
        numStrings = len(tree)
        length = tree.length
        goingUp = [True]*numStrings
        hue = [ii/self.length for ii in range(self.length)]
        colors = [HSV(hh, 1.0, 1.0).toRgb() for hh in hue]
        red = np.array([cc.red for cc in colors])
        green = np.array([cc.green for cc in colors])
        blue = np.array([cc.blue for cc in colors])

        stringEnd = length - self.length
        location = [int((0.5*np.cos(ii/numStrings*np.pi) + 0.5)*stringEnd) for ii in range(numStrings)]

        tree.clear()
        while True:
            for ii, string in enumerate(tree):
                loc = location[ii]
                string[loc:loc + self.length] = (red, green, blue)
                if goingUp[ii] and loc == stringEnd:
                    goingUp[ii] = False
                elif not goingUp[ii] and loc == 0:
                    goingUp[ii] = True
                if goingUp[ii]:
                    string.right()
                    location[ii] += 1
                else:
                    string.left()
                    location[ii] -= 1
            yield


register(BouncingRainbow, length=5)
