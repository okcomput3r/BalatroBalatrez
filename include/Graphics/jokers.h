#pragma once

#include <GLES3/gl3.h>
#include <vector>
#include <string>

// QUÉ hace el Joker
enum JokerAction {
    ACTION_ADD_MULT,
    ACTION_ADD_CHIPS,
    // ACTION_MULT_MULT,
    ACTION_ECONOMY
};

// CUÁNDO lo hace
enum JokerTrigger {
    TRIGGER_ALWAYS,       // Se aplica siempre
    TRIGGER_ON_HAND,      // Depende de la combinación jugada
    TRIGGER_ON_SUIT,      // Depende del palo 
    TRIGGER_ON_FACE_CARD, // Se aplica a sotas, caballos y reyes
    TRIGGER_PER_DISCARD,  // Multiplica por descartes restantes
    TRIGGER_PER_JOKER,    // Multiplica por Jokers en posesión
    TRIGGER_MAX_CARDS,    // Se activa si se juegan X o menos cartas (Half Joker)
    TRIGGER_END_ROUND,     // Efectos de economía al superar la ciega (Huevo)
    TRIGGER_ON_ACQUIRE    // Se activa al comprar el Joker en la tienda(credit card)
};



typedef struct Joker 
{
    int id;
    int atlasIndex; // Índice para saber qué sprite del atlas usar
    std::string name;
    
    JokerAction action;    // Qué hace
    float effectValue;     // Cuánto suma/multiplica
    
    JokerTrigger trigger;  // Cuándo lo hace
    
    // Variables para guardar la condición exacta
    std::string conditionString; // Guarda cosas como "Pareja" o "Color"
    int conditionInt;            // Guarda el ID del palo
    
};

extern std::vector<Joker> ownedJokers;
extern std::vector<Joker> jokerDatabase; // Base de datos de jokers disponibles en el juego, con sus efectos y características

