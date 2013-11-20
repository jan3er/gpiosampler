#ifndef SOUNDBANK_H
#define SOUNDBANK_H

// traversing directories
#include <dirent.h>

// sound mixer
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"

// general stuf
#include <iostream>
#include <array>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <cassert>
#include <memory>

using namespace std;

//////////////////////////////////////

template <unsigned int N> class Soundbank {
private:
  // store for every name N samples
  map<string, array<Mix_Chunk *, N> > soundbank;
  // the name of the current soundbank
  string currentBank;

public:

  /*
   * add directory to soundbank
   * assures that there are no nullptrs
   * existing samples are overwritten
   */
  void add(const string &pathToSoundbank) {
     
    // traverse directory
    DIR *pdir = opendir(pathToSoundbank.c_str());
    assert(pdir && "hasdfasdf");
    struct dirent *pent = nullptr;

    while ((pent = readdir(pdir))) {
      string path(pent->d_name);
      stringstream ss(path);
      string item;
      vector<string> elems;

      while (std::getline(ss, item, '.')) {
        elems.push_back(item);
      }

      if (elems.size() == 3 && elems[2].compare("wav") == 0) {
        string fullpath = "soundbank/" + path;
        Mix_Chunk *chunk = Mix_LoadWAV(fullpath.c_str());
        assert(chunk != nullptr);
        unsigned int index = atoi(elems[1].c_str());
        if (index < N) {
          cout << "add " << elems[0] << "." << index << ".wav" << endl;
          soundbank[elems[0]][index] = chunk;
        }
      }
    }
    closedir(pdir);

    // assure that there are no nullptrs
    for (auto &i : soundbank) {
      for (auto &j : i.second) {
	(void)j;
	assert(j != nullptr);
      }
    }

    // for the first add set current soundbank
    if (currentBank.empty()) {
      currentBank = soundbank.begin()->first;
    }
  }

  /*
   * clear the whole soundbank
   */
  void clear() { assert(0 && "todo"); }

  /*
   * get the i-th sound of the current bank
   */
  Mix_Chunk *get(unsigned int i) {
    assert(i < N);
    return soundbank["a"][i];
  }
};

#endif //SOUNDBANK_H
