#include "Log.h"

Log::Log(Terrains_t terrain)
{
	this->terrain = terrain;
	object = Objects_t::Log;
	ox = 3, oy = 0;
	cx = TILE / 2, cy = 22, dx = 14, dy = 6;
}
