build:
	cmake --build build --target blink

scrollwheel:
	cmake -B build -DMODE=SCROLLWHEEL
	cmake --build build --target blink

volume:
	cmake -B build -DMODE=VOLUME
	cmake --build build --target blink

flash:
	picotool load build/blink.elf && picotool reboot

prep:
	git submodule update --recursive --init
	cmake -DPICO_PLATFORM=rp2350 -S . -B build

.PHONY: build
