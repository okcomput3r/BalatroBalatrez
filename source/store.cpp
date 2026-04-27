#include <GLES3/gl3.h>
#include <glad/glad.h>
#include <Graphics/store.h>
#include <logic.h>

#include <cmath>
#include <SDL.h>
#include <stdlib.h>
#include <Utils/math.h>
#include <Graphics/jokers.h>
#include <iostream>
#include <random>


#include <vector>
#include <string>


JokerCard Joker1;
JokerCard Joker2;
JokerCard Joker3;

int numero1;
int numero2;
int numero3;

void InitJokersStore(StoreMenuState& storeState, int random1, int random2, int random3){
    numero1 = random1;
    numero2 = random2;
    numero3 = random3;

    storeState.jokers.clear(); 

    JokerCard j1, j2, j3;

    j1.spriteID = random1;
    j1.baseY = 500.0f; 
    j1.posX = j1.targetX = 740.0f;
    j1.posY = j1.targetY = j1.baseY;
    storeState.jokers.push_back(j1);

    j2.spriteID = random2;
    j2.baseY = 500.0f;
    j2.posX = j2.targetX = 1040.0f;
    j2.posY = j2.targetY = j2.baseY;
    storeState.jokers.push_back(j2);

    j3.spriteID = random3;
    j3.baseY = 500.0f; 
    j3.posX = j3.targetX = 1340.0f;
    j3.posY = j3.targetY = j3.baseY;
    storeState.jokers.push_back(j3);
    
    storeState.selectedIndex = 0;
    //storeState.menuY = 1500.0f;
    //storeState.targetMenuY = 1500.0f;

}

void InitStoreMenu(StoreMenuState& storeState, int random1, int random2, int random3){
    // Cargamos el atlas de los botones de pausa y las imágenes de fondo
    

    InitializeImage(storeState.background, 1110.0f, 800.0f, "fondoTienda.png");
    InitializeImage(storeState.comprar, 1920.0f /2.0f, 1080.0f / 2, "botonComprar.png");
    InitializeImage(storeState.atlasJokers, 0.0f, 0.0f, "Jokers.png");
    InitializeImage(storeState.descripcionJokers, 0.0f, 0.0f, "atlasDescripcionesCartas.png");


    InitializeAtlas(storeState.atlasJokers, 10, 16, 142.0f, 190.0f);
    InitializeAtlas(storeState.descripcionJokers, 5, 4, 180.0f, 180.0f);


    numero1 = random1;
    numero2 = random2;
    numero3 = random3;

    storeState.jokers.clear(); 

    JokerCard j1, j2, j3;

    j1.spriteID = random1;
    j1.baseY = 500.0f; 
    j1.posX = j1.targetX = 740.0f;
    j1.posY = j1.targetY = j1.baseY;
    storeState.jokers.push_back(j1);

    j2.spriteID = random2;
    j2.baseY = 500.0f;
    j2.posX = j2.targetX = 1040.0f;
    j2.posY = j2.targetY = j2.baseY;
    storeState.jokers.push_back(j2);

    j3.spriteID = random3;
    j3.baseY = 500.0f; 
    j3.posX = j3.targetX = 1340.0f;
    j3.posY = j3.targetY = j3.baseY;
    storeState.jokers.push_back(j3);
    
    storeState.selectedIndex = 0;
    storeState.menuY = 1500.0f;
    storeState.targetMenuY = 1500.0f;
    

    



    // DefineAtlasSprite(storeState.atlasJokers, jokerDatabase[random1].atlasIndex, 0.0f, 3040.0f - 190.0f, 142.0f, 190.0f);
    // DefineAtlasSprite(storeState.atlasJokers, jokerDatabase[random2].atlasIndex, 142.0f, 3040.0f - 190.0f, 142.0f, 190.0f);
    // DefineAtlasSprite(storeState.atlasJokers, jokerDatabase[random3].atlasIndex, 284.0f, 3040.0f - 190.0f, 142.0f, 190.0f);



    // DefineAtlasSprite(storeState.descripcionJokers, 0, 0.0f, 0.0f, 180.0f, 180.0f);
    // DefineAtlasSprite(storeState.descripcionJokers, 1, 180.0f, 0.0f, 180.0f, 180.0f);
    // DefineAtlasSprite(storeState.descripcionJokers, 2, 180.0f * 2, 0.0f, 180.0f, 180.0f);



    
    
}

void UpdateStoreMenu(StoreMenuState& storeState, u64 botonesPulsados, float delta_time, bool& pausa, int random1, int random2, int random3) {

    if (storeState.jokers.size() >= 3) {
        storeState.jokers[0].spriteID = random1;
        storeState.jokers[1].spriteID = random2;
        storeState.jokers[2].spriteID = random3;
    }

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

        if (!storeState.jokers.empty()) {
            
            if (estadoPartida.dinero >= 3 && ownedJokers.size() < 5) {
                
                // 1. Obtenemos el ID de la carta que estamos seleccionando
                int idComprado = storeState.jokers[storeState.selectedIndex].spriteID;

                // 2. Añadimos la carta al jugador y restamos el dinero
                ownedJokers.push_back(jokerDatabase[idComprado]);
                estadoPartida.dinero -= 3; 

                // 3. Borramos la carta de la lista de la tienda
                storeState.jokers.erase(storeState.jokers.begin() + storeState.selectedIndex);
                storeState.arraySeleccionados[storeState.selectedIndex] = false;

                // 4. Ajustamos el índice por si compramos la última carta de la derecha
                // para evitar que selectedIndex apunte a un lugar vacío (Out of bounds)
                if (storeState.selectedIndex >= storeState.jokers.size() && storeState.selectedIndex > 0) {
                    storeState.selectedIndex--;
                }
            }
        }
    }

    
}

void RenderStoreMenu(const StoreMenuState& storeState, const glm::mat4& projection, glm::mat4 model, ImageData img) {
    if (storeState.menuY >= 1000.0f) return;


    DibujarImagen(storeState.background, projection, model, storeState.background.posX, storeState.background.posY + storeState.menuY);


    for (int  i = 0; i < storeState.jokers.size(); i++) {
        DrawAtlasSprite(storeState.atlasJokers, jokerDatabase[storeState.jokers[i].spriteID].atlasIndex, projection, storeState.jokers[i].posX, storeState.jokers[i].posY + storeState.menuY);
    }

}


void RenderStoreMenuDescriptions(const StoreMenuState& storeState, const glm::mat4& projection, glm::mat4 model, bool Seleccionado[], int size, bool dibujar) {
    //if (!Seleccionado) return;
    if(dibujar){return;}

    for (int i = 0; i < size; i++){
        if (Seleccionado[i]){
            DibujarImagen(storeState.comprar, projection, model, storeState.jokers[i].posX + 180.0f, 550.0f + storeState.menuY);
            DrawAtlasSprite(storeState.descripcionJokers, storeState.jokers[i].spriteID, projection, storeState.jokers[i].posX -20.0f, 700.0f + storeState.menuY);
    }
}


}