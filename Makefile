.PHONY: gpiosampler
gpiosampler:
	g++ -o bin/gpiosampler -Wall -std=c++0x src/main.cpp -lSDL -lSDL_mixer -lwiringPi

.PHONY: install
install:
	sudo cp bin/gpiosampler /usr/bin
	sudo cp bin/gpiosampler.sh /usr/bin
	sudo cp init.d/gpiosampler.sh /etc/init.d
	sudo update-rc.d gpiosampler.sh defaults
