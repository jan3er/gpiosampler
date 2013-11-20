// general stuf
#include <iostream>

//project
#include "gpio.hpp"
#include "soundbank.hpp"
#include "trigger.hpp"
#include "controller.hpp"

using namespace std;

///////////////////////////////////

int main(int argc, char *argv[]) {
  (void) argc;
  (void) argv;
  cout << "start" << endl;
  Controller<7> c;
  c.run();

  return 0;
}

