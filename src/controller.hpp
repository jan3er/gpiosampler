#ifndef CONTROLLER_H
#define CONTROLLER_H

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

//project
#include "gpio.hpp"
#include "soundbank.hpp"
#include "trigger.hpp"

using namespace std;

///////////////////////////////////

template <unsigned int N> class Controller {
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
  Controller()
      : gpio{ new GPIO<N> }, soundbank{ new Soundbank<N> },
        trigger{ new Trigger<N>(gpio, soundbank) } {
    openAudio();
    soundbank->add("soundbank");
    trigger->step();
  }

  ~Controller() { closeAudio(); }

  void run() {
    while (1) {
        gpio->step();
        trigger->step();
    }
  }
};
#endif //CONTROLLER_H
