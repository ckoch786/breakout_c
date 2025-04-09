// TODO which compiler and what version am I using?
// Which operating systems and architectures/system capabilities does this build/run on?
// Idiosyncracies between GNU/Linux and Windows
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

// TODO which version of this am I currently using?
#include <raylib.h>

#include "vector.h"

const int SCREEN_SIZE = 320;
const int PADDLE_WIDTH = 50;
const int PADDLE_HEIGHT = 6;
const int PADDLE_POS_Y = 260;
const int PADDLE_SPEED = 200; // Travels 200 pixels per second
const int BALL_SPEED = 260;
const int BALL_RADIUS =  4;
const int BALL_START_Y = 160;
// TODO try using:
// static const in
// enum { NUM_BLOCKS_X  = 10, NUM_BLOCKS_Y = 8 };
// 10 columns with 8 rows
#define NUM_BLOCKS_X 10
#define NUM_BLOCKS_Y 8
const int BLOCK_WIDTH = 28;
const int BLOCK_HEIGHT = 10;

typedef enum {
	Yellow,
	Green,
	Orange,
	Red
} Block_Color;

Block_Color row_colors[NUM_BLOCKS_Y] = {
	Red,
	Red,
	Orange,
	Orange,
	Green,
	Green,
	Yellow,
	Yellow
};

Color block_color_values[4] = {
	[Yellow] = { 253, 24, 150, 255 },
	[Green] = { 180, 245, 190, 255 },
	[Orange] = { 170, 90, 85, 255 },
	[Red] = { 255, 90, 85, 255 },
};

const int block_color_score[4] = {
	[Yellow] = 2,
	[Green] = 4,
	[Orange] = 6,
	[Red] = 8
};

bool blocks[NUM_BLOCKS_X][NUM_BLOCKS_Y];
float paddle_pos_x = 0.0f;
Vector2 ball_pos; // TODO initialize these to the rl ZeroVector2
Vector2 ball_dir;
bool started = false;
bool game_over = false;
int score = 0;


// TODO double check this, what other ways are there to handle this?
float clamp(float d, float min, float max)  
{
	const float t = d < min ? min : d;
	return t > max ? max : t;
}

Rectangle calc_block_rect(int x, int y)
{
	return (Rectangle) {
		// Top-left corner position
		.x = (float)(20 + x * BLOCK_WIDTH),
		.y = (float)(40 + y * BLOCK_HEIGHT),
		.width = BLOCK_WIDTH,
		.height = BLOCK_HEIGHT
	};
}

// TODO does raylib have a built in for this?
// TODO yes, it appear that it does it is in raymath.h everything is the same but the normalize function seems off.
// TODO phase out the custom vector stuff but move it into a separate repo for colletions for learning purposes and 
// for the possiblity of using it for my own engine development down the road if for instance I choose to isolate libraries
// so that I can pull out parts/modify them to work the way I would like them to.
Vector2 normalize(Vector2 v)
{
	Vector2D v2 = (Vector2D){v.x, v.y};
	float m = cjk_magnitude(v2);
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
	return (Vector2) {
		v.x - 2.0f * dotProduct * normal.x,
		v.y - 2.0f * dotProduct * normal.y
	};
}

Vector2D v2to2d(Vector2 v) 
{
	return (Vector2D) {v.x, v.y};
}

Vector2 subtract(Vector2 v1, Vector2 v2)
{
	Vector2D result = cjk_subtract(v2to2d(v1), v2to2d(v2));
	return (Vector2) {result.x, result.y};
}


Vector2 add(Vector2 v1, Vector2 v2)
{
	Vector2D result = cjk_add(v2to2d(v1), v2to2d(v2));
	return (Vector2) {result.x, result.y};
}

Vector2 multiply(Vector2 v, float m)
{
	Vector2D result = cjk_multiply(v2to2d(v), m);
	return (Vector2) {result.x, result.y};
}

void restart(void) 
{
	paddle_pos_x = SCREEN_SIZE/2 - PADDLE_WIDTH/2;
	ball_pos = (Vector2) { 
		.x = SCREEN_SIZE/2,
		.y = BALL_START_Y
	};
	// TODO is the latest versions of C used in embedded development?  What version of C is the majority of embedded C in?
	started = false;
	game_over = false;
	score = 0;

	for (int x=0; x < NUM_BLOCKS_X; ++x) {
		for (int y=0; y < NUM_BLOCKS_Y; ++y) {
			blocks[x][y] = true;
		}
	}
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
			ball_pos = (Vector2) {
				.x = SCREEN_SIZE/2 + (float)cosf(GetTime()) * SCREEN_SIZE/2.5,
				.y = BALL_START_Y
			};

			if (IsKeyPressed(KEY_SPACE)) {
				Vector2 paddle_middle = {
					paddle_pos_x + PADDLE_WIDTH/2,
					PADDLE_POS_Y
				};

				// Note: Here the vector will point from the ball to the middle of the paddle
				// If you swap these then you will get a vector from the paddle_middle to the ball position instead
				Vector2 ball_to_paddle = subtract(paddle_middle, ball_pos);
				ball_dir = normalize((Vector2){ball_to_paddle.x, ball_to_paddle.y});
				started = true;
			} 
		} else if (game_over){
			if (IsKeyPressed(KEY_SPACE)) {
				restart();
			}
		} else {
			dt = GetFrameTime();
		}

		Vector2 previous_ball_pos = ball_pos;
		ball_pos.x += ball_dir.x * BALL_SPEED * dt;
		ball_pos.y += ball_dir.y * BALL_SPEED * dt;

		// Handle the ball bouncing off the right side of the screen
		if (ball_pos.x + BALL_RADIUS > SCREEN_SIZE) {
			ball_pos.x = SCREEN_SIZE - BALL_RADIUS;
			ball_dir = reflect(ball_dir, (Vector2){-1,0});
		}

		// Handle the ball bouncing of the left side of the screen
		if (ball_pos.x - BALL_RADIUS < 0) {
			// 0 +
			ball_pos.x = BALL_RADIUS;
			ball_dir = reflect(ball_dir, (Vector2){1, 0});
		}

		// Handle the ball bouncing off the top of the screen
		if (ball_pos.y - BALL_RADIUS < 0) {
			ball_pos.y = BALL_RADIUS;
			ball_dir = reflect(ball_dir, (Vector2){0,1});
		}
		
		// y + screen_size = middle of ball
		if (!game_over && ball_pos.y > SCREEN_SIZE + BALL_RADIUS * 6) {
			game_over = true;
		}

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

		// Collision of ball with paddle
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

			// The left? side of the paddle
			if (previous_ball_pos.x < paddle_rect.x) {
				//collision_normal += {-1, 0};
				collision_normal = add(collision_normal, (Vector2){-1, 0});
			}

			// The right? side of the paddle
			if (previous_ball_pos.x > paddle_rect.x + paddle_rect.width) {
				//collision_normal += {1, 0};
				collision_normal = add(collision_normal, (Vector2){1, 0});
			}

			// ___
			if (collision_normal.x != 0 && collision_normal.y != 0) {
				ball_dir = normalize(reflect(ball_dir, normalize(collision_normal)));
			}
		}

		//printf("FROM THE TOP!!!!!!!!!!!!!!!!!!!!!!!!!");
		bool found_block = false;

		// Handle collision of ball with blocks
block_x_loop:	for (int x = 0; x < NUM_BLOCKS_X; ++x) {
			for (int y = 0; y < NUM_BLOCKS_Y; ++y) {
				// TODO does this just prevent registering the collision for a block that a has already been it? Is it still
				// on the screen just no longer visible?
				if (blocks[x][y] == false) {
					// printf("blocks[%d][%d] is %d\n", x, y, blocks[x][y]);
					continue;
				}

				Rectangle block_rect = calc_block_rect(x ,y);

				// TODO Here we are using the previous frames ball position? Around 1:08:00 Balls and blocks collision
				if (CheckCollisionCircleRec(ball_pos, BALL_RADIUS, block_rect)) {
					Vector2 collision_normal = (Vector2){0,0};

					// ball hit from top
					if (previous_ball_pos.y < block_rect.y) {
						puts("ball hit from top");
						collision_normal = add(collision_normal, (Vector2) {0, -1});
					}

					// ball hit from bottom
					if (previous_ball_pos.y > block_rect.y + block_rect.height) {
						puts("ball hit from bottom");
						collision_normal = add(collision_normal, (Vector2) {0, 1});
					}

					// on left side
					if (previous_ball_pos.x < block_rect.x) {
						puts("ball hit from left side");
						collision_normal = add(collision_normal, (Vector2) {-1, 0});
					}

					// on right side
					if (previous_ball_pos.x > block_rect.x + block_rect.width) {
						puts("ball hit from right side");
						collision_normal = add(collision_normal, (Vector2) {1, 0});
					}

					// Reflecting the ball off the block
					if (collision_normal.x != 0 && collision_normal.y != 0) {
						ball_dir = reflect(ball_dir, collision_normal);
					}
					if (blocks[x][y]) {
						// Destroy the block
						blocks[x][y] = false;
						Block_Color row_color = row_colors[y]; 
						score += block_color_score[row_color];
						found_block = true;
						// TODO does this work the same way as the Odin break to label?
						goto block_x_loop;
					}
				}

			}
//			if (found_block) {
//				found_block = false;
//				break;
//			}
		}


		// --------------------------------------------------------
		// Draw
		// --------------------------------------------------------
		BeginDrawing();
		// TODO Write a function to trasform color numbers e.g.  {90, 80, 78, 255} to the format required by GetColor
		ClearBackground(GetColor(0x96bedcff));

		Camera2D camera = {
			.zoom=(float)(GetScreenHeight()/SCREEN_SIZE)
		};

		BeginMode2D(camera);

		// TODO did I need to use GetColor hear or could I just have passed in a Color struct?
		DrawRectangleRec(paddle_rect, GetColor(0x32965aff));
		DrawCircleV(ball_pos, BALL_RADIUS, GetColor(0xca5a14ff));

		// Do not draw blocks that the player has already hit
		// 10 columns with 8 rows
		for (int x=0; x < NUM_BLOCKS_X; ++x) {
			for (int y=0; y < NUM_BLOCKS_Y; ++y) {
				if (blocks[x][y] == false) {
					continue;
				}

				Rectangle block_rect = calc_block_rect(x, y);
				
				Vector2 top_left = { block_rect.x, block_rect.y };
				Vector2 top_right = { block_rect.x + block_rect.width, block_rect.y };
				Vector2 bottom_left = { block_rect.x, block_rect.y + block_rect.height };
				// TODO unused
				// Vector2 bottom_right = { block_rect.x + block_rect.width, block_rect.y + block_rect.height };

				DrawRectangleRec(block_rect, block_color_values[row_colors[y]]);
				DrawLineEx(top_left, top_right, 1, (Color){ 255, 255, 150, 100	});
				DrawLineEx(top_left, bottom_left, 1, (Color){ 255, 255, 150, 100 });

				// Looks like light is shining from the top left to give us darker colors
				DrawLineEx(top_right, top_right, 1, (Color){ 0, 0, 50, 100 });
				DrawLineEx(top_left, top_left, 1, (Color){ 0, 0, 50, 100 });
			}
		}

		// TODO this is stored on the stack correct?  Why would you use malloc etc instead and freeing it later?  Does that 
		// not put it on the heap instead?  What does that really mean?  It what ways are the stack and the heap different?
		// TODO does this take up 50 bytes?
		char score_text[50];
		sprintf(score_text, "%d", score);
		// 10 px
		// TODO will this overflow?
		DrawText(score_text, 5, 5, 10, WHITE);

		if (!started) {
			const char *start_text = "Start: SPACE";
			const int font_size = 15;
			const int start_text_width = MeasureText(start_text, font_size);
			DrawText(start_text, SCREEN_SIZE/2 - start_text_width/2, BALL_START_Y-30, 15, WHITE);
		}

		if (game_over) {
			// TODO does this take up 100 bytes?
			const char game_over_text[100];
			sprintf(game_over_text, "Score: %d. Reset: SPACE", score);
			const int font_size = 15;
			const int game_over_text_width = MeasureText(game_over_text, font_size);
			DrawText(game_over_text, SCREEN_SIZE/2 - game_over_text_width/2, BALL_START_Y-30, 15, WHITE);

		}

		EndMode2D();
		EndDrawing();
	}

	CloseWindow();
}
