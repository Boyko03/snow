#include "Map.h"
#include "TileFactory.h"
#include "surface.h"

using namespace Tmpl8;

Map::Map(int rows, int colls, Difficulty difficulty, Surface& screen) : rows(rows), colls(colls),
	difficulty(difficulty), screen(screen)
{
	for (int i = 0; i < rows; i++) {
		vector<Tile> row;
		map.push_back(row);
	}
}

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

void Map::DrawBackground()
{
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < colls + 2 * border_width; c++) {
			int x = screen.GetWidth() / 2 - (colls / 2 - c + border_width) * TILE;
			map[(first_row + r) % rows][c].DrawBackground(x, r * TILE - current_position, screen);
		}
	}
	/*int r = 0;
	for (auto& row : map) {
		int col = 0;
		for (auto tile : row) {
			int x = screen.GetWidth() / 2 - (colls / 2 - col + border_width) * TILE;
			tile.DrawBackground(x, r * TILE - current_position, screen);
			col++;
		}
		r++;
	}*/
}

void Map::DrawForeground()
{
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < colls + 2 * border_width; c++) {
			int x = screen.GetWidth() / 2 - (colls / 2 - c + border_width) * TILE;
			map[(first_row + r) % rows][c].DrawForegound(x, r * TILE - current_position, screen);
		}
	}
	/*int r = 0;
	for (auto& row : map) {
		int col = 0;
		for (auto tile : row) {
			int x = screen.GetWidth() / 2 - (colls / 2 - col + border_width) * TILE;
			tile.DrawForegound(x, r * TILE - current_position, screen);
			col++;
		}
		r++;
	}*/
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
	if (row.size() < colls) row.resize(colls + 2 * border_width);
	// Left border
	for (int i = 0; i < border_width; i++) {
		int r = rand() % 4;
		if (r == 0) row[i] = tileFactory.getTile(Tile::Terrains_t::Snow, Tile::Objects_t::None);
		else row[i] = tileFactory.getTile(Tile::Terrains_t::Snow, Tile::Objects_t(r + 9)); // 10, 11, 12
	}

	// Right border
	for (int i = 0; i < border_width; i++) {
		int r = rand() % 4;
		if (r == 0) row[border_width + colls + i] = tileFactory.getTile(Tile::Terrains_t::Snow, Tile::Objects_t::None);
		else row[border_width + colls + i] = tileFactory.getTile(Tile::Terrains_t::Snow, Tile::Objects_t(r + 9)); // 10, 11, 12
	}
}