from light_show.led_string import LedString
from light_show.led_tree import LedTree
from light_show.show import Show
import tree_performances

tree = LedTree([LedString(11) for _ in range(6)], False)
show = Show(tree)
show.start(30)
