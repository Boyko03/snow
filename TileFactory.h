#pragma once
#include "Tile.h"

class TileFactory
{
public:
	Tile getTile(Tile::Terrains_t terrain, Tile::Objects_t object);
};

