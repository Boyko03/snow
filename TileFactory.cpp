#include "TileFactory.h"
#include "Tile.h"

Tile TileFactory::getTile(Tile::Terrains_t terrain, Tile::Objects_t object)
{
	// Tile(int ox, int oy, int cx, int cy, int dx, int dy, Objects_t object);
	switch (object)
	{
	case Tile::Objects_t::Tree:
		return Tile(0, 1, TILE / 2, 25, 4, 7, object, terrain);
	case Tile::Objects_t::Rock:
		return Tile(2, 1, TILE / 2, 28, 10, 10, object, terrain);
	case Tile::Objects_t::Bush:
		return Tile(1, 1, TILE / 2, 28, 12, 16, object, terrain);
	case Tile::Objects_t::Log:
		return Tile(3, 0, TILE / 2, 22, 14, 6, object, terrain);
	case Tile::Objects_t::TwoTrees:
		return Tile(1, 0, TILE / 2, 25, 4, 7, object, terrain);
	case Tile::Objects_t::TopOfTree:
		return Tile(0, 0, -1, -1, 0, 0, object, terrain);
	case Tile::Objects_t::Sign:
		return Tile(2, 0, TILE / 2, 25, 10, 7, object, terrain);
	case Tile::Objects_t::None:
		return Tile(0, 1, TILE / 2, 25, 4, 7, object, terrain);
	case Tile::Objects_t::Flag:
		return Tile(4, 0, TILE / 2, 20, 6, 16, object, terrain, TILE, 2 * TILE);
	}
}
