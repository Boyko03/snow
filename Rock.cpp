#include "Rock.h"

Rock::Rock(Terrains_t terrain)
{
	this->terrain = terrain;
	object = Objects_t::Rock;
	ox = 2, oy = 1;
	cx = TILE / 2, cy = 28, dx = 10, dy = 10;
}
