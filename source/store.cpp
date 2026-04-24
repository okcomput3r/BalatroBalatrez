#include <Graphics/store.h>
#include <cmath>
#include <SDL.h>
#include <stdlib.h>
#include <Utils/math.h>




void InitStoreMenu(StoreMenuState& storeState) {
    // Cargamos el atlas de los botones de pausa y las imágenes de fondo
    InitializeImage(storeState.background, 1920.0f /2.0f, 1080.0f / 2, "fondoPausa.png");
    InitializeImage(storeState.difuminado, 1920.0f /2.0f, 1080.0f / 2, "difuminadoNegro.png");

    InitializeImage(storeState.uiAtlas, 0.0f, 0.0f, "botonesPausa.png");
    InitializeAtlas(storeState.uiAtlas, 1, 4, 339.0f, 54.0f);

    // 2. Definimos los recortes (Fondo y botones)
    //DefineAtlasSprite(storeState.uiAtlas, SPRITE_PAUSE_BG, 0.0f, 0.0f, 1920.0f, 1080.0f); // Fondo oscuro semi-transparente
    DefineAtlasSprite(storeState.uiAtlas, 0, 0.0f, 0.0f, 339.0f, 54.0f);
    DefineAtlasSprite(storeState.uiAtlas, 1, 0.0f, 54.0f, 339.0f, 54.0f);
    DefineAtlasSprite(storeState.uiAtlas, 2, 0.0f, 108.0f, 339.0f, 54.0f);
    DefineAtlasSprite(storeState.uiAtlas, 3, 0.0f, 162.0f, 339.0f, 54.0f);

    //Inicializamos el fondo

    // 3. Creamos los botones matemáticos
    JokerCard Joker1;
    Joker1.spriteID = 3;
    Joker1.baseX = 790.0f; 
    Joker1.posX = Joker1.targetX = Joker1.baseX;
    Joker1.posY = Joker1.targetY = 400.0f;

    JokerCard Joker2;
    Joker2.spriteID = 2;
    Joker2.baseX = 790.0f; 
    Joker2.posX = Joker2.targetX = Joker2.baseX;
    Joker2.posY = Joker2.targetY = 470.0f;

    JokerCard Joker3;
    Joker3.spriteID = 1;
    Joker3.baseX = 790.0f; 
    Joker3.posX = Joker3.targetX = Joker3.baseX;
    Joker3.posY = Joker3.targetY = 540.0f;

    

    storeState.jokers.push_back(Joker1);
    storeState.jokers.push_back(Joker2);
    storeState.jokers.push_back(Joker3);
    
    
    storeState.selectedIndex = 0;

    storeState.menuY = 1500.0f;
    storeState.targetMenuY = 1500.0f;
}

void UpdateStoreMenu(StoreMenuState& storeState, u64 botonesPulsados, float delta_time, bool& pausa) {

    if (std::abs(storeState.targetMenuY - storeState.menuY) > 0.5f) {
        storeState.menuY = LerpSimple(storeState.menuY, storeState.targetMenuY, delta_time * 15.0f);
    } else {
        storeState.menuY = storeState.targetMenuY;
    }


    if (storeState.targetMenuY > 0.0f) {
        return; 
    }

    // Navegación por el menú (Arriba / Abajo)
    if (botonesPulsados & HidNpadButton_Up) {
        if (storeState.selectedIndex > 0) storeState.selectedIndex--;
    }
    if (botonesPulsados & HidNpadButton_Down) {
        if ((size_t)storeState.selectedIndex < storeState.jokers.size() - 1) storeState.selectedIndex++;
    }


 


    // Actualizar el Lerp de todos los botones
    for (size_t i = 0; i < storeState.jokers.size(); i++) {
        JokerCard& btn = storeState.jokers[i];

        // Si es el botón seleccionado, el targetX se mueve 20 píxeles a la derecha
        if (i == storeState.selectedIndex) {
            btn.targetX = btn.baseX + 20.0f;
            storeState.arraySeleccionados[i] = true;
        } else {
            // Si no, vuelve a su posición original
            btn.targetX = btn.baseX;
            storeState.arraySeleccionados[i] = false;
        }

        // Ejecutamos el Lerp para que el movimiento sea suave
        if (std::abs(btn.targetX - btn.posX) > 0.5f) {
            btn.posX = LerpSimple(btn.posX, btn.targetX, delta_time * 15.0f);
        } else {
            btn.posX = btn.targetX;
        }

    }

    if (botonesPulsados & HidNpadButton_A) {
        
        switch (storeState.selectedIndex) {
            case 0:
                break;
            case 1: 
                break;
            case 2: 
                break;
        }
    }

    
}

void RenderStoreMenu(const StoreMenuState& storeState, const glm::mat4& projection, glm::mat4 model, ImageData img) {
    if (storeState.menuY >= 1000.0f) return;

    DibujarImagen(storeState.difuminado, projection, model, 1920.0f /2.0f, 1080.0f / 2);
    DibujarImagen(storeState.background, projection, model, storeState.background.posX, storeState.background.posY + storeState.menuY);

    for (const JokerCard& btn : storeState.jokers) {
        DrawAtlasSprite(storeState.uiAtlas, btn.spriteID, projection, btn.posX, btn.posY + storeState.menuY);
    }

}


void RenderStoreMenuDescriptions(const StoreMenuState& storeState, const glm::mat4& projection, glm::mat4 model, bool Seleccionado[], int size, bool dibujar) {
    //if (!Seleccionado) return;
    if(dibujar){return;}

    for (int i = 0; i < size; i++){
        if (Seleccionado[i]){
            DibujarImagen(storeState.background, projection, model, 400.0f + 70.0f * i, 790.0f);
    }
}


}