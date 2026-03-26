#pragma once

#include <GLES3/gl3.h>
#include <vector>
#include <queue>


#define SCREEN_W 1920
#define SCREEN_H 1080

#define DECK_POSITION_X SCREEN_W - CARD_W - 80
#define DECK_POSITION_Y 180

#define HAND_TOTAL_SIZE 700

#define HAND_POSITION_X SCREEN_W/2 - HAND_TOTAL_SIZE/2
#define HAND_POSITION_Y 300



#define N_VERTICES 6
#define CARD_W 85
#define CARD_H 98
#define CARD_SIZE 0.25
#define CARD_RATIO CARD_H/CARD_W

typedef struct Vertex_s
{
    float position[3];
    float texture[2];
} Vertex;
#define nVertices sizeof(CARD_VERTICES)/N_VERTICES

static Vertex CARD_VERTICES[] =
{
    { { -0.25f, -0.5f, 0.0f }, { 1.0f, 1.0f } },
    { { -0.25f,  0.5f, 0.0f }, { 1.0f, 0.0f } },
    { {  0.25f,  0.5f, 0.0f }, { 0.0f, 0.0f } },
    { {  0.25f,  0.5f, 0.0f }, { 0.0f, 0.0f } },
    { {  0.25f, -0.5f, 0.0f }, { 0.0f, 1.0f } },
    { { -0.25f, -0.5f, 0.0f }, { 1.0f, 1.0f } },
};



enum Card_House
{
    BASTOS,
    ESPADAS,
    COPAS,
    OROS 
};

enum Card_States
{
    HOLOGRAM,
    NEGATIVE,
    //...
};

enum Card_Type
{
    BASIC,
    CRISTAL,
    //...
};

#define MAX_STATES 10
#define INITIAL_DECK_SIZE 48
#define INITIAL_HAND_SIZE 6



typedef struct s_deck{
    
    int maxSize; // maxSize will double every time it is exceded
    int nCards;
    std::vector<GLuint> cardIDs;

} Deck;

typedef struct s_hand{
    
    int maxSize; // maxSize will double every time it is exceded
    int size;
    std::vector<GLuint> cardIDs;

} Hand;

typedef struct s_card{

    float position[3]; // position in space
    float scale[3];
    float angle;
    
    Card_States state;
    Card_House house;
    Card_Type type;
  
    GLuint ID;    // unique identifier
    int level; // 1 - 12 

    bool selected;

} Card;


// LOADERS & GETTERS

/***
*
*   LoadBaseCardBuffers() carga en memoria las estructúras básicas que comparten
*   todas las cartas, es decir, el VBO que contiene la posición de los
*   vertices de la carta, y el VAO, que contiene la forma de como interpretar esa 
*   información.
*
*/
GLint LoadBaseCardBuffers();

/***
*
*   LoadBaseCardShader() carga en memoria el programa base que todas las 
*   cartas ejecutarán al inicializarse.
*
*/
GLint LoadBaseCardShader();
GLint LoadBaseCardTextures();

GLint DestroyCardShaders();
GLint DestroyCardTextures(); 

GLuint RetrieveBaseCardShader();

// CARD MANIPULATION METHODS

// There is no need to a destroyCard() method because all the cards will
// be uploaded in the buffer at gameTime.
// estoy delirando tio no se ni lo que acabo de decir. Si que hay un destroyCard soy tonto


/***
 *
 *  DrawCard() draws the card, given a CardID, onto the screen. 
 *  this function takes care of rotation, translation and scale of the card,
 *  the activation of shaders and textures, retrieving uniforms for shader
 *  manipulation, and fetching a drawCall.
 *
 * */
void DrawCard(GLuint CardID);


/***
 * 
 *  CreateBaseCard() allocates a new card int the card's array
 *  and returns the identifier of the card. This identifier can
 *  be used to retrieve a reference to the actual card if any 
 *  value has to be changed.
 *  
 */
GLuint CreateBaseCard(Card_House house, int level);

/***
 * 
 *  DestroyCard() sets the positiom [CardID] in the card's array  
 *  to not used and adds the identifiar to the unused card's array.
 *  When a new Card is going to be created, it will check first the
 *  unused array and if a match is found, it will use that block of memory
 *  instead of allocating more.
 *
 */
GLint DestroyCard(GLuint CardID);

/***
 *  
 *  RetrieveCardReference() returns the direction in memory of the card 
 *  specified by its cardID, if the card cannot be found, then "result"
 *  will be 0, otherwise it will be set to 1. 
 *
 */
Card &RetrieveCardReference(GLuint CardID, uint8_t &result);

/***
 *  
 * UpdateCardPosition() sets the new position of the card in screenSpace,
 * that is from (0,0) to (SCREEN_W, SCREEN_H), later in the DrawCard()
 * function it gets clipped to [-1,1]. 
 *
 */
GLint UpdateCardPosition(GLuint CardID, float (&position)[3]);


/***
 *
 * UpdateCardRotation() sets the new position (in degrees) of the card
 *
 */
GLint UpdateCardRotation(GLuint CardID, float rotation);



// DECK & HAND HELPER METHODS


/***
*
*   LoadDeck() initializes the deck by: 
*       - Adding all 48 cards 
*       - Updating their position to be slightly offset
*       - Setting them backwards
*
*/
GLint LoadDeck(std::vector<GLuint> &deck);

/***
*
*   UpdateHand() updates the hand by: 
*        - Adding n cards to the hand 
*        - Substracting them from the deck
*        - Lerping their position to somewhere in center of the screen
*        - Lerping their rotation to face upwards
*
*/
GLint UpdateHand(std::vector<GLuint> &hand, int cursorPosition);

/***
*
*   AddCardToDeck(GLuint, Hand&) adds the identifier of the card to the 
*   current deck.  
*
*/
GLint AddCardToDeck(GLuint cardID, std::vector<GLuint> &deck);

/***
*
*   AddCardToHand() adds the identifier of the card to the 
*   current playing hand and substracts it from the deck.  
*
*/
GLint AddCardToHand(GLuint cardID, std::vector<GLuint> &hand,  std::vector<GLuint> &deck);

GLint RemoveCardFromDeck(GLuint cardID, Deck &deck);

GLint RemoveCardsFromHand(std::vector<GLuint> &hand);

void RenderDeck(std::vector<GLuint> &deck);

void RenderHand(std::vector<GLuint> &hand);
