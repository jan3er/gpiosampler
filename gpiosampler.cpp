// traversing directories
#include <dirent.h>

// sound mixer
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"

// GPIO
//#include <wiringPi.h>

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

template <unsigned int N> class GPIO {
private:
  enum State {HIGH, LOW, RISE, FALL};
  array<State, N> buttonState;
  //mapping[i] corresponds to the pin number of button i
  static array<unsigned int, N> mapping;

public:
  /*
   * get state of pin
   */
  State operator()(unsigned int i){
    assert(i < N);
    (void) i;
    cout << mapping[i] << endl;
    return LOW;
  }
};
template <> array<unsigned int, 4> GPIO<4>::mapping{ { 18, 17, 22, 27 } };

///////////////////////////////////

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
  Mix_Chunk *operator()(unsigned int i) {
    assert(i < N);
    return soundbank["a"][i];
  }
};

///////////////////////////////////

template <unsigned int N> class Trigger {
private:
  shared_ptr<GPIO<N> > gpio;
  shared_ptr<Soundbank<N> > soundbank;
  //play samples infinitely instead of just once
  bool loopSamples;

public:

  Trigger(shared_ptr<GPIO<N> > gpio, shared_ptr<Soundbank<N> > soundbank)
      : gpio(gpio), soundbank(soundbank) {}

  void step() {
    int channel = Mix_PlayChannel(-1, (*soundbank)(0), 0);

    while (Mix_Playing(channel) != 0) ;

    (*gpio)(0);
  }
};

///////////////////////////////////

template <unsigned int N> class Controll {
private:
  shared_ptr<GPIO<N> > gpio;
  shared_ptr<Soundbank<N> > soundbank;
  shared_ptr<Trigger<N> > trigger;

  /*
   * start SDL and SDL_mixser
   */
  void openAudio() {
    int audio_rate = 22050;             // Frequency of audio playback
    Uint16 audio_format = AUDIO_S16SYS; // Format of the audio we're playing
    int audio_channels = 2;             // 2 channels = stereo
    int audio_buffers = 64;             // Size of the audio buffers in memory
    bool error;

    // Initialize SDL audio
    error = !SDL_Init(SDL_INIT_AUDIO);
    assert(error && "error audio");

    // Initialize SDL_mixer with our chosen audio settings
    error =
        !Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers);
    assert(error && "error mixer open");
  }

  /*
   * close SDL and SDL_mixser
   */
  void closeAudio() {
    Mix_CloseAudio();
    SDL_Quit();
  }

public:
  Controll()
      : gpio{ new GPIO<N> }, soundbank{ new Soundbank<N> },
        trigger{ new Trigger<N>(gpio, soundbank) } {
    openAudio();
    soundbank->add("soundbank");
    trigger->step();
  }

  ~Controll() { closeAudio(); }
};

//////////////////////////////////////

int main(int argc, char *argv[]) {
  cout << "start" << endl;
  Controll<4> c;

  return 0;
}

/////////////////////////////////// ///////////////////////////////////
//////////////////////////////////////

/*
 * create a soundbank
 */

void setupGPIO() {
  // wiringPiSetupGpio();
  // pinMode (17, INPUT);
  // pullUpDnControl (17, PUD_DOWN);
}

///////////////////////////////////

#include "stdlib.h"

int main2(int argc, char *argv[]) {
  cout << "start" << endl;

  int audio_rate = 22050; // Frequency of audio playback
  Uint16 audio_format = AUDIO_S16SYS; // Format of the audio we're playing
  int audio_channels = 2; // 2 channels = stereo
  int audio_buffers = 64; // Size of the audio buffers in memory
  // int audio_buffers = 4096;	 		//Size of the audio buffers in
  // memory

  // Initialize SDL audio
  if (SDL_Init(SDL_INIT_AUDIO) != 0) {
    cout << "error audio" << endl;
    return 1;
  }

  // Initialize SDL_mixer with our chosen audio settings
  if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) !=
      0) {
    cout << "error mixer open" << endl;
    exit(1);
  }

  // load sounds from file
  map<string, array<Mix_Chunk *, 4> > soundbank;
  // readSoundbank(soundbank);

  // wait for some gpio input
  setupGPIO();
  while (1) {
    // while(!digitalRead(17));

    // Play our sound file, and capture the channel on which it is played
    int channel = Mix_PlayChannel(-1, soundbank["a"][0], 0);
    if (channel == -1) {
      cout << "error channel" << endl;
    }
    // Wait until the sound has stopped playing
    while (Mix_Playing(channel) != 0)
      ;
  }

  // Release the memory allocated to our sound
  // Mix_FreeChunk(sound);

  // Need to make sure that SDL_mixer and SDL have a chance to clean up
  Mix_CloseAudio();
  SDL_Quit();

  // Return success!
  return 0;
}
