#ifndef GPIO_H
#define GPIO_H

// GPIO
#include <wiringPi.h>

// general stuf
#include <iostream>
#include <array>
#include <cassert>

using namespace std;

////////////////////////////////////////////////

template <unsigned int N> class GPIO {
public:
  enum State {HIGH_LEVEL, LOW_LEVEL, RISING_EDGE, FALLING_EDGE};
private:
  array<State, N> buttonState;
  //mapping[i] corresponds to the pin number of button i
  static const array<unsigned int, N> mapping;

public:

  GPIO(){
    wiringPiSetupGpio();
    for(unsigned int i : mapping) {
      pinMode(i, INPUT);
      pullUpDnControl(i, PUD_UP);
    }
  }

  ~GPIO(){}
  /*
   * needs to be called exactly once per circle
   */
  void step()
  {
    for (unsigned int i = 0; i < N; i++) {
      if (digitalRead(mapping[i])) {
        // high level
        switch (buttonState[i]) {
        case State::HIGH_LEVEL:
          buttonState[i] = HIGH_LEVEL;
          break;
        case State::LOW_LEVEL:
          buttonState[i] = RISING_EDGE;
          cout << "rise " << i << endl;
          break;
        case State::RISING_EDGE:
          buttonState[i] = HIGH_LEVEL;
          break;
        case State::FALLING_EDGE:
          buttonState[i] = RISING_EDGE;
          cout << "rise " << i << endl;
          break;
        }
      } else {
        // low level
        switch (buttonState[i]) {
        case State::HIGH_LEVEL:
          buttonState[i] = FALLING_EDGE;
          cout << "fall " << i << endl;
          break;
        case State::LOW_LEVEL:
          buttonState[i] = LOW_LEVEL;
          break;
        case State::RISING_EDGE:
          buttonState[i] = FALLING_EDGE;
          cout << "fall " << i << endl;
          break;
        case State::FALLING_EDGE:
          buttonState[i] = LOW_LEVEL;
          break;
        }
      }
    }
  }
  /*
   * get state of pin
   */
  State get(unsigned int i){
    assert(i < N);
    return buttonState[i];
  }
};
#endif //GPIO_H
