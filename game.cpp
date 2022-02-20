#include "game.h"
#include "surface.h"
#include <cstdio> //printf
#include <time.h> // time
#include "Map.h"
#include "NormalMap.h"
#include "OffroadMap.h"

namespace Tmpl8
{
	static Surface home_screen = Surface("assets/home-screen2.png");
	static Surface level_mode = Surface("assets/level-mode.png");
	static Surface level_difficulty_normal = Surface("assets/level-difficulty.png");
	static Surface level_difficulty_offroad = Surface("assets/level-difficulty-off.png");
	static Surface game_over = Surface("assets/gameover.png");
	static Surface winner = Surface("assets/win.png");

	Map::Difficulty difficulty;

	Map* map = NULL;
	// -----------------------------------------------------------
	// Initialize the application
	// -----------------------------------------------------------
	void Game::Init()
	{
		srand(static_cast<unsigned int>(time(NULL)));
	}
	
	// -----------------------------------------------------------
	// Close down application
	// -----------------------------------------------------------
	void Game::Shutdown()
	{
		delete map;
	}

	// -----------------------------------------------------------
	// Main application tick function
	// -----------------------------------------------------------
	void Game::Tick(float deltaTime)
	{
		switch (state)
		{
		case STATE::MAIN_SCREEN:
			home_screen.CopyTo(screen, 0, 0);
			break;
		case STATE::LEVEL_MODE:
			level_mode.CopyTo(screen, 0, 0);
			break;
		case STATE::LEVEL_DIFFICULTY:
			if (mode == Mode::Normal)
				level_difficulty_normal.CopyTo(screen, 0, 0);
			else
				level_difficulty_offroad.CopyTo(screen, 0, 0);
			break;
		case STATE::GAME:
			if (!map) {
				if (mode == Mode::Normal)
					map = new NormalMap(20, 20, difficulty, *screen);
				else if (mode == Mode::OffRoad)
					map = new OffroadMap(20, 20, difficulty, *screen);
			}
			player = map->GetPlayer();
			screen->Clear(0);
			map->Move(deltaTime);
			map->Draw();
			if (player->health == 0) {
				state = STATE::GAME_OVER;
			}
			if (map->IsWin())
				state = STATE::WIN;
			break;
		case STATE::GAME_OVER:
			PrintScore(game_over);
			break;
		case STATE::WIN:
			PrintScore(winner);
			break;
		default:
			break;
		}
	}

	void Game::MouseUp(int button)
	{
		if (button == 1)
			switch (state) {
			case STATE::MAIN_SCREEN: MainScreenHandler(); break;
			case STATE::LEVEL_MODE: LevelModeHandler(); break;
			case STATE::LEVEL_DIFFICULTY: LevelDifficultyHandler(); break;
			case STATE::GAME_OVER: case STATE::WIN: EndgameHandler(); break;
			}
	}

	void Game::MouseMove(int x, int y)
	{
		mx = x, my = y;
	}

	void Game::KeyUp(int key)
	{
		switch (key)
		{
		case 79: case 80: case 81: case 82:
		case 4: case 7: case 26: case 22:
			if (state == STATE::GAME) player->NormalPosition(); break;
		case 70:	// PrtScr
			ScreenShot(); break;
		case 68:	// f11
			fullscreen = !fullscreen;
			break;
		default:
			break;
		}
	}

	void Game::KeyDown(int key)
	{
		// Left		->	80;	 4
		// Right	->	79;  7
		// Up 		->	82; 26
		// Down		->	81; 22
		if (state == STATE::GAME)
			switch (key)
			{
			case 80: case 4:
				player->TurnLeft(); break;
			case 79: case 7:
				player->TurnRight(); break;
			case 82: case 26:
				player->SlowDown(); break;
			case 81: case 22:
				player->Accelerate(); break;
			default:
				break;
			}
	}

	void Game::ScreenShot()
	{
		static int counter = 1;
		int width = screen->GetWidth();
		int height = screen->GetHeight();
		Pixel* buffer = screen->GetBuffer();

		struct TGAHeader
		{
			unsigned char ID = 0, colmapt = 0;			// set both to 0
			unsigned char type = 2;						// set to 2
			unsigned char colmap[5] = {0};				// set all elements to 0
			unsigned short xorigin = 0, yorigin = 0;	// set to 0
			unsigned short width, height;				// put image size here
			unsigned char bpp = 32;						// set to 32
			unsigned char idesc = 0x20;					// set to 0
		};

		TGAHeader header;

		header.width = width;
		header.height = height;

		char filename[18];
		sprintf(filename, "screenshot%03d.tga", counter++);

		FILE* file;
		if (file = fopen(filename, "wb")) {
			fwrite(&header, sizeof(header), 1, file);

			for (int i = 0; i < width * height; i++) {
				fputc(buffer[i] & BlueMask, file);
				fputc((buffer[i] & GreenMask) >> 8, file);
				fputc((buffer[i] & RedMask) >> 16, file);
				fputc(buffer[i] >> 24, file);
			}
			fclose(file);
		}
	}

	void Game::Reset()
	{
		delete map;
		map = NULL;

		state = STATE::GAME;
	}

	void Game::MainScreenHandler()
	{
		if (mx > 263 && mx < 548 && my > 311 && my < 386)
			state = STATE::LEVEL_MODE;
	}

	void Game::LevelModeHandler()
	{
		if (IsBackClicked())
			state = STATE::MAIN_SCREEN;
		else if (mx > 162 && mx < 350 && my > 192 && my < 345) {
			state = STATE::LEVEL_DIFFICULTY;
			mode = Mode::Normal;
		}
		else if (mx > 434 && mx < 623 && my > 192 && my < 345) {
			state = STATE::LEVEL_DIFFICULTY;
			mode = Mode::OffRoad;
		}
	}

	void Game::LevelDifficultyHandler()
	{
		if (IsBackClicked())
			state = STATE::LEVEL_MODE;
		else if (mx > 94 && mx < 273 && my > 178 && my < 320) {
			difficulty = Map::Difficulty::Easy;
			state = STATE::GAME;
		}
		else if (mx > 317 && mx < 497 && my > 178 && my < 320) {
			difficulty = Map::Difficulty::Medium;
			state = STATE::GAME;
		}
		else if (mx > 535 && mx < 714 && my > 178 && my < 320) {
			difficulty = Map::Difficulty::Hard;
			state = STATE::GAME;
		}
	}

	void Game::EndgameHandler()
	{
		if (my > 340 && my < 400)
			if (mx > 210 && mx < 366)
				Reset();
			else if (mx > 449 && mx < 606) {
				Reset();
				state = STATE::MAIN_SCREEN;
			}
	}

	bool Game::IsBackClicked()
	{
		return mx > 21 && mx < 100 && my > 21 && my < 100;
	}

	void Game::PrintScore(Surface& buff)
	{
		for (int i = 2 * TILE; i < screen->GetHeight() - 2 * TILE; i++)
			for (int j = 2 * TILE; j < screen->GetWidth() - 2 * TILE; j++)
				screen->GetBuffer()[i * screen->GetWidth() + j] = buff.GetBuffer()[i * screen->GetWidth() + j];

		if (mode == Mode::OffRoad) {
			char win_buff[16];
			sprintf(win_buff, "Score: %d", player->score);
			screen->Centre(win_buff, 220, 0x00ff00, 4);
		}
		else if (mode == Mode::Normal) {
			char* tmp = dynamic_cast<NormalMap*>(map)->GetTotalTime();
			char time_buff[24];
			sprintf(time_buff, "Time: %s", tmp);
			screen->Centre(time_buff, 220, 0x00ff00, 4);
			delete tmp;
		}
	}
};