from light_show.talent import register
from light_show.color import randomColor

__all__ = ("Spiral",)


class Spiral:
    """Lights spiral around"""
    def __init__(self, gap=5):
        self.gap = gap

    def __call__(self, tree):
        index = 0
        while True:
            tree.down()
            tree[index]["TOP"] = randomColor()
            index += 1
            if index >= len(tree):
                index = 0
            yield


register(Spiral, gap=5)
