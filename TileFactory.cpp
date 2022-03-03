#include "TileFactory.h"
#include "Tile.h"

Tile TileFactory::getTile(Tile::Terrains_t terrain, Tile::Objects_t object)
{
	// Tile(int ox, int oy, int cx, int cy, int dx, int dy, Objects_t object,
	//		Terrains_t terrain, int tile_width=TILE, int tile_height=TILE);
	switch (object)
	{
	// Objects
	case Tile::Objects_t::None:
		return Tile(0, 1, TILE / 2, 25, 4, 7, object, terrain);
	case Tile::Objects_t::Tree:
		return Tile(0, 1, TILE / 2, 25, 4, 7, object, terrain);
	case Tile::Objects_t::Rock:
		return Tile(2, 1, TILE / 2, 28, 10, 10, object, terrain);
	case Tile::Objects_t::Bush:
		return Tile(1, 1, TILE / 2, 28, 12, 16, object, terrain);
	case Tile::Objects_t::Log:
		return Tile(3, 0, TILE / 2, 22, 14, 6, object, terrain);
	// Not used yet
	case Tile::Objects_t::Sign:
		return Tile(2, 0, TILE / 2, 25, 10, 7, object, terrain);
	// Not randomly generated. Used for correct drawing of trees
	case Tile::Objects_t::TwoTrees:
		return Tile(1, 0, TILE / 2, 25, 4, 7, object, terrain);
	case Tile::Objects_t::TopOfTree:
		return Tile(0, 0, -1, -1, 0, 0, object, terrain);
	// Powerups
	case Tile::Objects_t::Heart:
		return Tile(5, 2, TILE / 2, TILE - 2, TILE / 2 - 2, TILE - 4, object, terrain);
	// Flags
	case Tile::Objects_t::BlueFlag:
		return Tile(6, 0, TILE / 2, 20, 6, 16, object, terrain, TILE, 2 * TILE);
	case Tile::Objects_t::RedFlag:
		return Tile(8, 0, TILE / 2, 20, 6, 16, object, terrain, TILE, 2 * TILE);
	// Border trees
	case Tile::Objects_t::Pine1:
		return Tile(10, 0, -1, -1, 0, 0, object, terrain, TILE, 2 * TILE);
	case Tile::Objects_t::Pine2:
		return Tile(11, 0, -1, -1, 0, 0, object, terrain, 38, 2 * TILE);
	case Tile::Objects_t::WinterTree:
		return Tile(13, 0, -1, -1, 0, 0, object, terrain, 42, 2 * TILE);
	}
}
