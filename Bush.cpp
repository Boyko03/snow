#include "Bush.h"

Bush::Bush(Terrains_t terrain)
{
	this->terrain = terrain;
	object = Objects_t::Bush;
	ox = 1, oy = 1;
	cx = TILE / 2, cy = 28, dx = 12, dy = 16;
}
