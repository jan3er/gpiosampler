// general stuf
#include <iostream>

//project
#include "my_controller.hpp"

using namespace std;

///////////////////////////////////



int main(int argc, char *argv[]) {
  (void) argc;
  (void) argv;
  cout << "start" << endl;
  MyController c;
  c.run();
  return 0;
}

