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
	int shield_timer = -1; // shield is active if timer > 0

	Player(float x, float y, int health = 3, float speed = 0.5f);

	/// <summary>
	/// Draw player on screen
	/// </summary>
	/// <param name="screen">- surface to draw player on to</param>
	/// <param name="elapsedTime">- elapsed time</param>
	void Draw(Surface& screen, float elapsedTime);
	/// <summary>
	/// Fade player to red and back
	/// </summary>
	/// <param name="timer">- animation frame [0 ~ 100]</param>
	void Blink(int timer);

private:
	enum class Direction {
		Left = -1,
		Normal = 0,
		Right = 1
	} direction = Direction::Normal;
	Sprite player;
	static Sprite copy;				// used to set player's colors to original after blinking

	int acceleration_counter = 0;	// Used to accelerate gradually
	int current_frame = 2;			// current player sprite frame

	float elapsedTime = 0;			// Elapsed time since last frame

	/// <summary>
	/// Set direction to left
	/// </summary>
	void TurnLeft();
	/// <summary>
	/// Set direction to right
	/// </summary>
	void TurnRight();
	/// <summary>
	/// Reduce speed, set player direction to normal
	/// </summary>
	void SlowDown();
	/// <summary>
	/// Increase player speed gradually
	/// </summary>
	void Accelerate();
	/// <summary>
	/// Set direction to normal
	/// </summary>
	void NormalPosition();
	/// <summary>
	/// Handles user input to set the appropriate frame for the player sprite
	/// </summary>
	void SetCorrectFrame();

	/// <summary>
	/// Draw shield around player
	/// </summary>
	/// <param name="screen"></param>
	void DrawShield(Surface& screen);
};

