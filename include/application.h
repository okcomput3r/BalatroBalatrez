#pragma once

#define GAME_SPEED_MULT 5

struct estadoo {
    int puntuacionGlobal = 0;
    float fichas = 0.0f;
    float mult = 0.0f;
    int dinero = 0;
    int manos = 0;
    int descartes = 0;
    int ronda = 0;
};

namespace APP
{

    

    /***
    *
    * int ConfigureApplication() takes care of all the inicialization progress that needs to be taken care
    * of before starting to render.
    * In this process the logs are created, the window is allocated, and all the SDL/GLES context
    * is configured
    * 
    * @return int result -> Returns -1 if the application couldn't initialize
    *
    */
    int ConfigureApplication();

    int SetupScene();

    void Render();

    void Update();

    void CleanApplication();

}