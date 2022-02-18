#pragma once

#include <utility>
#include <map>
#include "surface.h"

using namespace std;
using namespace Tmpl8;

constexpr int TILE = 32;

class Tile
{
public:
	int ox, oy;
	int cx, cy, dx, dy;	// Collisions: x, y, delta x, delta y
	enum class Terrains_t {
		Snow,
		Ice,
		Gravel,
		CobbleStone
	};
	enum class Objects_t {
		None,
		Tree,
		Rock,
		Log,
		Bush,
		Sign,

		// Not Random Generated
		TopOfTree,
		TwoTrees,

		// Normal map
		Flag
	};

	static map<Terrains_t, pair<int, int>> terrains;

	Terrains_t terrain;
	Objects_t object;

protected:
	static Surface tiles;
	static int t_width;	// tilemap width

public:
	virtual void Draw(int x, int y, Surface& screen);
	virtual void DrawObjectOnly(int x, int y, Surface& screen);

protected:
	virtual void DrawTile(int tx, int ty, Surface& screen, int x, int y);
};

