#include <Utils/audio.h>
#include <Utils/logs.h> 
#include <SDL_mixer.h>

namespace Audio {
    // Private variables to manage audio state
    static Mix_Music* currentMusic = nullptr;
    static bool isInitialized = false;

    // Initialize SDL_mixer and open the audio device function
    bool Init() {
        if (isInitialized) return true;

        // 48000Hz native audio rate for the Switch, 16-bit signed audio, stereo, and a 2048 byte chunk size
        if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            TRACE("Mix_OpenAudio Error: %s", Mix_GetError());
            return false;
        }

        // Initialize support for MP3 format
        Mix_Init(MIX_INIT_MP3);
        Mix_AllocateChannels(10);

        isInitialized = true;
        TRACE("Audio module initialized correctly.");
        return true;
    }

    // Load and reproduce a music track in loop
    void PlayBGM(const char* filePath, int volume) {
        if (!isInitialized) return;

        // Stops current music if it's playing before loading a new track
        StopBGM();

        currentMusic = Mix_LoadMUS(filePath);
        
        if (currentMusic != nullptr) {
            Mix_VolumeMusic(volume);
            Mix_PlayMusic(currentMusic, -1); // -1 means loop indefinitely
            TRACE("Reproducing music: %s", filePath);
        } else {
            TRACE("Error loading music %s: %s", filePath, Mix_GetError());
        }
    }

    // Stops current music if it's playing and frees the associated memory
    void StopBGM() {
        if (currentMusic != nullptr) {
            Mix_HaltMusic();
            Mix_FreeMusic(currentMusic);
            currentMusic = nullptr;
        }
    }

    // Closes the audio device and quits SDL_mixer
    void Clean() {
        StopBGM(); // Stop any music that might be playing

        if (isInitialized) {
            Mix_CloseAudio();
            Mix_Quit();
            isInitialized = false;
            TRACE("Audio closed and memory released.");
        }
    }

    // Adjust the music volume 
    void SetVolume(int volume) {
        if (!isInitialized) return;

        // Volume saturation (0-128)
        if (volume < 0){
            volume = 0;
        } else if (volume > 128) {
            volume = 128;
        }

        Mix_VolumeMusic(volume);
        TRACE("Music volume adjusted to %d", volume);
    }

    // Loads a sound effect into memory and returns the pointer
    Mix_Chunk* LoadSFX(const char* filePath) {
        if (!isInitialized) return nullptr;
        
        Mix_Chunk* chunk = Mix_LoadWAV(filePath);
        if (chunk == nullptr) {
            TRACE("Error loading SFX %s: %s", filePath, Mix_GetError());
        } else {
            TRACE("SFX loaded correctly: %s", filePath);
        }
        return chunk;
    }

    // Reproduce the sound effect on the first available channel with the specified volume
    void PlaySFX(Mix_Chunk* chunk, int volume) {
        if (!isInitialized || chunk == nullptr) return;
        
        Mix_VolumeChunk(chunk, volume);
        // -1 means SDL_mixer will automatically find the first available channel that is not currently playing
        Mix_PlayChannel(-1, chunk, 0); 
    }

    // Frees the memory allocated for the sound effect when exiting the game
    void FreeSFX(Mix_Chunk* chunk) {
        if (chunk != nullptr) {
            Mix_FreeChunk(chunk);
        }
    }
}