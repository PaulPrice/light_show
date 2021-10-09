from light_show.talent import register
from light_show.color import randomColor

__all__ = ("Sprinkle",)


class Sprinkle:
    """Colors sprinkle down on all strings"""
    def __call__(self, tree):
        while True:
            tree.down()
            for string in tree:
                string["TOP"] = randomColor()
            yield


register(Sprinkle)
