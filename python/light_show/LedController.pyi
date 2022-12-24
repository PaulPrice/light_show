from typing import overload

from .LedStrip import LedStrip
from .LedStripSet import LedStripSet

class StripType(int):
    RGBW: int
    RBGW: int
    GRBW: int
    GBRW: int
    BRGW: int
    BGRW: int
    RGB: int
    RBG: int
    GRB: int
    GBR: int
    BRG: int
    BGR: int

class LedController:
    @overload
    def __init__(self, gpio: int, num: int, type: StripType, dma: int = 10) -> None: ...
    @overload
    def __init__(self, gpio: np.ndarray, num: np.ndarray, type: np.ndarray, dma: int = 10) -> None: ...
    def render(self, wait: bool = True) -> None: ...
    def size(self) -> int: ...
    def __len__(self) -> int: ...
    def __getitem__(self, index: int) -> LedStrip: ...
    def getAll(self) -> LedStripSet: ...
    def off(self) -> None: ...
