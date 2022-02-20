#include "surface.h"
#pragma once

using namespace Tmpl8;

class Player
{
public:
	float x;
	float y;
	int health;
	int score = 0;
	float speed;
	bool is_hit = false;

	Player(float x, float y, int health = 3, float speed = 0.5f);

	void TurnLeft();
	void TurnRight();
	void SlowDown();
	void Accelerate();
	void NormalPosition();
	void Draw(Surface& screen);
	void Blink(int timer);

private:
	enum class Direction {
		Left = -1,
		Normal = 0,
		Right = 1
	} direction = Direction::Normal;
	static Sprite player;
	static Sprite copy;

	int acceleration_counter = 0;
};

