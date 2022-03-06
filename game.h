#pragma once

#include "Player.h"

#include <vector>

namespace Tmpl8 {

class Surface;
class Game
{
public:
	bool fullscreen = false;

	enum class STATE {
		MAIN_SCREEN,
		LEVEL_MODE,
		LEVEL_DIFFICULTY,
		GAME,
		END_GAME
	};

private:
	Player* player;
	Surface* screen;

	int mx;
	int my;

	bool SELECT = false;
	bool PAUSE = false;

	// STATE: END_GAME
	int gameOverKey = -1, counter = 0;
	bool isGameOver = true, gameOverKeyUp = false;

	char name[32] = "";
	int name_index = 0;

	STATE state = STATE::MAIN_SCREEN;

	enum class Mode {
		Normal,
		OffRoad
	} mode;

	struct ScoreStat {
		char name[13];
		int score;
	};

	struct TimeStat {
		char name[13];
		float score;
	};

	bool isMouseDown = false;
	bool btnSelect = false;
	int selectorIndex = 0;

	bool areStatsWritten = false;
	bool makeCopy = true;

public:
	void SetTarget( Surface* surface ) { screen = surface; }
	void Init();
	void Shutdown();
	void Tick( float deltaTime );
	void MouseUp(int button);
	void MouseDown( int button );
	void MouseMove(int x, int y);
	void KeyUp(int key);
	void KeyDown(int key);
	void ScreenShot();
	void Reset();

	STATE GetState() { return state; }

private:
	void MainScreenHandler();
	void LevelModeHandler();
	void LevelDifficultyHandler();
	void PauseBtnsHandler();
	void EndgameHandler();
	bool IsBackClicked();

	void ArrowKeyUpHandler(int key);

	void DrawHomeScreen();
	void DrawModeSelectScreen();
	void DrawDifficultySelectScreen();

	void DrawPauseScreen();

	// Prints score during endgame state
	void PrintScore();

	std::vector<ScoreStat> WriteStatsScore();
	std::vector<TimeStat> WriteStatsTime();

	void EnterName(int key);
	char GetCharByKey(int key);

	void PrintFPS(float deltaTime);
};

}; // namespace Tmpl8