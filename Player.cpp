#include "Player.h"
#include <cstdio>
#include <Windows.h>
#include <cmath>

Sprite Player::copy = Sprite(new Surface("assets/skier.png"), 6);

Player::Player(vec2 pos, int health, float speed) : pos(pos), health(health), speed(speed),
	player(Sprite(new Surface("assets/skier.png"), 6)),
	collider(BoxCollider(vec2(12, 17), vec2(20, 25)))
{
	player.SetFrame(2);
}

void Player::TurnLeft()
{
	speed *= (float)pow(0.999f, elapsedTime);
	direction = Direction::Left;
}

void Player::TurnRight()
{
	speed *= (float)pow(0.999, elapsedTime);
	direction = Direction::Right;
}

void Player::SlowDown()
{
	speed -= 0.1f * elapsedTime;
	if (speed < 0) speed = 0;
	player.SetFrame(5);
	direction = Direction::Normal;
}

void Player::Accelerate()
{
	if (acceleration_counter < 10) player.SetFrame(0), speed += 0.01f * elapsedTime;
	else if (acceleration_counter < 20) player.SetFrame(2);
	else if (acceleration_counter < 30) player.SetFrame(1), speed += 0.01f * elapsedTime;
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

void Player::Draw(Surface& screen, float elapsedTime)
{
	this->elapsedTime = elapsedTime;

	SetCorrectFrame();
	speed += 0.001f * elapsedTime;
	if (speed > 5.0f) speed = 5.0f;
	pos.x += (float)direction * speed * elapsedTime;

	// Blink
	if (hit_timer >= 0) Blink((int)hit_timer), hit_timer -= elapsedTime;
	else if (is_hit) Blink(0), is_hit = false;
	player.Draw(&screen, (int)pos.x, (int)pos.y);

	// Draw shield
	if (shield_timer >= 0) DrawShield(screen), shield_timer -= elapsedTime;
}

void Player::DrawShield(Surface& screen)
{
	static Surface shield = Surface("assets/shield.png");

	if (shield_timer > 100 || (int)shield_timer % 20 < 10) {
		shield.BlendCopyTo(&screen, (int)pos.x, (int)pos.y);
		screen.Circle(pos.x, pos.y, (float)shield.GetHeight() / 2, 0xff79f2f2);
	}
}

void Player::DrawCollisionBox(Surface& screen)
{
	screen.Box((int)(pos.x + collider.min.x), (int)(pos.y + collider.min.y),
		(int)(pos.x + collider.max.x), (int)(pos.y + collider.max.y), 0xff00ff00);
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
			speed += 0.001f * elapsedTime;
		}
		if (current_frame == 3) player.SetFrame(2);
		else player.SetFrame(4);
	}
	else if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState(VkKeyScanA('d'))) {
		TurnRight();
		if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState(VkKeyScanA('s'))) {
			speed += 0.001f * elapsedTime;
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