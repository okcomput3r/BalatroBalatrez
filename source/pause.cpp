#include <Graphics/pause.h>
#include <cmath>
#include <SDL.h>
#include <stdlib.h>
#include <Utils/math.h>




void InitPauseMenu(PauseMenuState& pauseState) {
    // Cargamos el atlas de los botones de pausa y las imágenes de fondo
    InitializeImage(pauseState.background, 1920.0f /2.0f, 1080.0f / 2, "fondoPausa.png");
    InitializeImage(pauseState.difuminado, 1920.0f /2.0f, 1080.0f / 2, "difuminadoNegro.png");

    InitializeImage(pauseState.uiAtlas, 0.0f, 0.0f, "botonesPausa.png");
    InitializeAtlas(pauseState.uiAtlas, 1, 4, 339.0f, 54.0f);

    // 2. Definimos los recortes (Fondo y botones)
    //DefineAtlasSprite(pauseState.uiAtlas, SPRITE_PAUSE_BG, 0.0f, 0.0f, 1920.0f, 1080.0f); // Fondo oscuro semi-transparente
    DefineAtlasSprite(pauseState.uiAtlas, 0, 0.0f, 0.0f, 339.0f, 54.0f);
    DefineAtlasSprite(pauseState.uiAtlas, 1, 0.0f, 54.0f, 339.0f, 54.0f);
    DefineAtlasSprite(pauseState.uiAtlas, 2, 0.0f, 108.0f, 339.0f, 54.0f);
    DefineAtlasSprite(pauseState.uiAtlas, 3, 0.0f, 162.0f, 339.0f, 54.0f);

    //Inicializamos el fondo

    // 3. Creamos los botones matemáticos
    PauseButton btnResume;
    btnResume.spriteID = 3;
    btnResume.baseX = 790.0f; 
    btnResume.posX = btnResume.targetX = btnResume.baseX;
    btnResume.posY = btnResume.targetY = 400.0f;

    PauseButton btnHandTypes;
    btnHandTypes.spriteID = 2;
    btnHandTypes.baseX = 790.0f; 
    btnHandTypes.posX = btnHandTypes.targetX = btnHandTypes.baseX;
    btnHandTypes.posY = btnHandTypes.targetY = 470.0f;

    PauseButton btnControls;
    btnControls.spriteID = 1;
    btnControls.baseX = 790.0f; 
    btnControls.posX = btnControls.targetX = btnControls.baseX;
    btnControls.posY = btnControls.targetY = 540.0f;

    PauseButton btnQuit;
    btnQuit.spriteID = 0;
    btnQuit.baseX = 790.0f;
    btnQuit.posX = btnQuit.targetX = btnQuit.baseX;
    btnQuit.posY = btnQuit.targetY = 610.0f;

    pauseState.buttons.push_back(btnResume);
    pauseState.buttons.push_back(btnHandTypes);
    pauseState.buttons.push_back(btnControls);
    pauseState.buttons.push_back(btnQuit);
    
    
    pauseState.selectedIndex = 0;

    pauseState.menuY = 1500.0f;
    pauseState.targetMenuY = 1500.0f;
}

void UpdatePauseMenu(PauseMenuState& pauseState, u64 botonesPulsados, float delta_time, bool& pausa) {

    if (std::abs(pauseState.targetMenuY - pauseState.menuY) > 0.5f) {
        pauseState.menuY = LerpSimple(pauseState.menuY, pauseState.targetMenuY, delta_time * 10.0f);
    } else {
        pauseState.menuY = pauseState.targetMenuY;
    }


    if (pauseState.targetMenuY > 0.0f) {
        return; 
    }

    // Navegación por el menú (Arriba / Abajo)
    if (botonesPulsados & HidNpadButton_Up) {
        if (pauseState.selectedIndex > 0) pauseState.selectedIndex--;
    }
    if (botonesPulsados & HidNpadButton_Down) {
        if ((size_t)pauseState.selectedIndex < pauseState.buttons.size() - 1) pauseState.selectedIndex++;
    }


 


    // Actualizar el Lerp de todos los botones
    for (size_t i = 0; i < pauseState.buttons.size(); i++) {
        PauseButton& btn = pauseState.buttons[i];

        // Si es el botón seleccionado, el targetX se mueve 20 píxeles a la derecha
        if (i == pauseState.selectedIndex) {
            btn.targetX = btn.baseX + 20.0f;
            pauseState.arraySeleccionados[i] = true;
        } else {
            // Si no, vuelve a su posición original
            btn.targetX = btn.baseX;
            pauseState.arraySeleccionados[i] = false;
        }

        // Ejecutamos el Lerp para que el movimiento sea suave
        if (std::abs(btn.targetX - btn.posX) > 0.5f) {
            btn.posX = LerpSimple(btn.posX, btn.targetX, delta_time * 15.0f);
        } else {
            btn.posX = btn.targetX;
        }

    }

    if (botonesPulsados & HidNpadButton_A) {
        
        switch (pauseState.selectedIndex) {
            case 0:
                pausa = !pausa;
                pauseState.targetMenuY = 1500.0f; 

                botonesPulsados &= ~HidNpadButton_A;
                break;

            case 1: 
                break;

            case 2: 
                break;

            case 3: 
                SDL_Quit();
                exit(0);
                break;
        }
    }

    
}

void RenderPauseMenu(const PauseMenuState& pauseState, const glm::mat4& projection, glm::mat4 model, ImageData img) {
    if (pauseState.menuY >= 1000.0f) return;

    DibujarImagen(pauseState.difuminado, projection, model, 1920.0f /2.0f, 1080.0f / 2);
    DibujarImagen(pauseState.background, projection, model, pauseState.background.posX, pauseState.background.posY + pauseState.menuY);

    for (const PauseButton& btn : pauseState.buttons) {
        DrawAtlasSprite(pauseState.uiAtlas, btn.spriteID, projection, btn.posX, btn.posY + pauseState.menuY);
    }

}


void RenderPauseMenuDescriptions(const PauseMenuState& pauseState, const glm::mat4& projection, glm::mat4 model, bool Seleccionado[], int size, bool dibujar) {
    //if (!Seleccionado) return;
    if(dibujar){return;}

    for (int i = 0; i < size; i++){
        if (Seleccionado[i]){
            DibujarImagen(pauseState.background, projection, model, 400.0f + 70.0f * i, 790.0f);
    }
}


}