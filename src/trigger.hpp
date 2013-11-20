#ifndef TRIGGER_H
#define TRIGGER_H

// sound mixer
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"

// general stuf
#include <iostream>
#include <array>
#include <cassert>
#include <memory>

//project
#include "gpio.hpp"
#include "soundbank.hpp"

using namespace std;

///////////////////////////////////

template <unsigned int N> class Trigger {
private:
  shared_ptr<GPIO<N> > gpio;
  shared_ptr<Soundbank<N> > soundbank;
  // play samples infinitely instead of just once
  bool loopSamples;
  // is the trigger currently active or bypassed?
  bool isActive;

public:

  Trigger(shared_ptr<GPIO<N> > gpio, shared_ptr<Soundbank<N> > soundbank)
      : gpio(gpio), soundbank(soundbank), loopSamples(false), isActive(true) {}

  void toggleLoop() { loopSamples = !loopSamples; }
  void setActive(bool value) { isActive = value; }

  /*
   * needs to be called exactly once per circle
   */
  void step() {
    if (!isActive) {
      return;
    }
    for (unsigned int i = 0; i < N; i++) {
      switch (gpio->get(i)) {
      case(GPIO<N>::State::LOW_LEVEL) :
        break;
      case(GPIO<N>::State::HIGH_LEVEL) :
        break;
      case(GPIO<N>::State::RISING_EDGE) :
        Mix_Pause(i);
        cout << "rise " << i << endl;
        break;
      case(GPIO<N>::State::FALLING_EDGE) :
        Mix_PlayChannel(i, soundbank->get(i), loopSamples ? -1 : 0);
        cout << "fall " << i << endl;
        break;
      }
    }
  }
};
#endif // TRIGGER_H
