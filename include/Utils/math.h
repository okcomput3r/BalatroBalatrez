#pragma once

void transformScreenToClipPos(float (&screenPos)[3], float(&clipPos)[3]);

void lerp(float (&currentPos)[3], float (&endPos)[3], float (&distance)[3], float t);

void lerpf(float &currentAngle, float &endAngle, float &angle, float t);

float LerpSimple(float start, float end, float t);