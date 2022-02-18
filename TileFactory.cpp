#include "TileFactory.h"
#include "Tree.h"
#include "Rock.h"
#include "Bush.h"
#include "Log.h"
#include "TwoTrees.h"
#include "TopOfTree.h"
#include "Sign.h"
#include "None.h"
#include "Flag.h"

Tile* TileFactory::getTile(Tile::Terrains_t terrain, Tile::Objects_t object)
{
	switch (object)
	{
	case Tile::Objects_t::Tree:
		return new Tree(terrain);
	case Tile::Objects_t::Rock:
		return new Rock(terrain);
	case Tile::Objects_t::Bush:
		return new Bush(terrain);
	case Tile::Objects_t::Log:
		return new Log(terrain);
	case Tile::Objects_t::TwoTrees:
		return new TwoTrees(terrain);
	case Tile::Objects_t::TopOfTree:
		return new TopOfTree(terrain);
	case Tile::Objects_t::Sign:
		return new Sign(terrain);
	case Tile::Objects_t::None:
		return new None(terrain);
	case Tile::Objects_t::Flag:
		return new Flag(terrain);
	default:
		return nullptr;
	}
}
