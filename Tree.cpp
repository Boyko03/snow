#include "Tree.h"

Tree::Tree(Terrains_t terrain)
{
	this->terrain = terrain;
	object = Objects_t::Tree;
	ox = 0, oy = 1;
	cx = TILE / 2, cy = 25, dx = 4, dy = 7;
}
