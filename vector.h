// vector.h
// Or vector_math.h ?
// or linear_algebra?
#pragma once

typedef struct Vector2D {
	float x;
	float y;
} Vector2D;

float cjk_magnitude(Vector2D v);
Vector2D cjk_normalize(Vector2D v);
Vector2D cjk_multiply(Vector2D v, float m);
Vector2D cjk_add(Vector2D v1, Vector2D v2);
Vector2D cjk_subtract(Vector2D v1, Vector2D v2);
Vector2D cjk_reflect(Vector2D vector , Vector2D normal);
