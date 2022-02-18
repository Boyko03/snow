#include "NormalMap.h"
#include "TileFactory.h"

NormalMap::NormalMap(int rows, int colls, Difficulty difficulty, Surface& screen) :
	Map(rows, colls, difficulty, screen)
{
	switch (difficulty)
	{
	case Map::Difficulty::Easy:
		flags_counter = 15 * DISTANCE;
		break;
	case Map::Difficulty::Medium:
		flags_counter = 25 * DISTANCE;
		break;
	case Map::Difficulty::Hard:
		flags_counter = 50 * DISTANCE;
		break;
	default:
		break;
	}

	// empty rows at spawn point
	for (int i = 0; i < rows / 3; i++)
		AddRow(true);
	for (int i = rows / 3; i < rows; i++)
		AddRow();
};

void NormalMap::AddRow(bool empty)
{
	vector<Tile*> row;
	// Left border
	for (int i = 0; i < border_width; i++)
		row.push_back(tileFactory->getTile(Tile::Terrains_t::Snow, Tile::Objects_t::TwoTrees));

	// Main map
	if (empty)
		for (int i = 0; i < colls; i++)
			row.push_back(tileFactory->getTile(Tile::Terrains_t::Snow, Tile::Objects_t::None));
	else
		for (int i = 0; i < colls; i++) {
			if (flags_counter >= 0 && flags_counter % DISTANCE == 0) {
				if (i == colls / 3 + 2 && flags_counter & 1)
					row.push_back(tileFactory->getTile(Tile::Terrains_t::Snow, Tile::Objects_t::Flag));
				else if (i == 2 * colls / 3 - 2 && flags_counter % (2 * DISTANCE) == 0)
					row.push_back(tileFactory->getTile(Tile::Terrains_t::Snow, Tile::Objects_t::Flag));
				else
					row.push_back(tileFactory->getTile(Tile::Terrains_t::Snow, Tile::Objects_t::None));
			}
			else
				row.push_back(tileFactory->getTile(Tile::Terrains_t::Snow, Tile::Objects_t::None));
		}

	// Right border
	for (int i = 0; i < border_width; i++)
		row.push_back(tileFactory->getTile(Tile::Terrains_t::Snow, Tile::Objects_t::TwoTrees));

	map.push_back(row);
	flags_counter--;
}

void NormalMap::Move()
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

	// Check if misses flag
	if ((flags_counter + 1) % DISTANCE == 0 && py % TILE < 16) {
		if (!missed_flag) {
			bool left_flag = (flags_counter + 1) & 1 && (player->x - 2 * TILE) / TILE > colls / 3 + 2;
			bool right_flag = (flags_counter + 1) % (2 * DISTANCE) == 0 && (player->x + TILE) / TILE < 2 * colls / 3 + 2;

			if (left_flag || right_flag) {
				player->score = max(player->score - DISTANCE, 0);
				missed_flag = true;
			}
		}
	}
	else
		missed_flag = false;

}

bool NormalMap::CheckPos(int x, int y)
{
	int tx = x / TILE;
	int ty = y / TILE;
	Tile* tile = map.at(ty).at(tx);

	if (tile->object == Tile::Objects_t::None) return true;
	return x % TILE > tile->cx + tile->dx || y % TILE > tile->cy || y % TILE < tile->cy - tile->dy;
}

void NormalMap::Draw()
{
	int r = 0;
	for (auto row : map) {
		int col = 0;
		for (auto tile : row) {
			int x = screen.GetWidth() / 2 - (colls / 2 - col + border_width) * TILE;
			tile->Draw(x, r * TILE - current_position, screen);
			col++;
		}
		r++;
	}

	DrawPlayer();
	DrawHearts();
	PrintScore();
}

void NormalMap::DrawPlayer()
{
	player->Draw(screen);

	int x = screen.GetWidth() / 2 - (colls / 2 + border_width) * TILE;
	int y = player->y - current_position;

	int tx = (player->x - x) / TILE;
	int ty = (player->y + current_position) / TILE;

	Tile* tile = map.at(ty).at(tx);
	Tile::Objects_t None = Tile::Objects_t::None;

	tile = map.at(ty + 1).at(tx);
	if (tile->object != None)
		tile->DrawObjectOnly(x + tx * TILE, y, screen);

	tile = map.at(ty + 1).at(tx + 1);
	if (tile->object != None)
		tile->DrawObjectOnly(x + (tx + 1) * TILE, y, screen);

	tile = map.at(ty + 2).at(tx);
	if (tile->object != None)
		tile->DrawObjectOnly(x + tx * TILE, y + TILE, screen);

	tile = map.at(ty + 2).at(tx + 1);
	if (tile->object != None)
		tile->DrawObjectOnly(x + (tx + 1) * TILE, y + TILE, screen);
}

bool NormalMap::IsWin()
{
	return flags_counter < -25;
}