#include <raylib.h>
#include <raymath.h>

#include <vector>
#include <iostream>

const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;
const char* TITLE = "Raylib-Circles";

constexpr int MAX_ENEMIES = 30;



const enum Direction{
	TOP, BOTTOM, RIGHT, LEFT
};

Vector2 RandomizedDirectionPosition(Direction dir) {
	float x;
	float y;
	switch (dir)
	{
	default:
		x = GetRandomValue(-10, GetScreenWidth());
		y = GetRandomValue(-120, -20);
		break;
	case BOTTOM:
		x = GetRandomValue(-10, GetScreenWidth());
		y = GetRandomValue(GetScreenHeight() + 20, GetScreenHeight() + 120);
		break;
	case RIGHT:
		x = GetRandomValue(GetScreenWidth() + 20, GetScreenWidth() + 120);
		y = GetRandomValue(-10, GetScreenHeight());
		break;
	case LEFT:
		x = GetRandomValue(-120, -20);
		y = GetRandomValue(-10, GetScreenHeight());
		break;
	}

	return Vector2{ x, y };
}

class Circle {
public:

	Vector2 pos;
	float radius;
	Color color;
	Circle(Vector2 pos, float radius, Color color) {
		this->pos = pos;
		this->radius = radius;
		this->color = color;
	}
	void Draw() {
		DrawCircleV(pos, radius, color);
	}
	
};
class AIEnemy : public Circle {
public:
	float speed;
	AIEnemy(Vector2 pos, float radius, Color color, Direction dir) : Circle(pos, radius, color) {
		this->dir = dir;
		speed = GetRandomValue(100, 250);
	}

	void Update() {
		Move();
	}

	void ResetPosition() {
		dir = (Direction)GetRandomValue(0, 3);
		pos = RandomizedDirectionPosition(dir);
	}

private:
	Direction dir;


	void Move() {
		Vector2 moveDir{};
		switch (dir)
		{
			// move to the bottom
		case TOP:
			moveDir.y += 1.0f;
			if (pos.y - radius >= GetScreenHeight()) {
				ResetPosition();
			}
			break;
			// move to the top
		case BOTTOM:
			moveDir.y -= 1.0f;
			if (pos.y + radius <= 0) {
				ResetPosition();
			}
			break;
			// move to the left
		case RIGHT:
			moveDir.x -= 1.0f;
			if (pos.x + radius <= 0) {
				ResetPosition();
			}
			break;
			// move to the right
		case LEFT:
			moveDir.x += 1.0f;
			if (pos.x - radius >= GetScreenWidth()) {
				ResetPosition();
			}
			break;
		}
		pos += moveDir * speed * GetFrameTime();
	}
};

class Player : public Circle {
public:
	float speed = 250.0f;
	bool isDead = false;
	Player(Vector2 pos, float radius, Color color) : Circle(pos, radius, color) {
		startingPos = pos;
	}

	void Update() {
		InputHandling();
		ScreenBoundaries();
	}

	void OnHitEnemy(const AIEnemy& enemy) {
		if (CheckCollisionCircles(pos, radius, enemy.pos, enemy.radius)) {
			isDead = true;
		}
	}

	void ResetPosition() {
		pos = startingPos;
	}

private:
	Vector2 startingPos{};
	void InputHandling() {
		Vector2 moveDir{};
		if (IsKeyDown(KEY_W)) moveDir.y -= 1.0f;
		if (IsKeyDown(KEY_S)) moveDir.y += 1.0f;
		if (IsKeyDown(KEY_A)) moveDir.x -= 1.0f;
		if (IsKeyDown(KEY_D)) moveDir.x += 1.0f;

		if (moveDir != Vector2Zero()) {
			moveDir = Vector2Normalize(moveDir);
		}

		pos += moveDir * speed * GetFrameTime();
	}

	void ScreenBoundaries() {
		if (pos.x - radius <= 0) {
			pos.x = radius;
		}
		if (pos.x + radius >= GetScreenWidth()) {
			pos.x = GetScreenWidth() - radius;
		}
		if (pos.y - radius <= 0) {
			pos.y = radius;
		}
		if (pos.y + radius >= GetScreenHeight()) {
			pos.y = GetScreenHeight() - radius;
		}
	}
};




void MainProgram() {
	InitWindow(SCR_WIDTH, SCR_HEIGHT, TITLE);
	SetRandomSeed(time(nullptr));
	SetTargetFPS(60);

	Player player{ Vector2{SCR_WIDTH / 2 , SCR_HEIGHT / 2}, 20.0f, BLACK };

	std::vector<AIEnemy> enemies{};

	float score = 0;

	for (int i = 0; i < MAX_ENEMIES; i++) {
		Color color{
			GetRandomValue(15, 230),
			GetRandomValue(15, 230),
			GetRandomValue(15, 230),
			255
		};
		// randomize direction
		Direction dir = (Direction)GetRandomValue(0, 3);

		AIEnemy enemy{ RandomizedDirectionPosition(dir), 20.0f, color, dir };
		enemies.push_back(enemy);
	}

	while (!WindowShouldClose()) {
		if (!player.isDead) {
			score += 0.1f;
			player.Update();

			// update all enemies in scene
			for (auto& enemy : enemies) {
				enemy.Update();

				// check if enemy hits player
				player.OnHitEnemy(enemy);
			}
		}
		BeginDrawing();
		ClearBackground(RAYWHITE);
		if (!player.isDead) {
			DrawText(TextFormat("%i", (int)score), 20, 30, 60, BLACK);
			player.Draw();
			for (auto& enemy : enemies) {
				enemy.Draw();
			}
		}
		else {
			DrawText("Final Score", ((GetScreenWidth() / 2) - 105), ((GetScreenHeight() / 2) - 75), 30, BLACK);
			DrawText(TextFormat("%i", (int)score), (GetScreenWidth() / 2) - 40, (GetScreenHeight() / 2) - 40, 80, BLACK);
			DrawText("Press R to restart", (GetScreenWidth() / 2) - 150, GetScreenHeight() - 100, 30, BLACK);

			if (IsKeyPressed(KEY_R)) {
				player.isDead = false;
				player.ResetPosition();
				score = 0;
				for (auto& enemy : enemies) {
					enemy.ResetPosition();
				}
			}
		}
		
		EndDrawing();
	}
}

#if _DEBUG
int main() {
	MainProgram();
	return 0;
}
#else
int WinMain() {
	MainProgram();
	return 0;
}
#endif
