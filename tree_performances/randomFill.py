try:
    import ulab as np
except ImportError:
    import numpy as np

from light_show.talent import register
from light_show.color import randomColor, BLACK
from light_show.utils import randFloat

__all__ = ("RandomFill",)


class RandomFill:
    """Pixel by pixel, fill the tree with color"""
    def __call__(self, tree):
        num = sum(len(ss) for ss in tree)
        indices = np.zeros((num, 2), dtype=int)
        start = 0
        for ii in range(len(tree)):
            length = len(tree[ii])
            ss = slice(start, start + length)
            indices[ss, 0] = ii
            indices[ss, 1] = np.arange(length)
            start += length
        while True:
            tree.clear()
            order = sorted([(randFloat(), ii) for ii in range(num)])
            for _, ii in order:
                ss, tt = indices[ii]
                tree[ss][tt] = randomColor()
                yield
            for _, ii in order:
                ss, tt = indices[ii]
                tree[ss][tt] = BLACK
                yield


register(RandomFill)
