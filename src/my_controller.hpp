#ifndef MY_CONTROLLER_H
#define MY_CONTROLLER_H

// general stuf
#include <iostream>
#include <array>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <cassert>
#include <memory>

//project
#include "controller.hpp"

using namespace std;

///////////////////////////////////

#define NUMBER_OF_PINS 7
typedef Trigger<NUMBER_OF_PINS> MyTrigger;
typedef GPIO<NUMBER_OF_PINS> MyGPIO;
typedef GPIO<NUMBER_OF_PINS> MyGPIO;

///////////////////////////////////

class MyController : public Controller<7> {
private:
  static const array<unsigned int, 2> activateButtons;
  static const MyTrigger::Mode defaultMode;
  bool isActive;
  bool activateButtonsDown;

  /*
   * to be called once each circle
   */
  void step() {
    // impulse is true if all activate buttons are pressed and have not been
    // pressed before
    bool impulse = false, tmp = true;
    for (unsigned int i : activateButtons) {
      tmp &= gpio->get(i) == MyGPIO::State::LOW_LEVEL;
    }
    if(tmp != activateButtonsDown) {
      activateButtonsDown = tmp;
      if(activateButtonsDown) {
        isActive = !isActive;
        impulse = true;
      }
    }

    //stuff to do when activated or deactivated
    if(isActive && impulse) {
      trigger->setActive(false);
      cout << "activate controller!" << endl;
    }
    if(!isActive && impulse) {
      trigger->setActive(true);
      cout << "deactivate controller!" << endl;
    }

    if (isActive) {
      if (gpio->get(1) == MyGPIO::State::FALLING_EDGE) {
        switch (trigger->getMode()) {
        case MyTrigger::Mode::PLAY_ONCE:
          trigger->setMode(MyTrigger::Mode::PLAY_WHILE_PRESSED);
          cout << "PLAY_WHILE_PRESSED;" << endl;
          break;
        case MyTrigger::Mode::PLAY_WHILE_PRESSED:
          trigger->setMode(MyTrigger::Mode::PLAY_UNTIL_PRESSED_AGAIN);
          cout << "PLAY_UNTIL_PRESSED_AGAIN;" << endl;
          break;
        case MyTrigger::Mode::PLAY_UNTIL_PRESSED_AGAIN:
          trigger->setMode(MyTrigger::Mode::PLAY_ONE_AT_A_TIME);
          cout << "PLAY_ONE_AT_A_TIME;" << endl;
          break;
        case MyTrigger::Mode::PLAY_ONE_AT_A_TIME:
          trigger->setMode(MyTrigger::Mode::PLAY_ONCE);
          cout << "PLAY_ONCE;" << endl;
          break;
        }
      }
    }
  }


public:
  MyController(string pathToSoundbank)
      : Controller(pathToSoundbank), isActive(false),
        activateButtonsDown(false) {
    trigger->setMode(defaultMode);
    gpio->step();
    if(gpio->get(0) != MyGPIO::State::FALLING_EDGE) {
        cout << "press first button while starting" << endl;
        exit(1);
    }
  };
};

/////////////////////////////////////

// pins on the board to which the indices 0 to NUMBER_OF_PINS-1 are mapped to
template <>
const array<unsigned int, NUMBER_OF_PINS> MyGPIO::mapping{ { 18, 17, 27, 22,
                                                             23, 24, 25 } };

// the buttons to press to go in config mode
const array<unsigned int, 2> MyController::activateButtons{ { 0, 6 } };

const MyTrigger::Mode MyController::defaultMode = MyTrigger::Mode::PLAY_WHILE_PRESSED;


#endif // MY_CONTROLLER_H
