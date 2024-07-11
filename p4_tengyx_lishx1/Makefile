PY = python3 # you may need to modify this line

all:
	$(PY) -m platformio run

download: all
	sudo chmod -R 777 /dev/bus/usb/
	./dfu-util -l | grep 28e9:0189
	./dfu-util -a 0 --dfuse-address 0x08000000:leave -D .pio/build/sipeed-longan-nano/firmware.bin

clean:
	rm -rf .pio

.PHONY: clean