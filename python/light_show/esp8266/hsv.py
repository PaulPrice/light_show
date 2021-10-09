# Taken from cpython

# HSV: Hue, Saturation, Value
# H: position in the spectrum
# S: color saturation ("purity")
# V: color brightness

__all__ = ("rgb_to_hsv", "hsv_to_rgb")


def rgb_to_hsv(r, g, b):
    maxc = max(r, g, b)
    minc = min(r, g, b)
    v = maxc
    if minc == maxc:
        return 0.0, 0.0, v
    s = (maxc-minc) / maxc
    rc = (maxc-r) / (maxc-minc)
    gc = (maxc-g) / (maxc-minc)
    bc = (maxc-b) / (maxc-minc)
    if r == maxc:
        h = bc-gc
    elif g == maxc:
        h = 2.0+rc-bc
    else:
        h = 4.0+gc-rc
    h = (h/6.0) % 1.0
    return h, s, v


def hsv_to_rgb(h, s, v):
    if s == 0.0:
        return v, v, v
    i = int(h*6.0)  # XXX assume int() truncates!
    f = (h*6.0) - i
    p = v*(1.0 - s)
    q = v*(1.0 - s*f)
    t = v*(1.0 - s*(1.0-f))
    i = i % 6
    if i == 0:
        return v, t, p
    if i == 1:
        return q, v, p
    if i == 2:
        return p, v, t
    if i == 3:
        return p, q, v
    if i == 4:
        return t, p, v
    if i == 5:
        return v, p, q
    # Cannot get here


# Vectorised versions with numpy

try:
    import ulab as np
except ImportError:
    import numpy as np


def hsv2rgb(hue, sat, value):
    # https://en.wikipedia.org/wiki/HSL_and_HSV#HSV_to_RGB_alternative
    # hue, sat and value all 0..1
    # Returns red, green, blue in 0..1
    def kk(nn):
        return (nn + 6*hue) % 6

    def ff(nn):
        return value - value*sat*np.clip(np.min((kk(nn), 4 - kk(nn)), axis=0), 0, 1)

    return ff(5), ff(3), ff(1)


def rgb2hsv(red, green, blue):
    # https://en.wikipedia.org/wiki/HSL_and_HSV#From_RGB
    # red, green, blue in 0..1hue, sat and value all 0..1
    # Returns hue, sat and value in 0..1
    try:
        length = len(red)
    except Exception:
        length = 1
    # Having to create and insert is due to an unfortunate limitation in ulab
    rgb = np.zeros((3, length), dtype=np.float)
    try:
        rgb[0] = red
        rgb[1] = green
        rgb[2] = blue
    except Exception:
        rgb[0] = red.flatten()
        rgb[1] = green.flatten()
        rgb[2] = blue.flatten()
    # These indexing games are because we can't index with integer arrays in ulab
    index = np.arange(length, dtype=np.uint16)
    index2d = np.zeros((length, 3), dtype=np.uint16)
    index2d[:, 0] = 0
    index2d[:, 1] = 1
    index2d[:, 2] = 2
    brightest = np.numerical.argmax(rgb, axis=0)
    isBrightest = index2d == brightest

    value = rgb[isBrightest]
    cc = value - np.min(rgb, axis=0)
    hueSelect = np.array([0, 2, 4])
    # Some unnecessary calculations, but still faster than a python loop
    hueCalc = np.array([green - blue, blue - red, red - green])
    hue = (hueSelect[index == brightest] + hueCalc[isBrightest]/cc)/6
    sat = np.where(value == 0, 0.0, cc/value)
    return hue, sat, value
