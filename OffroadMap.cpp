#include "OffroadMap.h"
#include "TileFactory.h"

OffroadMap::OffroadMap(int rows, int colls, Difficulty difficulty, Surface& screen) :
	Map(rows, colls, difficulty, screen)
{
	player = new Player(screen.GetWidth() / 2, 4 * TILE);

	// empty rows at spawn point
	for (int i = 0; i < rows / 2; i++)
		AddRow(true);
	for (int i = rows / 2; i < rows; i++)
		AddRow();
};

void OffroadMap::AddRow(bool empty)
{
	vector<Tile> row;
	// Left border
	for (int i = 0; i < border_width; i++)
		row.push_back(tileFactory.getTile(Tile::Terrains_t::Snow, Tile::Objects_t::TwoTrees));

	// Main map
	if (empty)
		for (int i = 0; i < colls; i++)
			row.push_back(tileFactory.getTile(Tile::Terrains_t::Snow, Tile::Objects_t::None));
	else
		for (int i = 0; i < colls; i++)
			row.push_back(GenerateTile(i + border_width));

	// Right border
	for (int i = 0; i < border_width; i++)
		row.push_back(tileFactory.getTile(Tile::Terrains_t::Snow, Tile::Objects_t::TwoTrees));

	map.push_back(row);
}

Tile OffroadMap::GenerateTile(int column) {
	Tile::Terrains_t terrain = Tile::Terrains_t::Snow;
	Tile::Objects_t object = GetRandomObject();

	// Check if tile above is tree in order to draw it correctly
	size_t map_size = map.size();
	Tile* tmp;
	if (map_size > 0 && object == Tile::Objects_t::Tree) {
		Tile::Objects_t above = map[map_size - 1][column].object;
		switch (above)
		{
		case Tile::Objects_t::Tree:
			tmp = &map[map_size - 1][column];
			map[map_size - 1][column] = tileFactory.getTile(tmp->terrain, Tile::Objects_t::TwoTrees);
			break;
		default:
			tmp = &map[map_size - 1][column];
			map[map_size - 1][column] = tileFactory.getTile(tmp->terrain, Tile::Objects_t::TopOfTree);
			break;
		}
	}

	return tileFactory.getTile(terrain, object);
}

Tile::Objects_t OffroadMap::GetRandomObject() {
	// Difficulty:
	// Easy		->		90% None, 10% Tree
	// Medium	->		80% None, 10% Tree, 10% Rock
	// Hard		->		70% None, 10% Tree, 10% Rock, 5% Log, 5% Bush
	int val = rand() % 100;
	int index = 0;

	switch (difficulty)
	{
	case Difficulty::Easy:
		if (val < 10) index = 1;		// Tree
		else index = 0;					// None
		break;
	case Difficulty::Medium:
		if (val < 10) index = 1;		// Tree
		else if (val < 20) index = 2;	// Rock
		else index = 0;					// None
		break;
	case Difficulty::Hard:
		if (val < 10) index = 1;		// Tree
		else if (val < 20) index = 2;	// Rock
		else if (val < 25) index = 3;	// Log
		else if (val < 30) index = 4;	// Bush
		else index = 0;					// None
		break;
	}

	return Tile::Objects_t(index);
}

void OffroadMap::Move(float deltaTime)
{
	if ((current_position += player->speed) >= TILE) {
		current_position -= TILE;
		DeleteRow();
		AddRow();
		player->score++;
	}

	int x = screen.GetWidth() / 2 - (colls / 2 + border_width) * TILE;
	// constraints
	if (player->x < border_width * TILE + x) player->x = (border_width * TILE + x);
	if (player->x + TILE > screen.GetWidth() - (border_width * TILE + x))
		player->x = (screen.GetWidth() - ((border_width + 1) * TILE + x));

	int px = player->x - x + 8;
	int py = player->y + 20 + current_position;

	static int timer = -1;
	if (CheckPos(px, py));
	else if (!player->is_hit) {
		player->is_hit = true;
		player->health--;
		player->speed = min(player->speed, 0.5f);
		timer = 100;
	}
	if (player->is_hit) player->Blink(timer);
	if (player->is_hit && --timer <= -1) player->is_hit = false;
}

bool OffroadMap::CheckPos(int x, int y)
{
	int tx = x / TILE;
	int ty = y / TILE;
	Tile tile = map[ty][tx];

	if (tile.object == Tile::Objects_t::None) return true;
	return x % TILE > tile.cx + tile.dx || y % TILE > tile.cy || y % TILE < tile.cy - tile.dy;
}

void OffroadMap::Draw()
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

	DrawPlayer();
	DrawHearts();
	PrintScore();
}

void OffroadMap::DrawPlayer()
{
	player->Draw(screen);

	int x = screen.GetWidth() / 2 - (colls / 2 + border_width) * TILE;
	int y = player->y - current_position;

	int tx = (player->x - x) / TILE;
	int ty = (player->y + current_position) / TILE;

	Tile* tile = &map[ty][tx];
	Tile::Objects_t None = Tile::Objects_t::None;

	if ((tile->object == Tile::Objects_t::TopOfTree && current_position < TILE / 2 + 8)
		|| tile->object == Tile::Objects_t::TwoTrees)
		tile->DrawObjectOnly(x + tx * TILE, y, screen);

	tile = &map[ty][tx + 1];
	if ((tile->object == Tile::Objects_t::TopOfTree && current_position < TILE / 2 + 8)
		|| tile->object == Tile::Objects_t::TwoTrees)
		tile->DrawObjectOnly(x + (tx + 1) * TILE, y, screen);

	tile = &map[ty + 1][tx];
	if (tile->object != None)
		tile->DrawObjectOnly(x + tx * TILE, y + TILE, screen);

	tile = &map[ty + 1][tx + 1];
	if (tile->object != None)
		tile->DrawObjectOnly(x + (tx + 1) * TILE, y + TILE, screen);
}

void OffroadMap::PrintScore()
{
	Sprite scoreboard = Sprite(new Surface("assets/score.png"), 1);
	scoreboard.Draw(&screen, 560, 0);
	char buff[8];
	sprintf(buff, "%03d", player->score);
	int width = 3;
	int x = (scoreboard.GetWidth() - (int)strlen(buff) * 6 * width) / 2;
	screen.Print(buff, 565 + x, 45, 0xffe8cd57, width);
}