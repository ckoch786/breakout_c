#include <stdio.h>
#include <math.h>
#include "vector.h"

// aka direction
float cjk_magnitude(Vector2D v)
{
	return sqrtf(v.x*v.x + v.y*v.y);
}

Vector2D cjk_normalize(Vector2D v)
{
	float m = cjk_magnitude(v);
	if (m > 0) {
		return (Vector2D){v.x/m, v.y/m};
	}

	return (Vector2D) {0.0f, 0.0f};
}

// TODO Should this be named scale instead and the float would be a scalar value?
// Also what is m? m is magnitude
Vector2D cjk_multiply(Vector2D v, float m)
{
	return (Vector2D) {v.x * m, v.y * m};
}

Vector2D cjk_add(Vector2D v1, Vector2D v2)
{
	return (Vector2D) {v1.x + v2.x, v1.y + v2.y};
}

Vector2D cjk_subtract(Vector2D v1, Vector2D v2)
{
	return (Vector2D) {v1.x - v2.x, v1.y - v2.y};
}

// TODO is there a way to handle naming collisions?
Vector2D cjk_reflect(Vector2D vector , Vector2D normal)
{
	printf("reflecting the vector onto the normal?\n");
	normal = cjk_normalize(normal);
	float dotProduct = vector.x * normal.x + vector.y + normal.y;

	// Apply the reflection formula: vector.- 2 * dot(v,n) * n
	return (Vector2D) {
		vector.x - 2.0f * dotProduct * normal.x,
		vector.y - 2.0f * dotProduct * normal.y,
	};
}


// TODO this does not seem to work with:
// $(CC) $(CFLAGS) -DRENAME_FUNC -c vector.c -o $(BUILD)/vector.o $(LIBS) 
#ifdef RENAME_FUNC
	#define reflect cjk_reflect
#endif
