#include <Graphics/jokers.h>
#include <GLES3/gl3.h>
#include <vector>
#include <string>

std::vector<Joker> ownedJokers;
std::vector<Joker> jokerDatabase = {
    // {ID_Logico, ID_Atlas, Nombre, Efecto, Valor}
    {0, 0, "Joker", EFFECT_ADD_MULT, 4.0f},
    {1, 16, "Greedy Joker", EFFECT_ADD_MULT_OROS, 4.0f},
    {2, 17, "Lusty Joker", EFFECT_ADD_MULT_COPAS, 4.0f},
    {3, 18, "Wrathful Joker", EFFECT_ADD_MULT_ESPADAS, 4.0f},
    {4, 19, "Gluttonous Joker", EFFECT_ADD_MULT_BASTOS, 4.0f},
    {5, 2, "Jolly Joker", EFFECT_ADD_MULT_PAREJA, 8.0f},
    {6, 3, "Zany Joker", EFFECT_ADD_MULT_TRIO, 12.0f},
    {7, 4, "Mad Joker", EFFECT_ADD_MULT_POKER, 20.0f},
    {8, 5, "Crazy Joker", EFFECT_ADD_MULT_ESCALERA, 12.0f},
    {9, 6, "Droll Joker", EFFECT_ADD_MULT_COLOR, 12.0f},
    {10, 140, "Sly Joker", EFFECT_ADD_CHIPS_PAREJA, 50.0f},
    {11, 141, "Wily Joker", EFFECT_ADD_CHIPS_TRIO, 100.0f},
    {12, 142, "Clever Joker", EFFECT_ADD_CHIPS_POKER, 150.0f},
    {13, 143, "Devious Joker", EFFECT_ADD_CHIPS_ESCALERA, 100.0f},
    {14, 144, "Crafty Joker", EFFECT_ADD_CHIPS_COLOR, 80.0f},
    {15, 7, "Half Joker", EFFECT_ADD_MULT_HALF_DECK, 20.0f},
    {16, 15, "Credit Card", EFFECT_ECONOMY_DEBT, 20.0f}, // Usaremos el valor 20 para ampliar la deuda en la lógica
    {17, 21, "Banner", EFFECT_CHIPS_PER_DISCARD, 40.0f},
    {18, 26, "Misprint", EFFECT_RANDOM_MULT, 23.0f},     // Lógica: Rand de 0 a 23
    {19, 32, "Scary Face", EFFECT_CHIPS_FACE_CARDS, 30.0f},
    {20, 33, "Abstract Joker", EFFECT_MULT_PER_JOKER, 3.0f},
    {21, 100, "Huevo", EFFECT_ECONOMY_END_ROUND, 5.0f}
};
