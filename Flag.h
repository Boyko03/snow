#pragma once
#include "Tile.h"
class Flag : public Tile
{
public:
	static Surface flags;

	Flag(Terrains_t terrain);

	void Draw(int x, int y, Surface& screen) override;
	void DrawObjectOnly(int x, int y, Surface& screen) override;
	void DrawTile(int tx, int ty, Surface& screen, int x, int y) override;

private:
	static int f_width; // flags width
};

