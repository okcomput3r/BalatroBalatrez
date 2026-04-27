#include <Graphics/store.h>
#include <cmath>
#include <SDL.h>
#include <stdlib.h>
#include <Utils/math.h>




void InitStoreMenu(StoreMenuState& storeState) {
    // Cargamos el atlas de los botones de pausa y las imágenes de fondo
    InitializeImage(storeState.background, 1110.0f, 800.0f, "fondoTienda.png");
    InitializeImage(storeState.comprar, 1920.0f /2.0f, 1080.0f / 2, "botonComprar.png");
    InitializeImage(storeState.atlasJokers, 0.0f, 0.0f, "Jokers.png");
    InitializeImage(storeState.descripcionJokers, 0.0f, 0.0f, "atlasDescripcionesCartas.png");


    InitializeAtlas(storeState.atlasJokers, 10, 16, 142.0f, 190.0f);
    InitializeAtlas(storeState.descripcionJokers, 5, 2, 180.0f, 180.0f);



    DefineAtlasSprite(storeState.atlasJokers, 0, 0.0f, 3040.0f - 190.0f, 142.0f, 190.0f);
    DefineAtlasSprite(storeState.atlasJokers, 1, 142.0f, 3040.0f - 190.0f, 142.0f, 190.0f);
    DefineAtlasSprite(storeState.atlasJokers, 2, 284.0f, 3040.0f - 190.0f, 142.0f, 190.0f);



    DefineAtlasSprite(storeState.descripcionJokers, 0, 0.0f, 0.0f, 180.0f, 180.0f);
    DefineAtlasSprite(storeState.descripcionJokers, 1, 180.0f, 0.0f, 180.0f, 180.0f);
    DefineAtlasSprite(storeState.descripcionJokers, 2, 180.0f * 2, 0.0f, 180.0f, 180.0f);


    JokerCard Joker1;
    Joker1.spriteID = 0;
    Joker1.baseY = 500.0f; 
    Joker1.posX = Joker1.targetX = 740.0f;
    Joker1.posY = Joker1.targetY = Joker1.baseY;

    JokerCard Joker2;
    Joker2.spriteID = 1;
    Joker2.baseY = 500.0f;
    Joker2.posX = Joker2.targetX = 1040.0f;
    Joker2.posY = Joker2.targetY = Joker2.baseY;

    JokerCard Joker3;
    Joker3.spriteID = 2;
    Joker3.baseY = 500.0f; 
    Joker3.posX = Joker3.targetX = 1340.0f;
    Joker3.posY = Joker3.targetY = Joker3.baseY;
    

    storeState.jokers.push_back(Joker1);
    storeState.jokers.push_back(Joker2);
    storeState.jokers.push_back(Joker3);
    
    
    storeState.selectedIndex = 0;

    storeState.menuY = 1500.0f;
    storeState.targetMenuY = 1500.0f;
}

void UpdateStoreMenu(StoreMenuState& storeState, u64 botonesPulsados, float delta_time, bool& pausa) {

    if (std::abs(storeState.targetMenuY - storeState.menuY) > 0.5f) {
        storeState.menuY = LerpSimple(storeState.menuY, storeState.targetMenuY, delta_time * 5.0f);
    } else {
        storeState.menuY = storeState.targetMenuY;
    }


    if (storeState.targetMenuY > 0.0f) {
        return; 
    }

    // Navegación por el menú (Arriba / Abajo)
    if (botonesPulsados & HidNpadButton_Left) {
        if (storeState.selectedIndex > 0) storeState.selectedIndex--;
    }
    if (botonesPulsados & HidNpadButton_Right) {
        if ((size_t)storeState.selectedIndex < storeState.jokers.size() - 1) storeState.selectedIndex++;
    }


 


    // Actualizar el Lerp de todos los botones
    for (size_t i = 0; i < storeState.jokers.size(); i++) {
        JokerCard& btn = storeState.jokers[i];

        // Si es el botón seleccionado, el targetY se mueve 20 píxeles a la derecha
        if (i == storeState.selectedIndex) {
            btn.targetY = btn.baseY - 25.0f;
            storeState.arraySeleccionados[i] = true;
        } else {
            // Si no, vuelve a su posición original
            btn.targetY = btn.baseY;
            storeState.arraySeleccionados[i] = false;
        }

        // Ejecutamos el Lerp para que el movimiento sea suave
        if (std::abs(btn.targetY - btn.posY) > 0.5f) {
            btn.posY = LerpSimple(btn.posY, btn.targetY, delta_time * 15.0f);
        } else {
            btn.posY = btn.targetY;
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

    DibujarImagen(storeState.background, projection, model, storeState.background.posX, storeState.background.posY + storeState.menuY);

    for (const JokerCard& btn : storeState.jokers) {
        DrawAtlasSprite(storeState.atlasJokers, btn.spriteID, projection, btn.posX, btn.posY + storeState.menuY);
    }

}


void RenderStoreMenuDescriptions(const StoreMenuState& storeState, const glm::mat4& projection, glm::mat4 model, bool Seleccionado[], int size, bool dibujar) {
    //if (!Seleccionado) return;
    if(dibujar){return;}

    for (int i = 0; i < size; i++){
        if (Seleccionado[i]){
            DibujarImagen(storeState.comprar, projection, model, 920.0f + 300.0f * i, 550.0f + storeState.menuY);
            DrawAtlasSprite(storeState.descripcionJokers, i, projection, 720.0f + 300.0f * i, 700.0f + storeState.menuY);
    }
}


}