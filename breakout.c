#include <stdio.h>
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


float clamp(float d, float min, float max)  
{
	const float t = d < min ? min : d;
	return t > max ? max : t;
}

void restart(void) 
{
	paddle_pos_x = SCREEN_SIZE/2 - PADDLE_WIDTH/2;
	ball_pos.x = SCREEN_SIZE/2;
	ball_pos.y = BALL_START_Y;
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
		float dt = GetFrameTime();

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


		// --------------------------------------------------------
		// Draw
		// --------------------------------------------------------
		BeginDrawing();
		ClearBackground(GetColor(0x96bedcff));

		Camera2D camera = {
			.zoom=(float)(GetScreenHeight()/SCREEN_SIZE)
		};

		BeginMode2D(camera);

		Rectangle paddle_rect = {
				paddle_pos_x, PADDLE_POS_Y,
				PADDLE_WIDTH, PADDLE_HEIGHT
		};

		DrawRectangleRec(paddle_rect, GetColor(0x32965aff));
		DrawCircleV(ball_pos, BALL_RADIUS, GetColor(0xca5a14ff));

		EndMode2D();
		EndDrawing();
	}

	CloseWindow();
}
