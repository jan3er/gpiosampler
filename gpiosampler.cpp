//traversing directories
#include <dirent.h>

//sound mixer
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"

//general stuf
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <cassert>

using namespace std;

///////////////////////////////////

template<unsigned int N>
void readSoundbank(map<string, array<Mix_Chunk*, N>> &soundbank)
{
    DIR *pdir = opendir ("soundbank");
    struct dirent *pent = nullptr;

    while ((pent = readdir (pdir)))
    {
	string path(pent->d_name);
	stringstream ss(path);
	string item;
        vector<string> elems;

	while (std::getline(ss, item, '.')) {
	    elems.push_back(item);
	}

	if(elems.size() == 3 && elems[2].compare("wav") == 0)
	{
	    string fullpath = "soundbank/" + path;
	    Mix_Chunk* chunk = Mix_LoadWAV(fullpath.c_str());
	    assert(chunk != nullptr);
	    soundbank[elems[0]][atoi(elems[1].c_str())] = chunk;
	}
    }

    for(auto &i : soundbank)
    {
	for(auto &j : i.second)
	{
	    (void) j;
	    assert(j != nullptr);
	}
    }
 
    closedir (pdir);
}

///////////////////////////////////

#include "stdlib.h"

int main(int argc, char *argv[])
{

	SDL_Surface *screen;			//Pointer to the main screen surface
	Mix_Chunk *sound = NULL;		//Pointer to our sound, in memory
	int channel;				//Channel on which our sound is played
	  
	int audio_rate = 22050;			//Frequency of audio playback
	Uint16 audio_format = AUDIO_S16SYS; 	//Format of the audio we're playing
	int audio_channels = 2;			//2 channels = stereo
	int audio_buffers = 4096;		//Size of the audio buffers in memory
	
	//Initialize BOTH SDL video and SDL audio
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		cout << "error" << endl;
		return 1;
	}
	
	//Initialize SDL_mixer with our chosen audio settings
	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
		cout << "error" << endl;
		exit(1);
	}
	
	//Load our WAV file from disk
	sound = Mix_LoadWAV("soundbank/a.0.wav");
	if(sound == NULL) {
		cout << "error" << endl;
	}

	map<string, array<Mix_Chunk*, 4>> soundbank;
	readSoundbank(soundbank);
	
	//Set the video mode to anything, just need a window
	screen = SDL_SetVideoMode(320, 240, 0, SDL_ANYFORMAT);
	if (screen == NULL) {
		cout << "error" << endl;
		return 1;
	}
	
	//Play our sound file, and capture the channel on which it is played
	channel = Mix_PlayChannel(-1, soundbank["a"][3], 0);
	if(channel == -1) {
		cout << "error" << endl;
	}

	
	//Wait until the sound has stopped playing
	while(Mix_Playing(channel) != 0);
	
	//Release the memory allocated to our sound
	Mix_FreeChunk(sound);
	
	//Need to make sure that SDL_mixer and SDL have a chance to clean up
	Mix_CloseAudio();
	SDL_Quit();	
	
	//Return success!
	return 0;
}
