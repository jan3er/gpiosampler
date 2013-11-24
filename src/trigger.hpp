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
public:
  /*
   * PLAY_ONCE:
   * each time the button is pressed down, play the chunk exactly once
   *
   * PLAY_WHILE_PRESSED:
   * when the button is pressed down, start play the chunk repeatedly until
   * the button is released
   *
   * PLAY_UNTIL_PRESSED_AGAIN:
   * when the button is pressed down, start play the chunk repeatedly until
   * the button is pressed again
   *
   * PLAY_ONE_AT_A_TIME:
   * like play_until_pressed_again, but stop sample if another one starts
   */
  enum Mode {
    PLAY_ONCE,
    PLAY_WHILE_PRESSED,
    PLAY_UNTIL_PRESSED_AGAIN,
    PLAY_ONE_AT_A_TIME
  };

private:
  shared_ptr<GPIO<N> > gpio;
  shared_ptr<Soundbank<N> > soundbank;
  // make noise only if this is true
  bool isActive;
  Mode mode;

public:

  Trigger(shared_ptr<GPIO<N> > gpio, shared_ptr<Soundbank<N> > soundbank)
      : gpio(gpio), soundbank(soundbank), isActive(true),
        mode(PLAY_ONCE) {}

  void setActive(bool value) { isActive = value; }
  void setMode(Mode m) { mode = m; }
  Mode getMode() { return mode; }

  /*
   * to be called exactly once per circle
   */
  void step() {
    for (unsigned int i = 0; i < N; i++) {
      if (!isActive) {
        Mix_HaltChannel(i);
      } else {
        typename GPIO<N>::State state = gpio->get(i);
        switch (mode) {

        case(Mode::PLAY_ONCE) :
          if (state == GPIO<N>::State::FALLING_EDGE) {
            Mix_PlayChannel(i, soundbank->get(i), 0);
            cout << "=> a_play " << i << endl;
          }
          break;

        case(Mode::PLAY_WHILE_PRESSED) :
          if (state == GPIO<N>::State::FALLING_EDGE) {
            Mix_PlayChannel(i, soundbank->get(i), -1);
            cout << "=> b_play " << i << endl;
          }
          else if (state == GPIO<N>::State::RISING_EDGE) {
            Mix_HaltChannel(i);
            cout << "=> b_stop " << i << endl;
          }
          break;

        case(Mode::PLAY_UNTIL_PRESSED_AGAIN) :
          if (state == GPIO<N>::State::FALLING_EDGE) {
            if (Mix_Playing(i)) {
              Mix_HaltChannel(i);
              cout << "=> c_stop " << i << endl;
            } else {
              Mix_PlayChannel(i, soundbank->get(i), -1);
              cout << "=> c_play " << i << endl;
            }
          }
          break;

        case(Mode::PLAY_ONE_AT_A_TIME) :
          if (state == GPIO<N>::State::FALLING_EDGE) {
            if (Mix_Playing(i)) {
              Mix_HaltChannel(i);
              cout << "=> d_stop " << i << endl;
            } else {
              Mix_HaltChannel(-1);
              Mix_PlayChannel(i, soundbank->get(i), -1);
              cout << "=> d_play " << i << endl;
            }
          }
          break;
        }
      }
    }
  }
};

#endif // TRIGGER_H
