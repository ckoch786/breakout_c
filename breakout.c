#include <stdio.h>
#include <math.h>

#include <raylib.h>

const int SCREEN_SIZE = 320;
const int PADDLE_WIDTH = 50;
const int PADDLE_HEIGHT = 6;
const int PADDLE_POS_Y = 260;
const int PADDLE_SPEED = 200; // Travels 200 pixels per second
const int BALL_SPEED = 260;
const int BALL_RADIUS =  4;
const int BALL_START_Y = 160;
// 10 columns with 8 rows
const int NUM_BLOCKS_X = 10;
const int NUM_BLOCKS_Y = 8;
const int BLOCK_WIDTH = 28;
const int BLOCK_HEIGHT = 10;

float paddle_pos_x;
Vector2 ball_pos;
Vector2 ball_dir;
bool started;


float clamp(float d, float min, float max)  
{
	const float t = d < min ? min : d;
	return t > max ? max : t;
}

float magnitude(Vector2 v) 
{
	return sqrtf(v.x*v.x + v.y*v.y);
}


// TODO does raylib have a built in for this?
Vector2 normalize(Vector2 v)
{
	float m = magnitude(v);
	if (m > 0) {
		return (Vector2){v.x/m, v.y/m};
	}

	return (Vector2){0.0f, 0.0f};
}

Vector2 reflect(Vector2 v, Vector2 normal) 
{
	normal = normalize(normal);

	float dotProduct = v.x * normal.x + v.y * normal.y;

	// Apply the reflection formula: v - 2 * dot(v,n) * n
	Vector2 reflection = {
		v.x - 2.0f * dotProduct * normal.x,
		v.y - 2.0f * dotProduct * normal.y
	};

	return reflection;
}


Vector2 subtract(Vector2 v1, Vector2 v2)
{
	return (Vector2) {v1.x - v2.x, v1.y - v2.y};
}

Vector2 add(Vector2 v1, Vector2 v2)
{
	return (Vector2) {v1.x + v2.x, v1.y + v2.y};
}

Vector2 multiply(Vector2 v, float m)
{
	return (Vector2) {v.x * m, v.y * m};
}

void restart(void) 
{
	paddle_pos_x = SCREEN_SIZE/2 - PADDLE_WIDTH/2;
	ball_pos.x = SCREEN_SIZE/2;
	ball_pos.y = BALL_START_Y;
	started = false;
}

int main (void) 
{
	// Sync with monitor refresh rate
	SetConfigFlags(FLAG_VSYNC_HINT);
	InitWindow(960, 960, "Breakout");
	SetTargetFPS(500);

	restart();

	while (!WindowShouldClose()) {

		// --------------------------------------------------------
		// Rendering
		// --------------------------------------------------------
		float dt = 0;

		if (!started) {
			ball_pos.x = SCREEN_SIZE/2 + (float)cosf(GetTime()) * SCREEN_SIZE/2.5;
			ball_pos.y = BALL_START_Y;

			if (IsKeyPressed(KEY_SPACE)) {
				Vector2 paddle_middle = {
					paddle_pos_x + PADDLE_WIDTH/2,
					PADDLE_POS_Y
				};

				// Note: Here the vector will point from the ball to the middle of the paddle
				// If you swap these then you will get a vector from the paddle_middle to the ball position instead
				Vector2 ball_to_paddle = subtract(paddle_middle, ball_pos);
				ball_dir = normalize(ball_to_paddle);
				started = true;
			} 
		} else {
			dt = GetFrameTime();
		}

		Vector2 previous_ball_pos = ball_pos;
		ball_pos.x += ball_dir.x * BALL_SPEED * dt;
		ball_pos.y += ball_dir.y * BALL_SPEED * dt;

		float paddle_move_velocity = 0;

		if (IsKeyDown(KEY_LEFT)) {
			paddle_move_velocity -= PADDLE_SPEED;
		}

		if (IsKeyDown(KEY_RIGHT)) {
			paddle_move_velocity += PADDLE_SPEED;
		}

		paddle_pos_x += paddle_move_velocity * dt;
		// Keep paddle in bounds of window
		paddle_pos_x = clamp(paddle_pos_x, 0, SCREEN_SIZE - PADDLE_WIDTH);

		Rectangle paddle_rect = {
			paddle_pos_x, PADDLE_POS_Y,
			PADDLE_WIDTH, PADDLE_HEIGHT
		};

		if (CheckCollisionCircleRec(ball_pos, BALL_RADIUS, paddle_rect)) {
			Vector2 collision_normal;
			// If ball hits side or top
			if (previous_ball_pos.y < paddle_rect.y + paddle_rect.height) {
				//collision_normal += {0, -1};
				collision_normal = add(collision_normal, (Vector2){0, -1});
				ball_pos.y = paddle_rect.y - BALL_RADIUS;
			}

			// If hit rect at bottom/under, not likely to occur. You lose.
			if (previous_ball_pos.y > paddle_rect.y + paddle_rect.height) {
				//collision_normal += {0, 1};
				collision_normal = add(collision_normal, (Vector2){0, 1});
				ball_pos.y = paddle_rect.y + paddle_rect.height + BALL_RADIUS;
			}

			// ___
			if (previous_ball_pos.x < paddle_rect.x) {
				//collision_normal += {-1, 0};
				collision_normal = add(collision_normal, (Vector2){-1, 0});
			}

			// ___
			if (previous_ball_pos.x > paddle_rect.x + paddle_rect.width) {
				//collision_normal += {1, 0};
				collision_normal = add(collision_normal, (Vector2){1, 0});
			}

			// ___
			if (collision_normal.x != 0 && collision_normal.y != 0) {
				ball_dir = normalize(reflect(ball_dir, normalize(collision_normal)));
			}
		}


		// --------------------------------------------------------
		// Draw
		// --------------------------------------------------------
		BeginDrawing();
		ClearBackground(GetColor(0x96bedcff));

		Camera2D camera = {
			.zoom=(float)(GetScreenHeight()/SCREEN_SIZE)
		};

		BeginMode2D(camera);

		DrawRectangleRec(paddle_rect, GetColor(0x32965aff));
		DrawCircleV(ball_pos, BALL_RADIUS, GetColor(0xca5a14ff));

		EndMode2D();
		EndDrawing();
	}

	CloseWindow();
}
