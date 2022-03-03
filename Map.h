#pragma once

#include <vector>
#include "Tile.h"
#include "Player.h"
#include "TileFactory.h"
using namespace std;

class TileFactory;
class Map
{
public:
	enum class Difficulty {
		Easy = 0,
		Medium,
		Hard
	};

protected:
	vector<Tile> map;
	int rows;
	int colls;
	int border_width = 3;
	float current_position = 0;
	int first_row = 0;
	int width;
	TileFactory tileFactory;
	Player* player;
	Surface& screen;
	Difficulty difficulty;

public:
	Map(int rows, int colls, Difficulty difficulty, Surface& screen);
	virtual ~Map();

	virtual void AddRow(bool empty = false) = 0;

	virtual void Move(float deltaTime) = 0;

	virtual void Draw() = 0;

	virtual bool IsWin() { return false; }

	Player* GetPlayer() { return player; }

protected:
	void DrawHearts();

	void AddBorder();

	void DrawBackground();
	void DrawForeground();
};

