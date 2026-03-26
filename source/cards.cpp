#include <Graphics/cards.h>
#include <Graphics/shaders.h>
#include <Graphics/textures.h>
#include <Utils/logs.h>
#include <Utils/math.h>

#include <SDL.h>
#include <GLES3/gl3.h>
#include <glad/glad.h>
#include <stdio.h>
#include <unistd.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <algorithm>
#include <vector>

static std::vector<Card> s_cardReferences;
static std::vector<GLuint> s_unusedCardIDs;

static GLuint s_cardBaseShader;
static GLuint s_cardBaseVAO;
static GLuint s_cardBaseVBO;

static GLuint s_textureCardFront;
static GLuint s_textureCardBack;
static GLuint s_textureAtlas;

static GLint s_transformLoc = -1;
static GLint s_uvOffsetLoc  = -1;
static GLint s_uvScaleLoc   = -1;
static GLint s_baseCardSamplerLoc = -1;
static GLint s_atlasSamplerLoc = -1;
static GLint s_visibleLoc = -1;

static glm::mat4 s_transformMatrix;


GLint LoadBaseCardBuffers()
{

  // Generate buffers
  glGenVertexArrays(1, &s_cardBaseVAO);
  glGenBuffers(1, &s_cardBaseVBO);

  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  glBindVertexArray(s_cardBaseVAO);
  glBindBuffer(GL_ARRAY_BUFFER, s_cardBaseVBO);
  
  // Set vertex buffer data (VBO)
  glBufferData(GL_ARRAY_BUFFER, sizeof(CARD_VERTICES), CARD_VERTICES, GL_DYNAMIC_DRAW);

  // Set Attributes (VAO)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture));
  glEnableVertexAttribArray(1);

  // Unbind the VAO and VBO so it doesn't interfere with other draw calls.
  // For what I understand, this is not strictly necessary because when 
  // another call is going to be called, a call to glBindBuffer and 
  // glBindVertexArray is usually made before the actual call 
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return 0;
}


GLint LoadBaseCardShader()
{
    
    s_cardBaseShader = compileShaderProgram("romfs:/data/shaders/cardBase.vert", "romfs:/data/shaders/cardBase.frag");
    if(s_cardBaseShader == 0)
    {
        TRACE("Couldnt load shader program with error %d", s_cardBaseShader);
        return -1;
    }

    return 0;
}

GLint LoadBaseCardTextures()
{
    s_textureCardFront = loadTexture("romfs:/data/textures/Card_Front.png");
    if (!s_textureCardFront)
    {
        TRACE("Couldn't load the texture");
    }

   s_textureCardBack = loadTexture("romfs:/data/textures/Card_Back.png");
    if (!s_textureCardBack)
    {
        TRACE("Couldn't load the texture");
    }
    s_textureAtlas = loadTexture("romfs:/data/textures/Deck.png");
    if (!s_textureAtlas)
    {
        TRACE("Couldn't load the texture atlas");
    }

    return 0;
}

GLuint CreateBaseCard(Card_House house, int level)
{
    GLuint ID;
    // if the ID is in the unused cards, then we can reuse that data structure
    if (s_unusedCardIDs.size() != 0) {
        TRACE("RECICLING CARD ID");
        ID = s_unusedCardIDs[s_unusedCardIDs.size()-1]; // Get the top of the unused IDs
        s_cardReferences[ID-1].ID = ID;
        s_cardReferences[ID-1].level = level;
        s_cardReferences[ID-1].house = house;
        
        s_cardReferences[ID-1].position[0] = 0.0f;
        s_cardReferences[ID-1].position[1] = 0.0f;
        s_cardReferences[ID-1].position[2] = 0.0f;

        s_cardReferences[ID-1].scale[0] = CARD_SIZE;
        s_cardReferences[ID-1].scale[1] = CARD_SIZE * CARD_RATIO;
        s_cardReferences[ID-1].scale[2] = 1.0f;

        s_cardReferences[ID-1].angle = 0.0f;

        s_cardReferences[ID-1].type = BASIC;
        s_cardReferences[ID-1].selected = false;

        s_unusedCardIDs.pop_back();
        TRACE("card RECICLED with ID: %d, Level: %d", ID, level);
    }
    else{
        Card newCard;
        ID = s_cardReferences.size()+1;
        newCard.ID = ID; // IDs: [1:n]
        newCard.level = level;
        newCard.house = house;
        newCard.position[0] = 0.0f;
        newCard.position[1] = 0.0f;
        newCard.position[2] = 0.0f;
        
        newCard.scale[0] = CARD_SIZE;
        newCard.scale[1] = CARD_SIZE * CARD_RATIO;
        newCard.scale[2] = 1.0f;

        newCard.angle = 0.0f;
        
        newCard.type = BASIC;
        newCard.selected = false;
        s_cardReferences.push_back(newCard);
        TRACE("newCard created with ID: %d, Level: %d", s_cardReferences[s_cardReferences.size()-1].ID, s_cardReferences[s_cardReferences.size()-1].level);
    }
    return ID;
}

GLint DestroyCard(GLuint CardID)
{
    if (s_unusedCardIDs.size() != 0)
    {
        // The CardID was out of bounds
        //if(CardID <= 0 || CardID > s_cardReferences.size() + 1) { TRACE("CARD NOT DESTROYED out of bounds"); return -1; }

        // Search if the ID is being used, if not then is not necesary to destroy it
        // so we can just return
        for(uint8_t i = 0; i <= s_unusedCardIDs.size() - 1; i++)
        {
            if (CardID == s_unusedCardIDs[i]) { TRACE("Card not destroyed because its already unused. ID: %d", CardID);return 0;}
        }

    }
    
    // we add the ID to the unused list  
    s_unusedCardIDs.push_back(CardID);

    TRACE( "Card Destroyed with ID %d", CardID);
    return 0;
 
}

GLuint RetrieveBaseCardShader()
{   
    return s_cardBaseShader;
}

void RetrieveUniform(GLint &uniformLoc, GLint shaderProgram,const char *uniformName)
{
    //TRACE("RETRIEVE UNIFORM %s", uniformName);
    if(uniformLoc == -1) { uniformLoc = glGetUniformLocation(shaderProgram, uniformName);    TRACE ("RETRIEVED uniform <%s> with id %d", uniformName, uniformLoc);}
}
 
Card &RetrieveCardReference(GLuint CardID, uint8_t &result)
{  
    if (CardID <= 0 || (uint8_t) CardID > s_cardReferences.size()) { result = 0; }
    else { result = 1; }
    // it will allways be true that the ID is 1 unit more than its position in the vector 
    return s_cardReferences[CardID-1];
}   
 
GLint UpdateCardPosition(GLuint CardID, float (&position)[3])
{
    uint8_t result;
    Card& cardReference = RetrieveCardReference(CardID, result);
    if (!result) { 
        TRACE("Couldn't get card reference. ID: %d", CardID);
        return -1;
    }
    cardReference.position[0] = position[0];
    cardReference.position[1] = position[1];
    cardReference.position[2] = position[2];
     
    return 0;
} 
  
void DrawCard(GLuint CardID)
{
    Card cardToRender;
    uint8_t result; 

    cardToRender = RetrieveCardReference(CardID, result);
    if (!result){ TRACE("Couldn't get card Reference. ID: %d", CardID);}

    switch(cardToRender.type)
    {
        case BASIC:
        {
            glUseProgram(s_cardBaseShader);

            // Get uniform location
            RetrieveUniform(s_transformLoc, s_cardBaseShader, "transform");
            RetrieveUniform(s_uvOffsetLoc, s_cardBaseShader, "uv_offset");
            RetrieveUniform(s_uvScaleLoc, s_cardBaseShader, "uv_scale");
            RetrieveUniform(s_baseCardSamplerLoc, s_cardBaseShader, "BaseTexture");
            RetrieveUniform(s_atlasSamplerLoc, s_cardBaseShader, "AtlasTexture");
            RetrieveUniform(s_visibleLoc, s_cardBaseShader, "visible");

            break;
        }
        case CRISTAL:
        {
            break;
        }
    }

    glBindVertexArray(s_cardBaseVAO);
    glBindBuffer(GL_ARRAY_BUFFER, s_cardBaseVBO);

    // UPDATE TRANSFORMATIONS
 
    s_transformMatrix = glm::mat4(1.0f);

    float clipPosition[3] = {0.0f,0.0f,0.0f};

    transformScreenToClipPos(cardToRender.position, clipPosition);
    
    s_transformMatrix = glm::translate(s_transformMatrix, glm::vec3(clipPosition[0],clipPosition[1],clipPosition[2]));
    s_transformMatrix = glm::scale(s_transformMatrix, glm:: vec3(cardToRender.scale[0],cardToRender.scale[1],cardToRender.scale[2]));
    s_transformMatrix = glm::rotate(s_transformMatrix,  glm::radians(cardToRender.angle), glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(s_transformLoc, 1, GL_FALSE, glm::value_ptr(s_transformMatrix));

    // TEXTURE SAMPLING

    glm::vec2 uv = glm::vec2(68.0f * (float) (cardToRender.level - 1) / 817.0f, 97.0f * (float) (cardToRender.house) / 389.0f);
    glm::vec2 scale = glm::vec2(68.0f/817.0f, 98.0f/389.0f);
    float visible = 1.0f;

    glUniform2f(s_uvOffsetLoc, uv.x, uv.y);
    glUniform2f(s_uvScaleLoc, scale.x, scale.y);
    glUniform1f(s_visibleLoc, visible);
    glUniform1i(s_baseCardSamplerLoc, 0);
    glUniform1i(s_atlasSamplerLoc, 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, s_textureCardFront);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, s_textureAtlas);

    glDrawArrays(GL_TRIANGLES, 0, nVertices);
     
    s_transformMatrix = glm::rotate(s_transformMatrix,  glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(s_transformLoc, 1, GL_FALSE, glm::value_ptr(s_transformMatrix));
    
    uv = glm::vec2(0.0f,0.0f);
    scale = glm::vec2(1.0f,1.0f);
    visible = 0.0f;

    glUniform2f(s_uvOffsetLoc, uv.x, uv.y);
    glUniform2f(s_uvScaleLoc, scale.x, scale.y);
    glUniform1f(s_visibleLoc, visible);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, s_textureCardBack);

    glDrawArrays(GL_TRIANGLES, 0, nVertices);

}

GLint LoadDeck(std::vector<GLuint> &deck)
{

    for (int palo = BASTOS; palo <= OROS; palo++)
    {
        for (int level = 1; level <= 12; level++)
        {
            GLuint Carta = CreateBaseCard(static_cast<Card_House>(palo),level);

            float position[3] = {(DECK_POSITION_X) + (float) level, (DECK_POSITION_Y)  + (float) level, 0.0f};
            UpdateCardPosition(Carta, position);
            deck.push_back(Carta);
        }
    }
    return 0;
}


void RenderDeck(std::vector<GLuint> &deck)
{
    if(deck.size() == 0) {return;}
    for (size_t i = 0; i <= deck.size()-1; i++)
    {
        float position[3] = {(DECK_POSITION_X) + (float) i/2, (DECK_POSITION_Y)  + (float) i/2, 0.0f};
        UpdateCardPosition(deck[i], position);

        //TRACE ("Rendering Card with id %d", deck[i]);
        DrawCard(deck[i]);
    }
}

GLint UpdateHand(std::vector<GLuint> &hand, int cursorPosition)
{
    if (hand.size() == 0) {return 0;}
    for (size_t i = 0; i <= hand.size()-1; i++)
    {
        uint8_t result;
        Card &cardRef = RetrieveCardReference(hand[i], result);
        if (!result) { TRACE ("Couldn't retrieve Card with ID: %d", hand[i]);}

        // actualiza la posición para que esté boca arriba
        float lerpAngle;
        float destAngle = 180.0f;
        lerpf(cardRef.angle, destAngle, lerpAngle, 0.05f);

        // actualiza la posición y comprueba si está seleccionada
        float lerpPosition[3] = {0.0f,0.0f,0.0f};
        float destPosition[3];


        
        // si la carta está seleccionada, actualizar su posición 50 pixeles por encima de las demás
        if(!cardRef.selected)
        {
            destPosition[0] = (float) HAND_POSITION_X + (i * (CARD_W + (float) HAND_TOTAL_SIZE) / hand.size());
            destPosition[1] = (float) HAND_POSITION_Y;
            destPosition[2] = 0.0f;
        }
        else
        {
            destPosition[0] = (float) HAND_POSITION_X + (i * (CARD_W + (float) HAND_TOTAL_SIZE) / hand.size());
            destPosition[1] = (float) HAND_POSITION_Y + 50.0f;
            destPosition[2] = 0.0f;
        }


        // si la posición de la carta en la mano es igual a la posición del cursor
        // se aumenta la posicion y de la carta 30px
        if( i  == (size_t) cursorPosition)
        {
            destPosition[1] += 30.0f;
        }

        lerp(cardRef.position, destPosition, lerpPosition, 0.3f);

        cardRef.angle += lerpAngle;
        cardRef.position[0] = lerpPosition[0];
        cardRef.position[1] = lerpPosition[1];
        cardRef.position[2] = lerpPosition[2]; 

        //TRACE ("Card With ID %d updated in hand. New pos: x-> %f y-> %f z-> %f", cardRef.ID, cardRef.position[0], cardRef.position[1], cardRef.position[2]);
    } 
    return 0;
}

void RenderHand(std::vector<GLuint> &hand)
{

    if(hand.size() == 0) {return;}
    for (size_t i = 0; i <= hand.size()-1; i++)
    {
        //TRACE ("Rendering Card with id %d", hand[i]);
        DrawCard(hand[i]);
    }
}

GLint AddCardToHand(GLuint cardID, std::vector<GLuint> &hand,  std::vector<GLuint> &deck)
{
    bool found = false;
    for (size_t i = 0; i <= deck.size()-1 && !found; i++)
    {
        if(deck[i] == cardID)
        {
            deck[i] = deck.back(); // moves the last item to the position of the object we want to destroy
            deck.pop_back();
            found = true;
        }

    }
    if(!found){ TRACE( "Card not in deck. ID: %d", cardID); return -1; }

    TRACE("Adding Card to hand. ID: %d", cardID);
    hand.push_back(cardID);

    return 0;
}

GLint RemoveCardsFromHand(std::vector<GLuint> &hand)
{
    std::vector<GLuint> cardsToErase;
    // se recorre la mano de derecha a izquierda
    for(size_t i = 0 ; i <= hand.size() - 1; i++)
    {

        uint8_t result;
        Card &cardRef = RetrieveCardReference(hand[i], result);
        if (result == -1) {TRACE("Couldn't reference card with id %d", hand[i]); return -1;}
        
        TRACE("CARD FOUND ID %d", cardRef.ID);
        if(cardRef.selected)
        {
            cardsToErase.push_back(hand[i]);
        }


    }

    for (size_t i = 0; i <= cardsToErase.size() -1; i++)
    {
        TRACE("ERASING CARD WITH ID %d", cardsToErase[i]);
        hand.erase(find(hand.begin(),hand.end(),cardsToErase[i]));

    }


    TRACE("CARDS REMOVED");


    return 0;
}

