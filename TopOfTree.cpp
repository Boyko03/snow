#include "TopOfTree.h"

TopOfTree::TopOfTree(Terrains_t terrain)
{
	this->terrain = terrain;
	object = Objects_t::TopOfTree;
	ox = 0, oy = 0;
	cx = cy = -1, dx = dy = 0;
}