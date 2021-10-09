__all__ = ("randInt", "randFloat")

try:
    import ulab as np
    from uos import urandom
    randInt = lambda: int.from_bytes(urandom(1), "little")
    randFloat = lambda: int.from_bytes(urandom(4), "little")/2**32
except ImportError:
    import numpy as np
    import random
    randInt = lambda: random.randrange(2**8)
    randFloat = lambda: random.uniform(0.0, 1.0)


def shuffle(iterable):
    return [xx[1] for xx in sorted([(randFloat(), item) for item in iterable])]
