#include <Graphics/cards.h>
#include <Utils/math.h>

void transformScreenToClipPos(float (&screenPos)[3], float (&clipPos)[3])
{

    clipPos[0] = ((screenPos[0]/SCREEN_W) * 2) - 1.0f;
    clipPos[1] = ((screenPos[1]/SCREEN_H) * 2) - 1.0f;
}

void lerp(float (&currentPos)[3], float (&endPos)[3], float (&distance)[3], float t)
{
    distance[0] = currentPos[0] + (endPos[0] - currentPos[0]) * t;
    distance[1] = currentPos[1] + (endPos[1] - currentPos[1]) * t;
    distance[2] = currentPos[2] + (endPos[2] - currentPos[2]) * t;

}

void lerpf(float &currentAngle, float &endAngle, float &angle, float t)
{
    

    (currentAngle > endAngle) ? angle = (currentAngle - endAngle) * t : angle = (endAngle - currentAngle) * t;
}

float LerpSimple(float start, float end, float t) {
    return start + (end - start) * t;
}