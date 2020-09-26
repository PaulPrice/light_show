PORT=/dev/cu.SLAB_USBtoUART
FILES=light_show string_performances tree_performances

install:	$(FILES)
	for ff in $(FILES); do ampy --port $(PORT) put $$ff; done


.PHONY: install
