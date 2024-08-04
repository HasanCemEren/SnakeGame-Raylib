#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>
using namespace std;

//Colors
Color green = { 173, 204, 96, 255 };
Color darkGreen = { 43, 51, 24, 255 };
//Variables
const int cellSize = 30;
const int cellCount = 25;
int offset = 75;
double lastUpdateTime = 0;

bool EventTriggered(double interval) {
	double currentTime = GetTime();
	if (currentTime - lastUpdateTime >= interval)
	{
		lastUpdateTime = currentTime;
		return true;
	}
	return false;
}
bool ElementInDeque(Vector2 element, deque<Vector2> deque) {
	for (unsigned int i = 0; i < deque.size(); i++)
	{
		if (Vector2Equals(deque[i], element)) {
			return true;
		}
	}
	return false;
}
//Classes
class Snake {
public:
	deque<Vector2> body = { Vector2{6,9}, Vector2{5,9}, Vector2{4,9} };
	Vector2 direction = { 1, 0 };
	bool addSegment = false;


	void Draw() {
		float x, y;
		for (unsigned int i = 0; i < body.size(); i++)
		{
			x = body[i].x;
			y = body[i].y;
			Rectangle segment = Rectangle{ offset + x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize };
			DrawRectangleRounded(segment, 0.5, 6, darkGreen);
		}
	}
	void Update() {
		body.push_front(Vector2Add(body[0], direction));
		if (addSegment)
		{
			addSegment = false;
		}
		else
		{
			body.pop_back();
		}

	}
	void reset() {
		body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };
		direction = { 1, 0 };
	}
};

class Food {
public:
	Vector2 position;
	Texture2D texture;
	Food(deque<Vector2> snakeBody)
	{
		Image image = LoadImage("Graphics/food.png");
		texture = LoadTextureFromImage(image);
		UnloadImage(image);
		position = generateRandomPos(snakeBody);
	}
	~Food() {
		UnloadTexture(texture);
	}

	void Draw() {
		DrawTexture(texture, offset + (position.x * cellSize), offset + (position.y * cellSize), WHITE);
	}

	Vector2 generateRandomCell() {
		float x = GetRandomValue(0, cellCount - 1);
		float y = GetRandomValue(0, cellCount - 1);
		return Vector2{ x, y };
	}

	Vector2 generateRandomPos(deque<Vector2> snakeBody) {
		Vector2 position = generateRandomCell();
		while (ElementInDeque(position, snakeBody))
		{
			position = generateRandomCell();
		}
		return position;
	}
};

class Game {
public:
	Snake snake = Snake();
	Food food = Food(snake.body);
	Sound eatSound;
	Sound wallSound;
	bool isRunning = true;
	unsigned int score = 0;
	int textWidth = MeasureText("Tap to Start", 20);

	Game() {
		InitAudioDevice();
		eatSound = LoadSound("Sounds/eat.mp3");
		wallSound = LoadSound("Sounds/wall.mp3");
	}
	~Game() {
		UnloadSound(eatSound);
		UnloadSound(wallSound);
		CloseAudioDevice();
	}
	void Draw() {
		food.Draw();
		snake.Draw();
		if (!isRunning)
		{
			DrawText("Tap to Start", ((offset + (cellCount * cellSize)) / 2) - (textWidth / 2), ((offset + (cellCount * cellSize)) / 2) - 10, 20, DARKGRAY);
		}
	}
	void Update() {
		if (isRunning)
		{
			snake.Update();
			checkCollisionWithFood();
			checkCollisionWithEdges();
			checkCollisionWithTail();
		}

	}
	void checkCollisionWithFood() {
		if (Vector2Equals(snake.body[0], food.position))
		{
			food.position = food.generateRandomPos(snake.body);
			PlaySound(eatSound);
			snake.addSegment = true;
			score++;
		}
	}
	void checkCollisionWithEdges() {
		if (snake.body[0].x == cellCount || snake.body[0].x == -1)
		{
			GameOver();
		}
		if (snake.body[0].y == cellCount || snake.body[0].y == -1)
		{
			GameOver();
		}
	}
	void checkCollisionWithTail() {
		deque<Vector2> headlessBody = snake.body;
		headlessBody.pop_front();
		if (ElementInDeque(snake.body[0], headlessBody))
		{
			GameOver();
		}
	}
	void GameOver() {
		snake.reset();
		PlaySound(wallSound);
		food.position = food.generateRandomPos(snake.body);
		isRunning = false;
		score = 0;
	}
};

int main() {
	InitWindow(2 * offset + (cellSize * cellCount), 2 * offset + (cellSize * cellCount), "Retro Snake Game");
	SetTargetFPS(60);

	//Instance
	Game game = Game();

	while (WindowShouldClose() == false)
	{

		BeginDrawing();
		//Event Handling

		//Updating Positions

		//Checking for Collisions

		//Drawing
		if (EventTriggered(0.2)) { game.Update(); }
		if ((IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) && (game.snake.direction.y != 1)) { game.snake.direction = { 0, -1 }; }
		if ((IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) && (game.snake.direction.y != -1)) { game.snake.direction = { 0, +1 }; }
		if ((IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) && (game.snake.direction.x != 1)) { game.snake.direction = { -1, 0 }; }
		if ((IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) && (game.snake.direction.x != -1)) { game.snake.direction = { +1, 0 }; }
		if (game.isRunning == false && IsMouseButtonPressed(0))
		{
			game.isRunning = true;
		}
		ClearBackground(green);
		DrawRectangleLinesEx(Rectangle{ (float)offset - 5, (float)offset - 5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10 }, 5, darkGreen);
		DrawText("Retro Snake", offset - 5, 20, 40, darkGreen);
		DrawText(TextFormat("%i", game.score), offset - 5, offset + (cellSize * cellCount) + 10, 40, darkGreen);
		game.Draw();

		EndDrawing();
	}

	CloseWindow();
	return 0;
}