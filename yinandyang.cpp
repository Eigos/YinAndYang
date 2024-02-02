#include <iostream>
#include <vector>
#include "raylib/raylib.hpp"

//---- Util Methods ----//
bool CollisionAABB(const Vector2& p1, const Vector2& p2);
constexpr float DegreeToRadianF(const float& degree);

//---- Classes ----//
enum class SquareTeam {
	TEAM_WHITE,
	TEAM_BLACK,
};

class Square {

public:

	Square(Vector2 position, SquareTeam team) :
		position(position), team(team) {}

	void setTeam(SquareTeam newTeam) {
		this->team = newTeam;
	}

	const SquareTeam getTeam() const {
		return this->team;
	}

	const Vector2 getPosition() const {
		return this->position;
	}

	void swapTeam() {
		if (team == SquareTeam::TEAM_BLACK)
			team = SquareTeam::TEAM_WHITE;
		else
			team = SquareTeam::TEAM_BLACK;
	}

protected:

	void setPosition(Vector2 position) {
		this->position = position;
	}

private:
	Vector2 position;
	SquareTeam team;

};

class DynamicSquare : public Square {

public:

	DynamicSquare(Vector2 position, SquareTeam team) : Square(position, team) {
		direction = GetRandomValue(0, 360);
	}

	void move(float dt) {
		Vector2 pos = getPosition();
		setPosition({
			((cosf(DegreeToRadianF(direction)) * velocity) * dt) + pos.x,
			((sinf(DegreeToRadianF(direction)) * velocity) * dt) + pos.y
			});
	}

	void reflect() {
		direction += 90;
	}

	void setDirection(float direction) {
		this->direction = direction;
	}

	float getDirection() {
		return direction;
	}

private:

	float direction;
	const float velocity = 500.0f;

};

//---- Constants ----//
static const int windowWidth = 1200;
static const int windowHeight = 840;
static const int squareSizeLenght = 60;

//---- Global Variables ----//
std::vector<Square> squares;
std::vector<DynamicSquare> players;

//---- Methods ----//
void InitSquares();
void DrawSquare(const Square& square);
Color SquareTeamToColor(SquareTeam team);
void CheckForBorders(DynamicSquare& dynamicSquare);
void CheckForSquareCollision(DynamicSquare& dynamicSquare);

int main()
{

	InitWindow(windowWidth, windowHeight, "Ying And Yang");
	SetTargetFPS(120);

	InitSquares();

	float dt = 0.0f;

	while (!WindowShouldClose())
	{
		float beg = static_cast<float>(GetTime());

		for (DynamicSquare& square : players) {
			square.move(dt);
			CheckForBorders(square);
			CheckForSquareCollision(square);
		}

		BeginDrawing();
		ClearBackground(RAYWHITE);

		for (Square& square : squares)
			DrawSquare(square);

		for (DynamicSquare& square : players) {
			square.swapTeam();
			DrawSquare(square);
			square.swapTeam();
		}

		DrawFPS(20, 20);
		EndDrawing();

		dt = static_cast<float>(GetTime()) - beg;
	}

	CloseWindow();

	return 0;
}


void InitSquares() {
	int xAxisSquareCount = windowWidth / squareSizeLenght;
	int yAxisSquareCount = windowHeight / squareSizeLenght;
	int totalSquareCount = xAxisSquareCount * yAxisSquareCount;

	for (int i = 0; i < totalSquareCount; i++) {

		float posX = (squareSizeLenght / 2) + ((i % xAxisSquareCount) * squareSizeLenght);
		float posY = (squareSizeLenght / 2) + ((i / xAxisSquareCount) * squareSizeLenght);

		Vector2 position{ posX, posY };
		SquareTeam team = i >= (totalSquareCount / 2) ? SquareTeam::TEAM_BLACK : SquareTeam::TEAM_WHITE;

		squares.push_back(Square(position, team));
	}

	players.push_back(DynamicSquare({ windowWidth / 2, 3 * windowHeight / 4 }, SquareTeam::TEAM_BLACK));
	players.push_back(DynamicSquare({ windowWidth / 2, windowHeight / 4 }, SquareTeam::TEAM_WHITE));
}

void DrawSquare(const Square& square) {

	DrawRectangle(
		square.getPosition().x - (squareSizeLenght / 2),
		square.getPosition().y - (squareSizeLenght / 2),
		squareSizeLenght,
		squareSizeLenght,
		SquareTeamToColor(square.getTeam())
	);
}

Color SquareTeamToColor(SquareTeam team) {

	switch (team) {
	case SquareTeam::TEAM_BLACK:
		return BLACK;
	case SquareTeam::TEAM_WHITE:
		return WHITE;
	default:
		return PURPLE;
	}
}

void CheckForBorders(DynamicSquare& dynamicSquare) {
	Vector2 pos = dynamicSquare.getPosition();

	if (pos.x < 0 || pos.x > windowWidth) {
		dynamicSquare.reflect();
		return;
	}

	if (pos.y < 0 || pos.y > windowHeight) {
		dynamicSquare.reflect();
		return;
	}

}

void CheckForSquareCollision(DynamicSquare& dynamicSquare) {
	dynamicSquare.swapTeam();

	for (Square& square : squares) {

		if (square.getTeam() != dynamicSquare.getTeam())
			continue;

		if (!CollisionAABB(dynamicSquare.getPosition(), square.getPosition()))
			continue;
		
		square.swapTeam();
		dynamicSquare.reflect();
		break;
	}

	dynamicSquare.swapTeam();
}

bool CollisionAABB(const Vector2& p1, const Vector2& p2) {

	// collision x-axis?
	bool collisionX = p1.x + squareSizeLenght >= p2.x &&
		p2.x + squareSizeLenght >= p1.x;

	// collision y-axis?
	bool collisionY = p1.y + squareSizeLenght >= p2.y &&
		p2.y + squareSizeLenght >= p1.y;

	// collision only if on both axes
	return collisionX && collisionY;
}

constexpr float DegreeToRadianF(const float& degree)
{
	return degree * (float)(3.14159265359 / 180);
}