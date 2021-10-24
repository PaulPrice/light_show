#!/usr/bin/env python3

import time
import random
import argparse

from light_show import LedController, StripType, ColorHSV

def blinky(num, gpio, pause=1, type=StripType.GRB):
    controller = LedController(gpio, num, StripType.GRB)
    strip = controller[0]
    while True:
        strip.fill(ColorHSV(random.random(), 1.0, 1.0).toRGB())
        controller.render()
        time.sleep(pause)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("num", type=int, help="Number of LEDs in strip")
    parser.add_argument("--gpio", type=int, default=12, help="GPIO pin (not physical pin)")
    parser.add_argument("--pause", type=float, default=1.0, help="Pause between blinks (sec)")
    parser.add_argument("--type", default="GRB", help="Type of LED strip")
    args = parser.parse_args()
    blinky(args.num, args.gpio, args.pause, getattr(StripType, args.type))


if __name__ == "__main__":
    main()
