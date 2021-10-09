try:
    import ulab as np
except ImportError:
    import numpy as np

from light_show.talent import register
from light_show.color import randomColor
from light_show.utils import randFloat

__all__ = ("Trails",)


class Trails:
    """Trails fall from the top"""
    def __init__(self, length=3):
        self.length = length

    def __call__(self, tree):
        numStrings = len(tree)
        length = tree.length
        trailEnd = 2*self.length
        probThreshold = 1.0/numStrings
        location = np.ones(numStrings, dtype=int)*length  # Location of trail
        colors = [None]*numStrings  # Color of trail
        while True:
            tree.down()
            location += 1
            for ii, string in enumerate(tree):
                if location[ii] <= self.length:
                    # Propagate a trail
                    string["TOP"] = colors[ii]
                elif location[ii] > trailEnd and randFloat() < probThreshold:
                    # Start a trail
                    location[ii] = 0
                    colors[ii] = randomColor()
            yield


register(Trails, length=3)
