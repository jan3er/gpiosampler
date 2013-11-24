// general stuf
#include <iostream>

//project
#include "my_controller.hpp"

using namespace std;

///////////////////////////////////



int main(int argc, char *argv[]) {
  cout << "start gpiosampler" << endl;
  assert(argc > 1);
  MyController c(argv[1]);
  c.run();
  return 0;
}

