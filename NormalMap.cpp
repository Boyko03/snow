#include "NormalMap.h"
#include "TileFactory.h"

NormalMap::NormalMap(int rows, int colls, Difficulty difficulty, Surface& screen) :
	Map(rows, colls, difficulty, screen)
{
	player = new Player(((colls + border_width) / 3 + 2) * TILE, 4 * TILE);

	switch (difficulty)
	{
	case Map::Difficulty::Easy:
		DISTANCE = 7;
		flags_counter = 15 * DISTANCE + 2;
		break;
	case Map::Difficulty::Medium:
		DISTANCE = 6;
		flags_counter = 25 * DISTANCE + 2;
		break;
	case Map::Difficulty::Hard:
		DISTANCE = 5;
		flags_counter = 35 * DISTANCE + 2;
		break;
	default:
		break;
	}

	max_flags_shown_per_frame = screen.GetHeight() / TILE / DISTANCE;

	// empty rows at spawn point
	for (int i = 0; i < rows / 3; i++)
		AddRow(true);
	for (int i = rows / 3; i < rows; i++)
		AddRow();
};

void NormalMap::AddRow(bool empty)
{
	if (first_row >= rows) first_row %= rows;
	Tile* row = &map[first_row * width];
	AddBorder();

	// Main map
	if (empty)
		for (int i = 0; i < colls; i++)
			row[border_width + i] = tileFactory.getTile(Tile::Terrains_t::Snow, Tile::Objects_t::None);
	else {
		for (int i = 0; i < colls; i++) {
			if (flags_counter > 0 && flags_counter % DISTANCE == 0) {
				if (i == 2 * colls / 3 - 2 && flags_counter % (2 * DISTANCE) == 0)
					row[border_width + i] = tileFactory.getTile(Tile::Terrains_t::Snow, Tile::Objects_t::RedFlag);
				else if (i == colls / 3 + 2 && flags_counter % (2 * DISTANCE) != 0)
					row[border_width + i] = tileFactory.getTile(Tile::Terrains_t::Snow, Tile::Objects_t::BlueFlag);
				else
					row[border_width + i] = tileFactory.getTile(Tile::Terrains_t::Snow, Tile::Objects_t::None);
			}
			else if (flags_counter % DISTANCE == 0 && !finish_drawn)
				row[border_width + i] = tileFactory.getTile(Tile::Terrains_t::FinishLine, Tile::Objects_t::None);
			else
				row[border_width + i] = tileFactory.getTile(Tile::Terrains_t::Snow, Tile::Objects_t::None);
		}

		flags_counter--;
	}

	// Check for finish
	if (!finish_drawn && flags_counter < 0 && flags_counter % DISTANCE == 0)
		finish_drawn = true;

	first_row++;
}

void NormalMap::Move(float deltaTime)
{
	total_time += deltaTime;

	if ((current_position += player->speed) >= TILE) {
		current_position -= TILE;
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
	if (py % TILE < 16 && !CheckFlag(px, py)) {
		if (!missed_flag) {
			total_time += 10000; // + 10 000 milliseconds
			missed_flag = true;
		}
	}
	else
		missed_flag = false;
}

bool NormalMap::CheckPos(int x, int y)
{
	int tx = x / TILE;
	int ty = (y / TILE + first_row) % rows;
	Tile tile = map[ty * width + tx];

	if (tile.object == Tile::Objects_t::None) {
		tile = map[ty * width + tx + 1];
		if (tile.object == Tile::Objects_t::None) return true;
		return x % TILE < TILE / 2 || (x + 16) % TILE < tile.cx - tile.dx || y % TILE > tile.cy || y % TILE < tile.cy - tile.dy;
	}
	return x % TILE > tile.cx + tile.dx || y % TILE >= tile.cy || y % TILE <= tile.cy - tile.dy;
}

bool NormalMap::CheckFlag(int x, int y)
{
	int tx = x / TILE;
	int ty = (y / TILE + first_row) % rows;

	for (int i = 0; i < colls; i++) {
		Tile& tile = map[ty * width + i];
		if (tile.object == Tile::Objects_t::BlueFlag) {
			return tx < i;
		}
		else if (tile.object == Tile::Objects_t::RedFlag) {
			return tx >= i && CheckPos(x, y);
		}
	}

	return true;
}

void NormalMap::Draw()
{
	DrawBackground();
	DrawForeground();
	DrawPlayer();

	DrawHearts();
	PrintTime();
}

void NormalMap::DrawPlayer()
{
	player->Draw(screen);

	int x = screen.GetWidth() / 2 - (colls / 2 + border_width) * TILE;
	int y = player->y - current_position;

	int tx = (player->x - x) / TILE;
	int ty = (player->y + current_position) / TILE;

	Tile* tile = &map[((ty + first_row) % rows) * width + tx];
	Tile::Objects_t None = Tile::Objects_t::None;

	tile = &map[((ty + first_row + 1) % rows) * width + tx];
	if (tile->object != None && tx >= border_width)
		tile->DrawObjectOnly(x + tx * TILE, y, screen);

	tile = &map[((ty + first_row + 1) % rows) * width  + tx + 1];
	if (tile->object != None && tx + 1 < colls + border_width)
		tile->DrawObjectOnly(x + (tx + 1) * TILE, y, screen);

	tile = &map[((ty + first_row + 2) % rows) * width + tx];
	if (tile->object != None && tx >= border_width)
		tile->DrawObjectOnly(x + tx * TILE, y + TILE, screen);

	tile = &map[((ty + first_row + 2) % rows) * width + tx + 1];
	if (tile->object != None && tx + 1 < colls + border_width)
		tile->DrawObjectOnly(x + (tx + 1) * TILE, y + TILE, screen);
}

bool NormalMap::IsWin()
{
	return flags_counter < -max_flags_shown_per_frame * DISTANCE - DISTANCE / 2 + 2;
}

void NormalMap::PrintTime()
{
	Sprite scoreboard = Sprite(new Surface("assets/timeboard.png"), 1);
	scoreboard.Draw(&screen, 535, 0);
	char* buff = GetTotalTime();
	int width = 3;
	int x = (scoreboard.GetWidth() - (int)strlen(buff) * 6 * width) / 2;
	screen.Print(buff, 540 + x, 45, 0xffe8cd57, width);
	delete buff;
}

char* NormalMap::GetTotalTime()
{
	char* buff = new char[16];

	int minutes = (int)total_time / 60000;
	int seconds = (int)total_time % 60000 / 1000;
	int millis = (int)total_time % 1000 / 10;
	sprintf(buff, "%02d:%02d:%02d", minutes, seconds, millis);

	return buff;
}