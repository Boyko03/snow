#include "Sign.h"

Sign::Sign(Terrains_t terrain)
{
	this->terrain = terrain;
	object = Objects_t::Sign;
	ox = 2, oy = 0;
	cx = TILE / 2, cy = 25, dx = 10, dy = 7;
}
