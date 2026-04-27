#pragma once
#include <SDL_mixer.h>

namespace Audio {
    // Initialize SDL_mixer and open the audio device
    bool Init();

    // Load and reproduce a music track in loop
    void PlayBGM(const char* filePath, int volume = 40);

    // Stops current music if it's playing and frees the associated memory
    void StopBGM();

    // Closes the audio device and quits SDL_mixer
    void Clean();

    // Adjust the music volume (0-128)
    void SetVolume(int volume);

    // Loads a sound effect into memory and returns the pointer
    Mix_Chunk* LoadSFX(const char* filePath);

    // Reproduce the sound effect on the first available channel with the specified volume
    void PlaySFX(Mix_Chunk* chunk, int volume = 60);

    // Frees the memory allocated for the sound effect when exiting the game
    void FreeSFX(Mix_Chunk* chunk);

}   