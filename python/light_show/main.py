import numpy as np

from light_show.LedController import LedController, StripType
from light_show.LedDisplay import LedDisplay
from light_show.show import Show
import light_show.performances  # noqa: imported for side-effects

from light_show.talent import enable, disable, getPerformanceNames


def main(gpio: int, num: int, ledType: str, limit: float, demo: bool = False):
    controller = LedController(gpio, num, getattr(StripType, ledType))
    groups = [slice(0, num//2),
              slice(num//2, num),
              ]
    reversed = [True, False]
    xx = np.linspace(0.0, 10.0, num, dtype=float)
    yy = np.concatenate((np.linspace(0.0, 2.5, num//2, dtype=float),
                         np.linspace(2.5, 0.0, num//2, dtype=float)))
    zz = np.zeros(num, dtype=float)
    xyz = np.vstack([xx, yy, zz]).T
    strip = [slice(None, None, None)]
    display = LedDisplay(controller, groups, reversed, xyz, strip)

    print(getPerformanceNames())
    for pp in getPerformanceNames():
        disable(pp)
    enable("Spooky")

    show = Show(display, demo=demo)
    show.start(limit)


if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument("--gpio", type=int, default=12, help="GPIO number")
    parser.add_argument("--num", type=int, default=40, help="Number of pixels")
    parser.add_argument("--ledType", type=str, default="RGB", help="Type of pixels")
    parser.add_argument("--demo", action="store_true", default=False, help="Demo, with plots?")
    parser.add_argument("limit", type=float, default=30, help="Time limit (sec)")
    args = parser.parse_args()
    main(**vars(args))
