from typing import Type, Dict, Any, Iterator, TypeVar
from abc import ABC, abstractmethod
from random import shuffle

from .LedDisplay import LedDisplay


__all__ = ("Performance", "Config",
           "register", "getPerformanceNames", "getPerformance", "setConfig", "getConfig",
           "generatePerformances", "disable", "enable")

Config = Dict[str, Any]

_performances: Config = {}
_disabled = set()


Perf = TypeVar("Perf", bound="Performance")  # Forward declaration


def register(cls: Type[Perf], **config: Config):
    name = cls.__name__
    print(f"Registering performance {name}")
    _performances[name] = (cls, config)


def getPerformanceNames() -> set[str]:
    return set(_performances.keys()) - _disabled


def getPerformance(name) -> Perf:
    cls, config = _performances[name]
    return cls(**config)


def setConfig(name: str, **update: Config):
    getConfig(name).update(update)


def getConfig(name: str) -> Config:
    return _performances[name][1]


def generatePerformances() -> Iterator[Perf]:
    while True:
        names = list(getPerformanceNames())
        if not names:
            raise RuntimeError("No performances enabled")
        shuffle(names)
        for nn in names:
            yield getPerformance(nn)


def disable(name: str):
    if name not in _performances:
        raise RuntimeError(f"Unrecognised performance: {name}")
    _disabled.add(name)


def enable(name: str):
    if name not in _performances:
        raise RuntimeError(f"Unrecognised performance: {name}")
    _disabled.discard(name)


class Performance(ABC):
    @abstractmethod
    def __init__(self, **config: Config):
        pass

    @abstractmethod
    def __call__(self, display: LedDisplay) -> Iterator[float]:
        pass

    def __init_subclass__(cls: Type["Performance"]):
        register(cls, **cls.defaults())

    @classmethod
    @abstractmethod
    def defaults(cls) -> Config:
        pass
