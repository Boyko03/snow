#pragma once
#include "Map.h"

constexpr int DISTANCE = 7;

class NormalMap : public virtual Map
{
public:
	NormalMap(int rows, int colls, Difficulty difficulty, Surface& screen);

	void AddRow(bool empty = false) override;

	void Move() override;
	void Draw() override;

	bool IsWin() override;

private:
	int flags_counter;
	bool missed_flag = false;

	bool CheckPos(int x, int y);

	void DrawPlayer();
};
