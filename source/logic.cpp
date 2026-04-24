#include <logic.h>
#include <Graphics/cards.h>
#include <Graphics/jokers.h> 
#include <Utils/logs.h>

EstadoPartida estadoPartida;

void IniciarNuevaPartida() {
    estadoPartida.faseActual = PHASE_BLIND_SELECT;
    estadoPartida.dinero = 4;
    estadoPartida.puntuacionGlobal = 0;
    estadoPartida.ronda = 1;
    ownedJokers.clear();
}

void ConfigurarCiega(float objetivo) {
    estadoPartida.ciegaObjetivo = objetivo;
    estadoPartida.puntuacionGlobal = 0;
    estadoPartida.manos = 4;      
    estadoPartida.descartes = 3;   
    estadoPartida.faseActual = PHASE_PLAYING;
}

void ObtenerValoresBaseMano(std::string nombreMano, float &baseChips, float &baseMult) {
    if (nombreMano == "Escalera de Color") { baseChips = 100; baseMult = 8; }
    else if (nombreMano == "Poker") { baseChips = 60; baseMult = 7; }
    else if (nombreMano == "Full House") { baseChips = 40; baseMult = 4; }
    else if (nombreMano == "Color") { baseChips = 35; baseMult = 4; }
    else if (nombreMano == "Escalera") { baseChips = 30; baseMult = 4; }
    else if (nombreMano == "Trio") { baseChips = 30; baseMult = 3; }
    else if (nombreMano == "Doble Pareja") { baseChips = 20; baseMult = 2; }
    else if (nombreMano == "Pareja") { baseChips = 10; baseMult = 2; }
    else { baseChips = 5; baseMult = 1; } // Carta Alta
}

void CalcularPuntuacionPrevia(std::string nombreMano, const std::vector<unsigned int>& cartasSeleccionadas) {
    if (cartasSeleccionadas.empty()) {
        estadoPartida.fichas = 0;
        estadoPartida.mult = 0;
        return;
    }

    float chips = 0;
    float mult = 0;

    ObtenerValoresBaseMano(nombreMano, chips, mult);

    // =========================================================
    // PASADA 1: Valor numérico y Jokers que afectan CARTA a CARTA
    // =========================================================
    for (unsigned int cardId : cartasSeleccionadas) {
        uint8_t result;
        Card &c = RetrieveCardReference(cardId, result);
        
        if (result) {
            float valorCarta = 0;
            if (c.level >= 2 && c.level <= 12) {
                valorCarta = c.level;
            } else if (c.level == 1) { 
                valorCarta = 13; // El As vale 13
            }
            chips += valorCarta;

            // Comprobamos Jokers para ESTA carta en concreto
            for (Joker& joker : ownedJokers) {
                bool seActivaCarta = false;

                if (joker.trigger == TRIGGER_ON_SUIT && c.house == joker.conditionInt) {
                    seActivaCarta = true;
                } 
                else if (joker.trigger == TRIGGER_ON_FACE_CARD && c.level >= 10 && c.level <= 12) {
                    seActivaCarta = true;
                }

                if (seActivaCarta) {
                    if (joker.action == ACTION_ADD_MULT) mult += joker.effectValue;
                    else if (joker.action == ACTION_ADD_CHIPS) chips += joker.effectValue;
                }
            }
        }
    }

    // =========================================================
    // PASADA 2: Jokers que se activan POR LA MANO o GLOBALES
    // =========================================================
    for (Joker& joker : ownedJokers) {
        if (joker.action == ACTION_ECONOMY) continue;

        bool seActivaGlobal = false;
        float multiplicadorDinamico = 1.0f; 

        if (joker.trigger == TRIGGER_ALWAYS) {
            seActivaGlobal = true;
        } 
        else if (joker.trigger == TRIGGER_ON_HAND) {
            if (nombreMano.find(joker.conditionString) != std::string::npos ||
               (nombreMano == "Full House" && joker.conditionString == "Pareja") ||
               (nombreMano == "Full House" && joker.conditionString == "Trio")) {
                seActivaGlobal = true;
            }
        }
        else if (joker.trigger == TRIGGER_MAX_CARDS) {
            if (cartasSeleccionadas.size() <= (size_t)joker.conditionInt) seActivaGlobal = true;
        }
        else if (joker.trigger == TRIGGER_PER_DISCARD) {
            seActivaGlobal = true;
            multiplicadorDinamico = (float)estadoPartida.descartes;
        }
        else if (joker.trigger == TRIGGER_PER_JOKER) {
            seActivaGlobal = true;
            multiplicadorDinamico = (float)ownedJokers.size();
        }

        if (seActivaGlobal) {
            float valorReal = joker.effectValue * multiplicadorDinamico;
            
            if (joker.action == ACTION_ADD_MULT) mult += valorReal;
            else if (joker.action == ACTION_ADD_CHIPS) chips += valorReal;
        }
    }

    // Actualizar para que se muestre en la interfaz
    estadoPartida.fichas = chips;
    estadoPartida.mult = mult;
}

void JugarMano(std::string nombreMano, const std::vector<unsigned int>& cartasSeleccionadas) {
    if (estadoPartida.manos <= 0) return; // No te quedan manos
    
    estadoPartida.manos--;
    estadoPartida.faseActual = PHASE_CALCULATING; // Bloquea los controles mientras calcula

    CalcularPuntuacionPrevia(nombreMano, cartasSeleccionadas);
    
    // Multiplicamos Fichas x Mult y lo sumamos al total
    float puntuacionMano = estadoPartida.fichas * estadoPartida.mult;
    estadoPartida.puntuacionGlobal += puntuacionMano;

    TRACE("Mano jugada: %s | Puntos obtenidos: %f | Total: %f / %f", 
          nombreMano.c_str(), puntuacionMano, estadoPartida.puntuacionGlobal, estadoPartida.ciegaObjetivo);

    // Comprobar si hemos ganado o perdido
    if (estadoPartida.puntuacionGlobal >= estadoPartida.ciegaObjetivo) {
        TRACE("¡CIEGA SUPERADA!");
        estadoPartida.faseActual = PHASE_SHOP;
    } else if (estadoPartida.manos == 0) {
        TRACE("GAME OVER");
        estadoPartida.faseActual = PHASE_GAME_OVER;
    } else {
        estadoPartida.faseActual = PHASE_PLAYING;
    }
}

void DescartarCartas() {
    if (estadoPartida.descartes > 0) {
        estadoPartida.descartes--;
    }
}

void AvanzarSiguienteCiega() {
    estadoPartida.ronda++;
    float nuevoObjetivo = estadoPartida.ciegaObjetivo * 1.5f; 
    estadoPartida.dinero += 5; 
    for (Joker& joker : ownedJokers) {
        if (joker.action == ACTION_ECONOMY && joker.trigger == TRIGGER_END_ROUND) {
            estadoPartida.dinero += (int)joker.effectValue;
            TRACE("El Joker %s te ha dado %d monedas extra.", joker.name.c_str(), (int)joker.effectValue);
        }
    }
    ConfigurarCiega(nuevoObjetivo);
}