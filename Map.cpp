#include "Map.h"
#include "TileFactory.h"
#include "surface.h"

using namespace Tmpl8;

Map::Map(int rows, int colls, Difficulty difficulty, Surface& screen) : rows(rows), colls(colls),
	difficulty(difficulty), screen(screen)
{
	player = new Player(screen.GetWidth() / 2, 128);
	this->tileFactory = new TileFactory();
}

Map::~Map()
{
	delete tileFactory;
	delete player;

	for (int i = 0; i < map.size(); i++)
		DeleteRow();
}

void Map::DeleteRow()
{
	vector<Tile>& row = map.at(0);
	while (!row.empty()) {
		row.pop_back();
	}
	map.erase(map.begin());
}

void Map::DrawHearts()
{
	Sprite heart = Sprite(new Surface("assets/heart.png"), 1);
	for (int i = 0; i < player->health; i++) {
		heart.Draw(&screen, heart.GetWidth() * i + 16, 16);
	}
}

void Map::PrintScore()
{
	Sprite scoreboard = Sprite(new Surface("assets/score.png"), 1);
	scoreboard.Draw(&screen, 560, 0);
	char buff[8];
	sprintf(buff, "%04d", player->score);
	screen.Print(buff, 595, 45, 0xffe8cd57, 3);
}