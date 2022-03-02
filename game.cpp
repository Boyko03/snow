#include "game.h"
#include "surface.h"
#include <cstdio> //printf
#include <time.h> // time
#include "Map.h"
#include "NormalMap.h"
#include "OffroadMap.h"
#include "template.h"

#include <Windows.h>
#include <queue>
#include <string>

namespace Tmpl8
{
	static Surface home_screen = Surface("assets/home-screen.png");
	static Surface level_mode = Surface("assets/level-mode.png");
	static Surface level_difficulty = Surface("assets/level-difficulty.png");

	static Surface bg_end = Surface("assets/bg_end.png");
	static Surface ranking = Surface("assets/ranking.png");
	static Sprite top10sprite = Sprite(new Surface("assets/top10.png"), 1);

	static Sprite btnStart = Sprite(new Surface("assets/buttons/btn_start.png"), 2);
	static Sprite btnSlalom = Sprite(new Surface("assets/buttons/btn_slalom.png"), 2);
	static Sprite btnOffroadHard = Sprite(new Surface("assets/buttons/btn_offroad_hard.png"), 2);

	static Sprite btnResume = Sprite(new Surface("assets/buttons/play-circle.png"), 1);
	static Sprite btnRestart = Sprite(new Surface("assets/buttons/arrow-counterclockwise.png"), 1);
	static Sprite btnQuit = Sprite(new Surface("assets/buttons/house-door.png"), 1);

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
		static int p = 0;
		if (PAUSE) {
			static Surface copy = Surface(ScreenWidth, ScreenHeight);
			if (p == 0) screen->CopyTo(&copy, 0, 0), p = 1;
			copy.CopyTo(screen, 0, 0);
			DrawPauseScreen();
		}
		else {
			p = 0;
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
				if (player->health == 0 || map->IsWin()) {
					state = STATE::END_GAME;
				}
				break;
			case STATE::END_GAME:
				PrintScore(bg_end);
				break;
			default:
				break;
			}

			// PrintFPS(deltaTime);
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
			case STATE::GAME:
				if (PAUSE) PauseBtnsHandler(); break;
			case STATE::END_GAME: EndgameHandler(); break;
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
			case STATE::END_GAME: EndgameHandler(); break;
			}
		}
	}

	void Game::MouseMove(int x, int y)
	{
		mx = x, my = y;
	}

	void Game::KeyUp(int key)
	{
		static bool end = false;

		// High priority
		if (key == 70) {	// PrtScr
			ScreenShot();
			return;
		}
		else if (key == 68) {	// f11
			fullscreen = !fullscreen;
			return;
		}
		else if (state == STATE::END_GAME) {
			if (!end) end = true;
			else {
				Reset();
				state = STATE::MAIN_SCREEN;
				end = false;
			}
			return;
		}

		switch (key)
		{
		case 19: // P
			if (state == STATE::GAME)
				PAUSE = !PAUSE;
			else if (state == STATE::MAIN_SCREEN) EnterName(key);
			break;
		case 79: case 80: case 81: case 82:
		case 4: case 7: case 26: case 22:
			ArrowKeyUpHandler(key); break;
		case 40: case 44: // Enter, Space
			if (state != STATE::GAME) {
				btnSelect = true;
				MouseUp(1);
				selectorIndex = 0;
			}
			break;
		case 41:	// Esc
			if (state == STATE::LEVEL_MODE) state = STATE::MAIN_SCREEN;
			else if (state == STATE::LEVEL_DIFFICULTY) state = STATE::LEVEL_MODE;
			else if (state == STATE::GAME) PAUSE = !PAUSE;
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
		/*if (state == STATE::GAME && !PAUSE)
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
			}*/
	}

	void Game::ScreenShot()
	{
		static int counter = 1;
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

		header.width = ScreenWidth;
		header.height = ScreenHeight;

		char filename[32];
		sprintf(filename, "screenshots\\screenshot%03d.tga", counter++);
		if (CreateDirectory("screenshots", NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
			FILE* file;
			if (file = fopen(filename, "wb")) {
				fwrite(&header, sizeof(header), 1, file);

				for (int i = 0; i < ScreenWidth * ScreenHeight; i++) {
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

		makeCopy = true;
		areStatsWritten = false;
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

		if (!isMouseDown) {
			if (IsBackClicked()) {
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
	}

	void Game::PauseBtnsHandler()
	{
		int btnWidth = btnResume.GetWidth();
		int btnHeight = btnResume.GetHeight();

		int X = ScreenWidth / 2 - btnWidth / 2;		// Resume, Restart and Quit are same size
		int Y = ScreenHeight / 2 - btnHeight / 2;

		if (!isMouseDown) {
			if (my > Y && my < Y + btnHeight) {
				// Resume
				if (mx > X - btnWidth - TILE && mx < X - TILE) {
					PAUSE = false;
				}
				// Restart
				else if (mx > X && mx < X + btnWidth) {
					PAUSE = false;
					Reset();
					state = STATE::GAME;
				}
				// Quit
				else if (mx > X + btnWidth + TILE && mx < X + 2 * btnWidth + TILE) {
					PAUSE = false;
					Reset();
					state = STATE::MAIN_SCREEN;
				}
			}
		}
	}

	void Game::EndgameHandler()
	{
		if (!isMouseDown) {
			Reset();
			state = STATE::MAIN_SCREEN;
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

		screen->Centre("Press Esc to exit", ScreenHeight - 2 * TILE, 0xff555555, 2);

		if (strcmp(name, "")) {
			screen->Box(256, 230, 556, 265, 0xff00ff00);
			screen->Print(name, 265, 237, 0xff34eb43, 4);
		}
		else {
			screen->Box(256, 230, 556, 265, 0xffff0000);
			screen->Print("Name", 265, 237, 0xffcccccc, 4);
		}
		static int br = 0;
		int x = 265 + 4 * 6 * strlen(name);
		if (br++ < 50)
			screen->Line(x, 234, x, 261, 0xff010101);
		if (br == 100) br = 0;
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

	void Game::DrawPauseScreen()
	{
		screen->SubBlendBar(0, 0, ScreenWidth, ScreenHeight, 0x21212121);
		int btnWidth = btnResume.GetWidth();
		int btnHeight = btnResume.GetHeight();

		int X = ScreenWidth / 2 - btnWidth / 2;		// Resume, Restart and Quit are same size
		int Y = ScreenHeight / 2 - btnHeight / 2;

		btnResume.Draw(screen, X - btnWidth - TILE, Y);

		btnRestart.Draw(screen, X, Y);

		btnQuit.Draw(screen, X + btnWidth + TILE, Y);
	}

	void Game::PrintScore(Surface& buff)
	{
		static Surface copy = Surface(ScreenWidth, ScreenHeight);
		if (makeCopy) screen->CopyTo(&copy, 0, 0), makeCopy = false;

		copy.CopyTo(screen, 0, 0);
		screen->SubBlendBar(0, 0, ScreenWidth, ScreenHeight, 0x21212121);

		int r_width = ranking.GetWidth();
		int r_height = ranking.GetHeight();
		Pixel* src = ranking.GetBuffer();

		for (int i = 0; i < r_height; i++)
			for (int j = 0; j < r_width; j++)
				if (src[i * r_width + j] != 0)
					screen->GetBuffer()[i * r_width + j] = src[i * r_width + j];

		screen->Centre("Press any key to continue", 14 * TILE + 10, 0xffeeeeee, 2);

		if (mode == Mode::OffRoad) {
			static std::vector<ScoreStat> top10;
			if (!areStatsWritten) {
				top10 = WriteStatsScore();
				areStatsWritten = true;
			}

			int i = 0;
			bool isPlayerResultPrinted = false;
			for (auto stat : top10) {
				unsigned int color = 0xffffff00;
				if (!isPlayerResultPrinted && !strcmp(stat.name, name) && stat.score == player->score)
					color = 0xff00ff00, isPlayerResultPrinted = true;

				int y1 = (4 + i) * TILE + 5, y2 = (5 + i) * TILE - 5;
				screen->BlendBar(3 * TILE, y1, screen->GetWidth() - 3 * TILE, y2, 0xff222222);

				char pos[4] = "";
				sprintf(pos, "%2d.", i + 1);
				screen->Print(pos, 3 * TILE + 10, y1, color, 3);

				screen->Print(stat.name, 5 * TILE + 10, y1, color, 3);

				char score[15];
				sprintf(score, "%*c%*d", 10, ' ', 4, stat.score);
				screen->Print(score, 440, y1, color, 3);
				i++;
			}

			if (isPlayerResultPrinted) {
				top10sprite.Draw(screen, screen->GetWidth() / 2 - top10sprite.GetWidth() / 2, 2 * TILE);
			}
		}
		else if (mode == Mode::Normal) {
			static std::vector<TimeStat> top10;
			if (!areStatsWritten) {
				top10 = WriteStatsTime();
				areStatsWritten = true;
			}

			int i = 0;
			bool isPlayerResultPrinted = false;
			for (auto stat : top10) {
				unsigned int color = 0xffffff00;
				if (!isPlayerResultPrinted &&
					!strcmp(stat.name, name) && (int)stat.score == (int)dynamic_cast<NormalMap*>(map)->GetTotalTimeMs())
					color = 0xff00ff00, isPlayerResultPrinted = true;

				int y1 = (4 + i) * TILE + 5, y2 = (5 + i) * TILE - 5;
				screen->BlendBar(3 * TILE, y1, screen->GetWidth() - 3 * TILE, y2, 0xff222222);

				char pos[4] = "";
				sprintf(pos, "%2d.", i + 1);
				screen->Print(pos, 3 * TILE + 10, y1, color, 3);

				screen->Print(stat.name, 5 * TILE + 10, y1, color, 3);
				char score[16 + 6];
				int total_time = (int)stat.score;
				int minutes = total_time / 60000;
				int seconds = total_time % 60000 / 1000;
				int millis = total_time % 1000 / 10;
				sprintf(score, "%*c%02d:%02d:%02d", 6, ' ', minutes, seconds, millis);
				screen->Print(score, 440, y1, color, 3);
				i++;
			}

			if (isPlayerResultPrinted) {
				top10sprite.Draw(screen, screen->GetWidth() / 2 - top10sprite.GetWidth() / 2, 2 * TILE);
			}
		}
	}

	std::vector<Game::ScoreStat> Game::WriteStatsScore()
	{
		string filename = "stats\\score_";
		if (difficulty == Map::Difficulty::Easy) filename += "easy.txt";
		else if (difficulty == Map::Difficulty::Medium) filename += "medium.txt";
		else filename += "hard.txt";

		class Compare
		{
		public:
			bool operator() (ScoreStat a, ScoreStat b)
			{
				return a.score < b.score;
			}
		};

		vector<ScoreStat> top10;

		ScoreStat new_stat;
		strcpy(new_stat.name, name);
		new_stat.score = player->score;

		if (CreateDirectory("stats", NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
			FILE* file;

			priority_queue<ScoreStat, vector<ScoreStat>, Compare> all_stats;
			all_stats.push(new_stat);
			if (file = fopen(filename.c_str(), "rb")) {
				ScoreStat tmp;
				for (int i = 0; fread(&tmp, sizeof(tmp), 1, file) == 1; i++) {
					all_stats.push(tmp);
				}
				fclose(file);
			}

			if (file = fopen(filename.c_str(), "wb")) {
				while (!all_stats.empty()) {
					fwrite(&all_stats.top(), sizeof(new_stat), 1, file);
					if (top10.size() < 10) top10.push_back(all_stats.top());
					all_stats.pop();
				}
				fclose(file);
			}
		}
		else {
			// Failed to create directory
		}

		return top10;
	}

	std::vector<Game::TimeStat> Game::WriteStatsTime()
	// void Game::WriteStatsTime()
	{
		string filename = "stats\\time_";
		if (difficulty == Map::Difficulty::Easy) filename += "easy.txt";
		else if (difficulty == Map::Difficulty::Medium) filename += "medium.txt";
		else filename += "hard.txt";

		class Compare
		{
		public:
			bool operator() (TimeStat a, TimeStat b)
			{
				return a.score > b.score;
			}
		};

		vector<TimeStat> top10;

		TimeStat new_stat;
		strcpy(new_stat.name, name);
		new_stat.score = dynamic_cast<NormalMap*>(map)->GetTotalTimeMs();

		if (CreateDirectory("stats", NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
			FILE* file;

			priority_queue<TimeStat, vector<TimeStat>, Compare> all_stats;
			if (player->health > 0) all_stats.push(new_stat); // Save result only if player hasn't lost
			if (file = fopen(filename.c_str(), "rb")) {
				TimeStat tmp;
				for (int i = 0; fread(&tmp, sizeof(tmp), 1, file) == 1; i++) {
					all_stats.push(tmp);
				}
				fclose(file);
			}

			if (file = fopen(filename.c_str(), "wb")) {
				while (!all_stats.empty()) {
					fwrite(&all_stats.top(), sizeof(new_stat), 1, file);
					if (top10.size() < 10) top10.push_back(all_stats.top());
					all_stats.pop();
				}
				fclose(file);
			}
		}

		return top10;
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

	void Game::PrintFPS(float deltaTime)
	{
		static float delta = 0.0f, fps = 1 / (deltaTime / 1000.0f);
		static int frames = 0;

		delta += deltaTime;
		frames++;

		if (delta > 1000.0f) {
			fps = frames / (delta / 1000.0f);
			frames = 0;
			delta = 0.0f;
		}

		char FPS[12];
		sprintf(FPS, "FPS: %.2f", fps);
		screen->Print(FPS, 10, 10, 0x2e9121, 2);
	}
};