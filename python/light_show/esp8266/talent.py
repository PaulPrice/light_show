from .utils import shuffle

__all__ = ("register", "getPerformanceNames", "getPerformance", "setConfig", "getConfig",
           "generatePerformances", "disable", "enable", "getHtml")


_performances = {}
_disabled = set()


def register(PerformanceClass, **config):
    name = PerformanceClass.__name__
    print(f"Registering performance {name}")
    assert "enabled" not in config  # Reserved for switching it on and off
    _performances[name] = (PerformanceClass, config)


def getPerformanceNames():
    return set(_performances.keys()) - _disabled


def getPerformance(name):
    Perf, config = _performances[name]
    return Perf(**config)


def setConfig(name, **update):
    _, config = _performances[name]
    config.update(update)


def getConfig(name):
    _, config = _performances[name]
    return config


def generatePerformances():
    while True:
        for name in shuffle(getPerformanceNames()):
            yield getPerformance(name)


def disable(name):
    _disabled.add(name)


def enable(name):
    _disabled.discard(name)


def getHtml():
    pass
