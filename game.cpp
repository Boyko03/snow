#include "game.h"
#include "surface.h"
#include <cstdio> //printf
#include <time.h> // time
#include "Map.h"
#include "NormalMap.h"
#include "OffroadMap.h"

#include <Windows.h>

namespace Tmpl8
{
	static Surface home_screen = Surface("assets/home-screen.png");
	static Surface level_mode = Surface("assets/level-mode.png");
	static Surface level_difficulty = Surface("assets/level-difficulty.png");
	// static Surface level_difficulty_offroad = Surface("assets/level-difficulty-off.png");
	static Surface game_over = Surface("assets/gameover.png");
	static Surface winner = Surface("assets/win.png");

	static Sprite btnStart = Sprite(new Surface("assets/buttons/btn_start.png"), 2);
	static Sprite btnSlalom = Sprite(new Surface("assets/buttons/btn_slalom.png"), 2);
	static Sprite btnOffroadHard = Sprite(new Surface("assets/buttons/btn_offroad_hard.png"), 2);

	Map::Difficulty difficulty;

	Map* map = NULL;
	// -----------------------------------------------------------
	// Initialize the application
	// -----------------------------------------------------------
	void Game::Init()
	{
		srand(static_cast<unsigned int>(time(NULL)));
		//btnStart.SetFrame(0);
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
			DrawHomeScreen(); break;
		case STATE::LEVEL_MODE:
			DrawModeSelectScreen();	break;
		case STATE::LEVEL_DIFFICULTY:
			DrawDifficultySelectScreen(); break;
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
		if (button == 1) {
			isMouseDown = false;

			switch (state) {
			case STATE::MAIN_SCREEN: MainScreenHandler(); break;
			case STATE::LEVEL_MODE: LevelModeHandler(); break;
			case STATE::LEVEL_DIFFICULTY: LevelDifficultyHandler(); break;
			case STATE::GAME_OVER: case STATE::WIN: EndgameHandler(); break;
			}
		}
	}

	void Game::MouseDown(int button)
	{
		if (button == 1) {
			isMouseDown = true;

			switch (state) {
			case STATE::MAIN_SCREEN: MainScreenHandler(); break;
			case STATE::LEVEL_MODE: LevelModeHandler(); break;
			case STATE::LEVEL_DIFFICULTY: LevelDifficultyHandler(); break;
			case STATE::GAME_OVER: case STATE::WIN: EndgameHandler(); break;
			}
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
			if (state == STATE::GAME) { player->NormalPosition(); break; }
			else ArrowKeyUpHandler(key); break;
		case 70:	// PrtScr
			ScreenShot(); break;
		case 68:	// f11
			fullscreen = !fullscreen;
			break;
		case 40: case 44: // Enter, Space
			btnSelect = true;
			MouseUp(1);
			selectorIndex = 0;
			break;
		default:
			if (state == STATE::MAIN_SCREEN) EnterName(key);
			break;
		}
	}

	void Game::ArrowKeyUpHandler(int key)
	{
		switch (state)
		{
		case STATE::MAIN_SCREEN:
			EnterName(key);
			break;
		case STATE::LEVEL_MODE:
			if (key == 80) {
				selectorIndex = (selectorIndex - 1) % 2;
				if (selectorIndex < 0) selectorIndex = 1;
			}
			else if (key == 79) selectorIndex = (selectorIndex + 1) % 2;
			break;
		case STATE::LEVEL_DIFFICULTY:
			if (key == 80) {
				selectorIndex = (selectorIndex - 1) % 3;
				if (selectorIndex < 0) selectorIndex = 2;
			}
			else if (key == 79) selectorIndex = (selectorIndex + 1) % 3;
			break;
		case STATE::GAME:
			player->NormalPosition();
			break;
		case STATE::GAME_OVER:
			break;
		case STATE::WIN:
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

		char filename[32];
		sprintf(filename, "screenshots\\screenshot%03d.tga", counter++);
		if (CreateDirectory("screenshots", NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
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
		else {
			// Failed to create directory
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
		if (strcmp(name, "")) {
			if (btnSelect) {
				btnSelect = false;
				state = STATE::LEVEL_MODE;
			}

			if (mx > 263 && mx < 548 && my > 311 && my < 386) {
				if (isMouseDown) btnStart.SetFrame(1);
				else state = STATE::LEVEL_MODE;
			}
		}
		if (!isMouseDown) btnStart.SetFrame(0);
	}

	void Game::LevelModeHandler()
	{
		if (btnSelect) {
			btnSelect = false;
			state = STATE::LEVEL_DIFFICULTY;
			mode = Mode(selectorIndex);
			return;
		}

		if (!isMouseDown) {
			if (IsBackClicked())
				state = STATE::MAIN_SCREEN;
			else if (mx > 162 && mx < 350 && my > 192 && my < 345) {
				state = STATE::LEVEL_DIFFICULTY;
				mode = Mode::Normal;
				selectorIndex = 0;
			}
			else if (mx > 434 && mx < 623 && my > 192 && my < 345) {
				state = STATE::LEVEL_DIFFICULTY;
				mode = Mode::OffRoad;
				selectorIndex = 0;
			}
		}
	}

	void Game::LevelDifficultyHandler()
	{
		if (btnSelect) {
			btnSelect = false;
			state = STATE::GAME;
			difficulty = Map::Difficulty(selectorIndex);
			return;
		}

		if (!isMouseDown && IsBackClicked()) {
			state = STATE::LEVEL_MODE;
			selectorIndex = (int)mode;
		}
		// width: 203, height: 163
		else if (mx > 92 && mx < 295 && my > 168 && my < 331) {
			difficulty = Map::Difficulty::Easy;
			state = STATE::GAME;
		}
		// space between: 17
		else if (mx > 312 && mx < 505 && my > 168 && my < 331) {
			difficulty = Map::Difficulty::Medium;
			state = STATE::GAME;
		}
		else if (mx > 532 && mx < 735 && my > 168 && my < 331) {
			difficulty = Map::Difficulty::Hard;
			state = STATE::GAME;
		}
	}

	void Game::EndgameHandler()
	{
		if (!isMouseDown && my > 340 && my < 400) {
			if (mx > 210 && mx < 366)
				Reset();
			else if (mx > 449 && mx < 606) {
				Reset();
				state = STATE::MAIN_SCREEN;
			}
		}
	}

	bool Game::IsBackClicked()
	{
		return mx > 21 && mx < 100 && my > 21 && my < 100;
	}

	void Game::DrawHomeScreen()
	{
		home_screen.CopyTo(screen, 0, 0);
		btnStart.Draw(screen, 262, 310);

		if (strcmp(name, "")) {
			screen->Box(256, 230, 556, 265, 0x00ff00);
			screen->Print(name, 265, 237, 0x34eb43, 4);
		}
		else {
			screen->Box(256, 230, 556, 265, 0xff0000);
			screen->Print("Name", 265, 237, 0xcccccc, 4);
		}
	}

	void Game::DrawModeSelectScreen()
	{
		level_mode.CopyTo(screen, 0, 0);
		if (selectorIndex == 0)
			btnSlalom.SetFrame(1), btnOffroadHard.SetFrame(0);
		else
			btnSlalom.SetFrame(0), btnOffroadHard.SetFrame(1);

		btnSlalom.Draw(screen, 162, 192);
		btnOffroadHard.Draw(screen, 434, 192);
	}

	void Game::DrawDifficultySelectScreen()
	{
		level_difficulty.CopyTo(screen, 0, 0);
		if (mode == Mode::Normal) {
			for (int i = 0; i < 3; i++) {
				if (i == selectorIndex) btnSlalom.SetFrame(1);
				btnSlalom.Draw(screen, 92 + i * (btnSlalom.GetWidth() + 17), 168);
				btnSlalom.SetFrame(0);
			}
		}
		else {
			static Sprite btnOffroadEasy = Sprite(new Surface("assets/buttons/btn_offroad_easy.png"), 2);
			static Sprite btnOffroadMedium = Sprite(new Surface("assets/buttons/btn_offroad_medium.png"), 2);
			btnOffroadEasy.SetFrame(selectorIndex == 0);
			btnOffroadEasy.Draw(screen, 92, 168);

			btnOffroadMedium.SetFrame(selectorIndex == 1);
			btnOffroadMedium.Draw(screen, 312, 168);

			btnOffroadHard.SetFrame(selectorIndex == 2);
			btnOffroadHard.Draw(screen, 532, 168);
		}
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

	void Game::EnterName(int key)
	{
		if (key == 42) {
			if (name_index > 0) name[--name_index] = 0;
		}
		else if (name_index < 12) {
			char c = GetCharByKey(key);
			if (c) name[name_index++] = c;
		}
	}

	char Game::GetCharByKey(int key)
	{
		if (key >= 4 && key < 30) return key + 'a' - 4;
		if (key == 39) return '0';
		if (key >= 30 && key < 39) return key + '1' - 30;
		
		return 0;
	}
};