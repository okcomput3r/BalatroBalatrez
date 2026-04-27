#include <application.h>
#include <Utils/logs.h>

// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <queue>



#include <dirent.h>

// Include the main libnx system header, for Switch development
#include <switch.h>

// GRAPHICS AND IMPUTS INCLUDES
#include <SDL.h>
#include <SDL_video.h>
#include <SDL_opengles2.h>
#include <GLES3/gl3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Custom headers

#include <Graphics/shaders.h>
#include <Graphics/textures.h>
#include <Graphics/cards.h>
#include <Graphics/image.h>
#include <Graphics/jokers.h>  
#include <Graphics/pause.h>
#include <Graphics/store.h>

#include <Utils/logs.h>
#include <Utils/math.h>
#include <Utils/audio.h>

#include <logic.h>

// some switch buttons
#define JOY_A     0
#define JOY_B     1
#define JOY_X     2
#define JOY_Y     3
#define JOY_PLUS  10
#define JOY_MINUS 11
#define JOY_LEFT  12
#define JOY_UP    13
#define JOY_RIGHT 14
#define JOY_DOWN  15


// switch pad
PadState pad;

//SDL/GLES Global Contex

SDL_Event event;
SDL_Window *window;
SDL_GLContext GLcontext;  // GLES (OpenGL for Embeded System) context for shaders and whatever  

// GLOBAL 
std::vector<GLuint> globalDeck;
std::vector<GLuint> globalHand;
int cursor;
bool pausa = true;

int cursorInterfaz;
bool shop = false;

std::string manoJugada = "";
std::string orden = "POR PALOS";
bool typeOfSort = true;


ImageData backgroundLogo;
ImageData imagee;
ImageData atlasss;
ImageData Jokers;  
ImageData barra;


PauseMenuState menuPausa;
StoreMenuState menuStore;
//EstadoPartida estadoPartida;


Mix_Chunk* s_sfxCardSelect = nullptr;
Mix_Chunk* s_sfxCardUnselect = nullptr;

uint8_t maxSelectedCards = 5;
uint8_t selectedCardsCount = 0;

uint8_t handSize = 8;

uint8_t maxJokerHandSize = 5;




int APP::ConfigureApplication()
{
    TRACE("APP INITIALIZATION");


    // Innit romfs and logs
    romfsInit();

    if (innitLogs() == -1) { return -1; }

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0 )
    {
        TRACE("SDL_Init: %s\n", SDL_GetError());
        return -1;
    }

    // Initialize audio module
    Audio::Init();

    // Configs context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES); 
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "1"); 

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    window = SDL_CreateWindow("Prueba de cartas", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN);
    if (!window) {
        TRACE("SDL_CreateWindow: %s\n", SDL_GetError());
        return -1;
    }

    GLcontext = SDL_GL_CreateContext(window);
    if(!GLcontext)
    {
        TRACE("SDL_GL_CreateContext: %s\n", SDL_GetError());
        return -1;
    }


    // Carga todas las funciones de OpenGl
    int version = gladLoadGLLoader(SDL_GL_GetProcAddress);
    if(version == 0) {
        TRACE("Unable To load GLAD: %s\n", SDL_GetError());
        return -1;
    }


    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // configuración del pad 1
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);


    TRACE("APP INICIALIZATED");
    return 0;
}
GLuint cardHandle;

int APP::SetupScene()
{
    TRACE("START SETUP OF SCENE");

    InitializeImage(backgroundLogo, 1920.0f / 2, 2000.0f, "Font.png");
    InitializeImage(imagee, 1920.0f / 2, 800.0f, "Font.png");
    InitializeImage(barra, 100.0f, 0.0f, "barraInfo.png");

    InitializeImage(atlasss, 0, 0, "Font.png");
    InitializeImage(Jokers, 0, 0, "Jokers.png");  

    InitializeAtlas(atlasss, 15, 8, 20.0f, 20.0f);
    InitializeAtlas(Jokers, 10, 16, 142.0f, 190.0f);

    InitPauseMenu(menuPausa);
    InitStoreMenu(menuStore);



   

    // Cards.h
    LoadBaseCardBuffers();
    LoadBaseCardShader();
    LoadBaseCardTextures();

    LoadDeck(globalDeck);


    for (int i = 1; i <= handSize; i++ )
    {
        //AddCardToHand(i, globalHand, globalDeck);
        DrawTopCard(globalHand, globalDeck);
    }

    if (typeOfSort) {SortHandSuit(globalHand);} else {SortHandValue(globalHand);}

    

    // Posiciona el cursor al principio de la baraja
    if (pausa){
        cursor = 0;
    }

    // Load SFX into memory
    s_sfxCardSelect = Audio::LoadSFX("romfs:/data/audio/card_select.wav");
    s_sfxCardUnselect = Audio::LoadSFX("romfs:/data/audio/card_unselect.wav");

    // Load and play background music only when the scene is setted up correctly
    Audio::PlayBGM("romfs:/data/audio/Balatro_Theme.mp3", 60);
    Audio::SetVolume(60); // Volume already set in PlayBGM, but can be adjusted later with this function

    TRACE("SCENE SETTED AND MUSIC STARTED");

    IniciarNuevaPartida();
    ConfigurarCiega(300.0f); 
    

    return 0;
}

float finalPos[3] = {SCREEN_W/2,SCREEN_H/2, 0.0f};
float endAngle = 180.0f;

uint32_t tick = SDL_GetTicks();
uint32_t last_tick = SDL_GetTicks();;
float delta_time;

void APP::Update()
{
    
    tick = SDL_GetTicks();
    // Cuántos segundos han pasado desde el último frame (ej. 0.016s a 60fps)
    delta_time = (tick - last_tick) / 1000.0f; 
    last_tick = tick;
    

    padUpdate(&pad);
    u64 botonesPulsados = padGetButtonsDown(&pad);

    if (botonesPulsados & HidNpadButton_B) {
        pausa = !pausa;
        if (menuPausa.targetMenuY == 0.0f) {
            menuPausa.targetMenuY = 1500.0f; // Si estaba arriba, lo mandamos abajo
        } else {
            menuPausa.targetMenuY = 0.0f;    // Si estaba abajo, lo mandamos arriba
        }
    }


    
    

    if (pausa){

        if (estadoPartida.faseActual == PHASE_PLAYING){

            if (botonesPulsados & HidNpadButton_Left) {
                if (cursor > 0) {
                    cursor--; 
                }
            }

            if (botonesPulsados & HidNpadButton_Right) {
                
                if ((size_t) cursor < globalHand.size() - 1) { 
                    cursor++; 
                }
                
            }

            // =======================================================
            // BOTÓN A: SELECCIONAR Y DESELECCIONAR CARTAS
            // =======================================================

            if (botonesPulsados & HidNpadButton_A) {
                uint8_t result;
                Card &cardRef = RetrieveCardReference(globalHand[cursor], result);

                if( (selectedCardsCount >= 0 && cardRef.selected) || (selectedCardsCount < maxSelectedCards && !cardRef.selected) )
                    {

                    if(!cardRef.selected) 
                    {
                        TRACE("Selecting Card with ID %d", cardRef.ID);

                        selectedCardsCount++;

                        Audio::PlaySFX(s_sfxCardSelect, 60);
                    }else{
                        TRACE("Unselecting Card with ID %d", cardRef.ID);

                        selectedCardsCount--;

                        Audio::PlaySFX(s_sfxCardUnselect, 60);
                    }
                    cardRef.selected = !cardRef.selected;
                
                }

                
            }

            manoJugada = EvaluateSelectedHand(globalHand);
            
            std::vector<unsigned int> cartasSeleccionadas;
            for (size_t i = 0; i < globalHand.size(); i++) {
                uint8_t result;
                Card &cardRef = RetrieveCardReference(globalHand[i], result);
                if (result && cardRef.selected) {
                    cartasSeleccionadas.push_back(globalHand[i]); 
                }
            }

            CalcularPuntuacionPrevia(manoJugada, cartasSeleccionadas);
            
            // =======================================================
            // BOTÓN X: JUGAR LA MANO
            // =======================================================
            if (botonesPulsados & HidNpadButton_X)
            {
                if (selectedCardsCount > 0)
                {
                
                    JugarMano(manoJugada, cartasSeleccionadas);

                    
                    RemoveCardsFromHand(globalHand);
                    selectedCardsCount = 0;
                
                    
                    while(globalDeck.size() > 0 && globalHand.size() < handSize) {
                        DrawTopCard(globalHand, globalDeck);
                        Audio::PlaySFX(s_sfxCardSelect, 60);
                    }
                    
                    if (typeOfSort) {SortHandSuit(globalHand);} else {SortHandValue(globalHand);}
                    cursor = 0;
                }
            }

            // =======================================================
            // BOTÓN Y: DESCARTAR
            // =======================================================
            if (botonesPulsados & HidNpadButton_Y) 
            {
                
                if (selectedCardsCount > 0 && estadoPartida.descartes > 0) 
                {
                    DescartarCartas(); 
                    
                    RemoveCardsFromHand(globalHand);
                    selectedCardsCount = 0;
                
                    while(globalDeck.size() > 0 && globalHand.size() < handSize) {
                        DrawTopCard(globalHand, globalDeck);
                        Audio::PlaySFX(s_sfxCardSelect, 60);
                    }
                    if (typeOfSort) {SortHandSuit(globalHand);} else {SortHandValue(globalHand);}
                    cursor = 0;
                }  
            }

            // =======================================================
            // BOTÓN MINUS (-): ORDENAR LA MANO
            // ======================================================
            if (botonesPulsados & HidNpadButton_Minus) {
                typeOfSort = !typeOfSort;
                if (typeOfSort) {SortHandSuit(globalHand);} else {SortHandValue(globalHand);}  
            }
        }

        else if (estadoPartida.faseActual == PHASE_SHOP) {

            
            if (menuStore.targetMenuY == 1500.0f) {
                menuStore.targetMenuY = 0.0f;
                shop = !shop;
            }

            // =======================================================
            // BOTÓN MÁS (+): PASAR A LA SIGUIENTE CIEGA
            // ======================================================
            if (botonesPulsados & HidNpadButton_Plus) {
                if (menuStore.targetMenuY == 0.0f) {
                    menuStore.targetMenuY = 1500.0f;
                    shop = !shop;
                }

                AvanzarSiguienteCiega();
                
                for (size_t i = 0; i < globalHand.size(); i++) DestroyCard(globalHand[i]);
                for (size_t i = 0; i < globalDeck.size(); i++) DestroyCard(globalDeck[i]);
                
                globalHand.clear();
                globalDeck.clear();
                selectedCardsCount = 0;

                LoadDeck(globalDeck);

                for (int i = 0; i < handSize; i++) {
                    DrawTopCard(globalHand, globalDeck);
                }

                if (typeOfSort) {SortHandSuit(globalHand);} else {SortHandValue(globalHand);}
                cursor = 0;
            }
        }

        // Actualización gráfica general
        UpdateHand(globalHand, cursor, delta_time);
        if(typeOfSort){ orden = "POR PALOS";} else {orden = "POR VALOR";}
        
        updateLogs();

    }
    UpdatePauseMenu(menuPausa, botonesPulsados, delta_time, pausa);
    UpdateStoreMenu(menuStore, botonesPulsados, delta_time, pausa);
}




void APP::Render()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    float screen_width = 1920.0f;
    float screen_height = 1080.0f;

    float jokers_spacing = 125.0f;
    float jokers_start_x = (screen_width / 2.0f) - (jokers_spacing * (ownedJokers.size() - 1.0f) / 2.0f);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 projection = glm::ortho(0.0f, (float)screen_width, (float)screen_height, 0.0f, -1.0f, 1.0f);

    glm::mat4 model = glm::mat4(1.0f);




    RenderHand(globalHand);
    RenderDeck(globalDeck);

    DibujarImagen(barra, projection, model, 300.0f, 540.0f);

    

    DrawText(atlasss,std::to_string(ownedJokers.size()) +" / " + std::to_string(maxJokerHandSize)+ " JOKERS", projection, (screen_width/2.0f) + 500.0f, 130.0f, 1.5f, -5.0f);
    
    for (int i = 0; i < ownedJokers.size(); i++)
    {
        int marcoJokers = ownedJokers[i].atlasIndex;
        DrawAtlasSprite(Jokers, marcoJokers, projection,  jokers_start_x + jokers_spacing * i , 80.0f, 0.8f); 
    }



    DrawText(atlasss, manoJugada, projection, 300.0f, 520.0f, 1.5f, -5.0f);



    DrawText(atlasss, std::to_string((int)estadoPartida.fichas), projection, 190.0f, 605.0f, 2.0f, -5.0f);
    DrawText(atlasss, std::to_string((int)estadoPartida.mult), projection, 405.0f, 605.0f, 2.0f, -5.0f);

    DrawText(atlasss, std::to_string(estadoPartida.manos), projection, 315.0f, 750.0f, 1.5f, -5.0f);
    DrawText(atlasss, std::to_string(estadoPartida.descartes), projection, 445.0f, 750.0f, 1.5f, -5.0f);

    // Dibuja la puntuación total acumulada y el objetivo de la ciega
    DrawText(atlasss, std::to_string((int)estadoPartida.puntuacionGlobal), projection, 360.0f, 415.0f, 1.5f, -5.0f);
    DrawText(atlasss, "Anota al menos: ", projection, 320.0f, 150.0f, 1.5f, -5.0f);
    DrawText(atlasss, std::to_string((int)estadoPartida.ciegaObjetivo), projection, 300.0f, 180.0f, 1.5f, -5.0f);

    DrawText(atlasss,"$" + std::to_string(estadoPartida.dinero), projection, 380.0f, 850.0f, 1.5f, -5.0f);
    DrawText(atlasss, std::to_string(estadoPartida.ronda), projection, 445.0f, 970.0f, 1.5f, -5.0f);


    DrawText(atlasss, "ORDEN:\n" + orden, projection, 825.0f, 950.0f, 1.5f, -5.0f);

    // Dibuja el menú de la tienda si has ganado
    if (estadoPartida.faseActual == PHASE_SHOP) {
        // DrawText(atlasss, "¡CIEGA SUPERADA!", projection, 900.0f, 300.0f, 3.0f, -5.0f);
        // DrawText(atlasss, "BIENVENIDO A LA TIENDA", projection, 900.0f, 400.0f, 2.0f, -5.0f);
        // DrawText(atlasss, "PULSA + PARA LA SIGUIENTE CIEGA", projection, 900.0f, 600.0f, 1.5f, -5.0f);
        
    }

    RenderPauseMenu(menuPausa, projection, model, imagee);

    
    RenderStoreMenu(menuStore, projection, model, imagee);
    RenderStoreMenuDescriptions(menuStore, projection, model, menuStore.arraySeleccionados, 3, !shop);
   
    


    SDL_GL_SwapWindow(window);
    //TRACE("END OF RENDER at time %d", SDL_GetTicks());
    //TRACE("ms: %f", delta_time);
}




void APP::CleanApplication()
{   
    //DestroyImage(backgroundLogo);
    //destroyShaderProgram(cardBaseShader);
    //texDestroy(textureFront);
    //texDestroy(textureBack);
    Audio::FreeSFX(s_sfxCardSelect);
    Audio::FreeSFX(s_sfxCardUnselect);
    Audio::Clean(); 
    terminateLogs();
    SDL_Quit();
}