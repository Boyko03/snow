#include "Player.h"
#include <cstdio>

Sprite Player::player = Sprite(new Surface("assets/skier2.png"), 6);
Sprite Player::copy = Sprite(new Surface("assets/skier2.png"), 6);

Player::Player(float x, float y, int health, float speed) : x(x), y(y), health(health), speed(speed)
{
	player.SetFrame(2);
}

void Player::TurnLeft()
{
	//x -= speed + 1.0f;
	speed *= 0.999f;
	player.SetFrame(4);
	direction = Direction::Left;
}

void Player::TurnRight()
{
	//x += speed + 1.0f;
	speed *= 0.999;
	player.SetFrame(3);
	direction = Direction::Right;
}

void Player::SlowDown()
{
	speed -= 0.5f;
	if (speed < 0) speed = 0;
	player.SetFrame(5);
	direction = Direction::Normal;
}

void Player::Accelerate()
{
	static int counter = 0;
	if (counter % 5 == 0) {
		switch (counter / 5)
		{
		case 1: case 3: player.SetFrame(1); break;
		case 2: player.SetFrame(2); break;
		default:
			player.SetFrame(0);
			counter = 0;
			speed += 0.2f;
			break;
		}
	}
	counter++;
	direction = Direction::Normal;
}

void Player::NormalPosition()
{
	player.SetFrame(2);
	direction = Direction::Normal;
}

void Player::Draw(Surface& screen)
{
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
			buf[i * width + j] = (refR << 16) | (g << 8) | b;
		}
	}
}
