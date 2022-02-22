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
	/// <summary>
	/// Terrain types
	/// </summary>
	enum class Terrains_t {
		Snow,
		FinishLine,
		Ice,
		Gravel,
		CobbleStone
	};

	/// <summary>
	/// Object types
	/// </summary>
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
		BlueFlag,
		RedFlag,

		// Border trees
		Pine1,
		Pine2,
		WinterTree
	};

	/// <summary>
	/// Use tile factory. (Uses terrain and object)
	/// </summary>
	/// <param name="ox">- Object X from tilemap</param>
	/// <param name="oy">- Object Y from tilemap</param>
	/// <param name="cx">- Collision center X</param>
	/// <param name="cy">- Collision bottom Y</param>
	/// <param name="dx">- Collision delta X</param>
	/// <param name="dy">- Collision delta Y</param>
	/// <param name="object">- Object</param>
	/// <param name="terrain">- Terrain</param>
	/// <param name="tile_width">- tile width, default: TILE</param>
	/// <param name="tile_height">- tile height, default: TILE</param>
	Tile(int ox, int oy, int cx, int cy, int dx, int dy, Objects_t object, Terrains_t terrain, int tile_width=TILE, int tile_height=TILE);

	int ox, oy;
	int cx, cy, dx, dy;	// Collisions: x, y, delta x, delta y

	static map<Terrains_t, pair<int, int>> terrains;

	Terrains_t terrain;
	Objects_t object;

private:
	static Surface tiles;
	static int t_width;	// tilemap width
	int tile_width, tile_height;

public:
	void Draw(int x, int y, Surface& screen);
	void DrawObjectOnly(int x, int y, Surface& screen);

private:
	void DrawTile(int tx, int ty, Surface& screen, int x, int y, bool is_object=false);
};

