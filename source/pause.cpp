#include <Graphics/pause.h>
#include <cmath>

// Definimos unos IDs ficticios para el ejemplo
#define SPRITE_PAUSE_BG 50
#define SPRITE_BTN_RESUME 51
#define SPRITE_BTN_QUIT 52

// Función de Lerp (puedes importarla de tu Utils si la tienes ahí)
static float LerpSimple(float start, float end, float t) {
    return start + (end - start) * t;
}

void InitPauseMenu(PauseMenuState& pauseState) {
    // 1. Cargamos el atlas de la interfaz de pausa
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

void UpdatePauseMenu(PauseMenuState& pauseState, u64 botonesPulsados, float delta_time) {

    if (std::abs(pauseState.targetMenuY - pauseState.menuY) > 0.5f) {
        pauseState.menuY = LerpSimple(pauseState.menuY, pauseState.targetMenuY, delta_time * 15.0f);
    } else {
        pauseState.menuY = pauseState.targetMenuY;
    }

    // 2. BLOQUEO DE INPUTS
    // Si el menú está bajado (o bajando), no hacemos caso a la cruceta
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

        // Efecto Hover: Si es el botón seleccionado, el targetX se mueve 40 píxeles a la derecha
        if (i == pauseState.selectedIndex) {
            btn.targetX = btn.baseX + 20.0f;
        } else {
            // Si no, vuelve a su posición original
            btn.targetX = btn.baseX;
        }

        // Ejecutamos el Lerp para que el movimiento sea suave
        if (std::abs(btn.targetX - btn.posX) > 0.5f) {
            btn.posX = LerpSimple(btn.posX, btn.targetX, delta_time * 15.0f);
        } else {
            btn.posX = btn.targetX;
        }
    }
}

void RenderPauseMenu(const PauseMenuState& pauseState, const glm::mat4& projection, glm::mat4 model, ImageData img) {
    // 1. Dibujar fondo (quizás ocupe toda la pantalla y oscurezca el juego)
    //DrawAtlasSprite(pauseState.uiAtlas, SPRITE_PAUSE_BG, projection, 0.0f, 0.0f);
    if (pauseState.menuY >= 1000.0f) return;

    // Dibujar fondo sumándole el menuY
    //DrawAtlasSprite(pauseState.uiAtlas, SPRITE_PAUSE_BG, projection, 0.0f, pauseState.menuY);
    DibujarImagen(pauseState.difuminado, projection, model, 1920.0f /2.0f, 1080.0f / 2);
    DibujarImagen(pauseState.background, projection, model, pauseState.background.posX, pauseState.background.posY + pauseState.menuY);

    // Dibujar los botones sumándoles también el menuY
    for (const PauseButton& btn : pauseState.buttons) {
        DrawAtlasSprite(pauseState.uiAtlas, btn.spriteID, projection, btn.posX, btn.posY + pauseState.menuY);
    }

}


// void RenderPauseMenuDescriptions(const PauseMenuState& pauseState, const glm::mat4& projection, glm::mat4 model, bool Seleccionado) {
//     if (!Seleccionado) return;

//     DibujarImagen()


// }