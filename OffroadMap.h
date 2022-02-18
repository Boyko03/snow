#pragma once
#include "Map.h"

class OffroadMap : public virtual Map
{
public:
	OffroadMap(int rows, int colls, Difficulty difficulty, Surface& screen);

	void AddRow(bool empty = false) override;

	void Move() override;
	void Draw() override;

private:
	Tile GenerateTile(int column);
	Tile::Objects_t GetRandomObject();

	bool CheckPos(int x, int y);

	void DrawPlayer();
};
