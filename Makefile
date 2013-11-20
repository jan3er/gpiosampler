.PHONY: gpiosampler
gpiosampler:
	g++ -o gpiosampler -Wall -std=c++0x src/main.cpp -lSDL -lSDL_mixer -lwiringPi
