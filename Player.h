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
	int shield_timer = -1;

	Player(float x, float y, int health = 3, float speed = 0.5f);

	void Draw(Surface& screen);
	void Blink(int timer);

private:
	enum class Direction {
		Left = -1,
		Normal = 0,
		Right = 1
	} direction = Direction::Normal;
	Sprite player;
	static Sprite copy;

	int acceleration_counter = 0;
	int current_frame = 2;

	void TurnLeft();
	void TurnRight();
	void SlowDown();
	void Accelerate();
	void NormalPosition();
	void SetCorrectFrame();

	void DrawShield(Surface& screen);
};

