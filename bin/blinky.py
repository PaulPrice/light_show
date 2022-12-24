#!/usr/bin/env python3

import time
import random
import argparse

from light_show import LedController, StripType, ColorHSV
import light_show.colorSet


def blinky(
    num: int,
    gpio: int,
    pause: float = 1,
    lifetime: float = 0,
    stripType: StripType = StripType.RGB,
    colors: light_show.colorSet.ColorGenerator = light_show.colorSet.RandomHSV(),
):
    controller = LedController(gpio, num, stripType)
    strip = controller[0]

    runtime = 0
    while True:
        strip.fill(next(colors))
        controller.render()
        time.sleep(pause)
        runtime += pause
        if lifetime > 0 and runtime > lifetime:
            break
    strip.clear()
    controller.render()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("num", type=int, help="Number of LEDs in strip")
    parser.add_argument("--gpio", type=int, default=12, help="GPIO pin (not physical pin)")
    parser.add_argument("--pause", type=float, default=1.0, help="Pause between blinks (sec)")
    parser.add_argument("--type", default="RGB", help="Type of LED strip")
    parser.add_argument("--lifetime", type=float, default=0, help="Lifetime of blinking")
    parser.add_argument(
        "--colors", choices=light_show.colorSet.__all__, default="RandomHSV", help="Color generator"
    )
    args = parser.parse_args()
    colors = getattr(light_show.colorSet, args.colors)()
    blinky(args.num, args.gpio, args.pause, args.lifetime, getattr(StripType, args.type), colors=colors)


if __name__ == "__main__":
    main()
