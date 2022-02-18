#include "TwoTrees.h"

TwoTrees::TwoTrees(Terrains_t terrain)
{
	this->terrain = terrain;
	object = Objects_t::TwoTrees;
	ox = 1, oy = 0;
	cx = TILE / 2, cy = 25, dx = 4, dy = 7;
}
