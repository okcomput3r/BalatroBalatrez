#pragma once
#include <string>
#include <vector>

// Estados del juego
enum GamePhase {
    PHASE_BLIND_SELECT, // Pantalla para elegir la siguiente ciega
    PHASE_PLAYING,      // Durante la partida tirando cartas
    PHASE_CALCULATING,  // Animación de puntuación
    PHASE_SHOP,         // En la tienda
    PHASE_GAME_OVER     // Has perdido
};

// Estructura para mantener el estado de la partida
struct EstadoPartida {
    GamePhase faseActual;

    // Estadísticas de la ronda actual
    float ciegaObjetivo = 0.0f;
    float puntuacionGlobal = 0.0f;
    float fichas = 0.0f;
    float mult = 0.0f;

    // Recursos del jugador
    int dinero = 0;
    int manos = 0;
    int descartes = 0;

    int ronda = 1;
};

// Declaración de tu variable global
extern EstadoPartida estadoPartida;

// Funciones principales de la lógica
void IniciarNuevaPartida();
void ConfigurarCiega(float objetivo);
void CalcularPuntuacionPrevia(std::string nombreMano, const std::vector<unsigned int>& cartasSeleccionadas);
void JugarMano(std::string nombreMano, const std::vector<unsigned int>& cartasSeleccionadas);
void DescartarCartas();
void AvanzarSiguienteCiega();
