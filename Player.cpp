#include "Player.h"
#include <cstdio>
#include <Windows.h>

Sprite Player::copy = Sprite(new Surface("assets/skier_32.png"), 6);

Player::Player(float x, float y, int health, float speed) : x(x), y(y), health(health), speed(speed),
	player(Sprite(new Surface("assets/skier_32.png"), 6))
{
	player.SetFrame(2);
}

void Player::TurnLeft()
{
	//x -= speed + 1.0f;
	speed *= 0.999f;
	direction = Direction::Left;
}

void Player::TurnRight()
{
	//x += speed + 1.0f;
	speed *= 0.999;
	direction = Direction::Right;
}

void Player::SlowDown()
{
	speed -= 0.1f;
	if (speed < 0) speed = 0;
	player.SetFrame(5);
	direction = Direction::Normal;
}

void Player::Accelerate()
{
	if (acceleration_counter < 10) player.SetFrame(0), speed += 0.01f;
	else if (acceleration_counter < 20) player.SetFrame(2);
	else if (acceleration_counter < 30) player.SetFrame(1), speed += 0.01f;
	else acceleration_counter = 0;
	acceleration_counter++;
	direction = Direction::Normal;
}

void Player::NormalPosition()
{
	player.SetFrame(2);
	acceleration_counter = 0;
	direction = Direction::Normal;
}

void Player::Draw(Surface& screen)
{
	SetCorrectFrame();
	speed += 0.001f;
	if (speed > 5.0f) speed = 5.0f;
	x += (float)direction * (speed);
	player.Draw(&screen, (int)x, (int)y);
}

void Player::Blink(int timer)
{
	int width = player.GetSurface()->GetWidth();
	int height = player.GetSurface()->GetHeight();
	int step = 25;
	Pixel* buf = player.GetBuffer();
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			Pixel p = buf[i * width + j];
			Pixel ref = copy.GetBuffer()[i * width + j];
			
			// Original values
			int refA = (ref & 0xff000000) >> 24;
			int refR = (ref & 0xff0000) >> 16;
			int refG = (ref & 0x00ff00) >> 8;
			int refB = ref & 0x0000ff;

			// New values
			// int r = refR / step * (timer % (2 * step) - step);	// Use original red
			int g = refG / step * (timer % (2 * step) - step);
			int b = refB / step * (timer % (2 * step) - step);
			
			// For smoother transition
			// if (r == 0 && refR != 0) r = 1;						// Use original red
			if (g == 0 && refG != 0) g = 1;
			if (b == 0 && refB != 0) b = 1;

			// Values are negative when image is appearing
			// if (r < 0) r = -r;									// Use original red
			if (g < 0) g = -g;
			if (b < 0) b = -b;

			// Apply new color
			buf[i * width + j] = (refA << 24) | (refR << 16) | (g << 8) | b;
		}
	}
}

void Player::SetCorrectFrame()
{
	current_frame = player.GetFrame();

	if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState(VkKeyScanA('a'))) {
		TurnLeft();
		if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState(VkKeyScanA('s'))) {
			speed += 0.001f;
		}
		if (current_frame == 3) player.SetFrame(2);
		else player.SetFrame(4);
	}
	else if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState(VkKeyScanA('d'))) {
		TurnRight();
		if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState(VkKeyScanA('s'))) {
			speed += 0.001f;
		}
		if (current_frame == 4) player.SetFrame(2);
		else player.SetFrame(3);
	}
	else if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState(VkKeyScanA('w'))) {
		SlowDown();
	}
	else if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState(VkKeyScanA('s'))) {
		Accelerate();
	}
	else {
		NormalPosition();
	}
}