#include "Map.h"
#include "TileFactory.h"
#include "surface.h"

using namespace Tmpl8;

Map::Map(int rows, int colls, Difficulty difficulty, Surface& screen) : rows(rows), colls(colls),
	difficulty(difficulty), screen(screen) {}

Map::~Map()
{
	delete player;

	for (int i = 0; i < map.size(); i++)
		DeleteRow();
}

void Map::DeleteRow()
{
	vector<Tile>& row = map[0];
	while (!row.empty()) {
		row.pop_back();
	}
	map.erase(map.begin());
}

void Map::DrawTiles()
{
	int r = 0;
	for (auto& row : map) {
		int col = 0;
		for (auto tile : row) {
			int x = screen.GetWidth() / 2 - (colls / 2 - col + border_width) * TILE;
			tile.Draw(x, r * TILE - current_position, screen);
			col++;
		}
		r++;
	}
}

void Map::DrawHearts()
{
	Sprite heart = Sprite(new Surface("assets/heart.png"), 1);
	for (int i = 0; i < player->health; i++) {
		heart.Draw(&screen, heart.GetWidth() * i + 16, 16);
	}
}

void Map::DrawBorder(vector<Tile>& row)
{
	for (int i = 0; i < border_width; i++) {
		int r = rand() % 3;
		row.push_back(tileFactory.getTile(Tile::Terrains_t::Snow, Tile::Objects_t(r + 10))); // 10, 11, 12
	}
}