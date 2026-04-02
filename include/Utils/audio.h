#pragma once

namespace Audio {
    // Initialize SDL_mixer and open the audio device
    bool Init();

    // Load and reproduce a music track in loop
    void PlayBGM(const char* filePath, int volume = 60);

    // Stops current music if it's playing and frees the associated memory
    void StopBGM();

    // Closes the audio device and quits SDL_mixer
    void Clean();

    // Adjust the music volume (0-128)
    void SetVolume(int volume);
}   