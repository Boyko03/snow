#include "OffroadMap.h"
#include "TileFactory.h"

OffroadMap::OffroadMap(int rows, int colls, Difficulty difficulty, Surface& screen) :
	Map(rows, colls, difficulty, screen)
{
	player = new Player((float)screen.GetWidth() / 2, 4 * TILE);

	// empty rows at spawn point
	for (int i = 0; i < rows / 2; i++)
		AddRow(true);
	for (int i = rows / 2; i < rows; i++)
		AddRow();
};

void OffroadMap::AddRow(bool empty)
{
	if (first_row >= rows) first_row %= rows;
	Tile* row = &map[first_row * width];
	AddBorder();

	// Main map
	if (empty)
		for (int i = 0; i < colls; i++)
			row[border_width + i] = tileFactory.getTile(Tile::Terrains_t::Snow, Tile::Objects_t::None);
	else
		for (int i = 0; i < colls; i++)
			row[border_width + i] = GenerateTile(i + border_width);

	first_row++;
}

Tile OffroadMap::GenerateTile(int column) {
	Tile::Terrains_t terrain = Tile::Terrains_t::Snow;
	Tile::Objects_t object = GetRandomObject();

	// Check if tile above is tree in order to draw it correctly
	Tile* tmp;
	int above_row = first_row > 0 ? first_row - 1 : rows - 1;
	bool isAbove = map[above_row * width + column].ox >= 0;
	if (isAbove && object == Tile::Objects_t::Tree) {
		Tile::Objects_t above = map[above_row * width + column].object;
		switch (above)
		{
		case Tile::Objects_t::Tree:
			tmp = &map[above_row * width + column];
			map[above_row * width + column] = tileFactory.getTile(tmp->terrain, Tile::Objects_t::TwoTrees);
			break;
		default:
			tmp = &map[above_row * width + column];
			map[above_row * width + column] = tileFactory.getTile(tmp->terrain, Tile::Objects_t::TopOfTree);
			break;
		}
	}

	return tileFactory.getTile(terrain, object);
}

Tile::Objects_t OffroadMap::GetRandomObject() {
	// Difficulty:
	// Easy		->		89% None, 10% Tree, 1% Powerup
	// Medium	->		79% None, 10% Tree, 10% Rock, 1% Powerup
	// Hard		->		69% None, 10% Tree, 10% Rock, 5% Log, 5% Bush, 1% Powerup
	int val = rand() % 100;
	int index = 0;

	switch (difficulty)
	{
	case Difficulty::Easy:
		if (val < 10) index = 1;		// Tree
		else if (val != 99) index = 0;	// None
		else index = GetRandomPowerup();// Powerup
		break;
	case Difficulty::Medium:
		if (val < 10) index = 1;		// Tree
		else if (val < 20) index = 2;	// Rock
		else if (val != 99) index = 0;	// None
		else index = GetRandomPowerup();// Powerup
		break;
	case Difficulty::Hard:
		if (val < 10) index = 1;		// Tree
		else if (val < 20) index = 2;	// Rock
		else if (val < 25) index = 3;	// Log
		else if (val < 30) index = 4;	// Bush
		else if (val != 99) index = 0;	// None
		else index = GetRandomPowerup();// Powerup
		break;
	}

	return Tile::Objects_t(index);
}

int OffroadMap::GetRandomPowerup()
{
	int index, p = rand() % 3;

	switch (p) {
	case 0:
		if (player->health < 3)
			index = 8;
		else index = 0;
		break;
	case 1:
		index = 9;
		break;
	default:
		index = 0;
	}

	return index;
}

void OffroadMap::Move(float deltaTime)
{
	if ((current_position += player->speed) >= TILE) {
		current_position -= TILE;
		AddRow();
		player->score++;
	}

	int x = screen.GetWidth() / 2 - (colls / 2 + border_width) * TILE;
	// constraints
	if (player->x < border_width * TILE + x) player->x = (float)(border_width * TILE + x);
	if (player->x + TILE > screen.GetWidth() - (border_width * TILE + x))
		player->x = (float)(screen.GetWidth() - ((border_width + 1) * TILE + x));

	int px = (int)player->x - x + 8;
	int py = (int)(player->y + current_position) + 20;

	static int hit_timer = -1, shield_timer = -1;
	if (CheckPos(px, py) || player->shield_timer > -1);
	else if (!player->is_hit) {
		player->is_hit = true;
		player->health--;
		player->speed = min(player->speed, 0.5f);
		hit_timer = 100;
	}
	if (player->is_hit) player->Blink(hit_timer);
	if (player->is_hit && --hit_timer <= -1) player->is_hit = false;
}

bool OffroadMap::CheckPos(int x, int y)
{
	int tx = x / TILE;
	int ty = (y / TILE + first_row) % rows;
	Tile* tile = &map[ty * width + tx];

	if (tile->object == Tile::Objects_t::None) {
		tile = &map[ty * width + tx + 1];
		if (tile->object == Tile::Objects_t::None) return true;
		else if (CheckForPowerups(tile)) return true;
		return x % TILE < TILE / 2 || (x + 16) % TILE < tile->cx - tile->dx || y % TILE > tile->cy || y % TILE < tile->cy - tile->dy;
	}
	else if (CheckForPowerups(tile)) return true;
	return x % TILE > tile->cx + tile->dx || y % TILE > tile->cy || y % TILE < tile->cy - tile->dy;
}

bool OffroadMap::CheckForPowerups(Tile* tile)
{
	if (tile->object == Tile::Objects_t::Heart) {
		player->health += 1;
		*tile = tileFactory.getTile(tile->terrain, Tile::Objects_t::None);
		return true;
	}
	else if (tile->object == Tile::Objects_t::Shield) {
		player->shield_timer = 300;
		*tile = tileFactory.getTile(tile->terrain, Tile::Objects_t::None);
		return true;
	}

	return false;
}

void OffroadMap::Draw()
{
	DrawBackground();
	DrawForeground();
	DrawPlayer();

	DrawHearts();
	PrintScore();
	PrintShieldTimer();
}

void OffroadMap::DrawPlayer()
{
	player->Draw(screen);

	int x = screen.GetWidth() / 2 - (colls / 2 + border_width) * TILE;	// partially off-screen border
	int y = (int)(player->y - current_position);						// screen y

	int tx = (int)((player->x - x) / TILE);								// map x
	int ty = (int)((player->y + current_position) / TILE);				// map y

	Tile::Objects_t None = Tile::Objects_t::None;

	// Current Tile
	Tile* tile = &map[((ty + first_row) % rows) * width + tx];
	if ((tile->object == Tile::Objects_t::TopOfTree && current_position < TILE / 2 + 8)
		|| tile->object == Tile::Objects_t::TwoTrees && tx >= border_width)
		tile->DrawObjectOnly(x + tx * TILE, y, screen);

	// Right
	tile = &map[((ty + first_row) % rows) * width + tx + 1];
	if ((tile->object == Tile::Objects_t::TopOfTree && current_position < TILE / 2 + 8)
		|| tile->object == Tile::Objects_t::TwoTrees && tx + 1 < colls + border_width)
		tile->DrawObjectOnly(x + (tx + 1) * TILE, y, screen);

	// Bottom
	tile = &map[((ty + first_row + 1) % rows) * width + tx];
	if (tile->object != None && tx >= border_width)
		tile->DrawObjectOnly(x + tx * TILE, y + TILE, screen);

	// Bottom Right
	tile = &map[((ty + first_row + 1) % rows) * width + tx + 1];
	if (tile->object != None && tx + 1 < colls + border_width)
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

void OffroadMap::PrintShieldTimer()
{
	if (player->shield_timer > -1) {
		static Sprite shield = Sprite(new Surface("assets/shield_sprite.png"), 1);
		shield.Draw(&screen, 16, 48);
		char timer[5] = "";
		sprintf(timer, "%d:%02d", player->shield_timer / 100, player->shield_timer % 100);
		screen.Print(timer, shield.GetWidth() + 32, 52, 0xff00a2e8, 4);
	}
}