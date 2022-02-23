#include "Player.h"
#pragma once

namespace Tmpl8 {

class Surface;
class Game
{
public:
	bool fullscreen = false;

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
private:
	Player* player;
	Surface* screen;

	int mx;
	int my;

	bool SELECT = false;

	enum class STATE {
		MAIN_SCREEN,
		LEVEL_MODE,
		LEVEL_DIFFICULTY,
		GAME,
		GAME_OVER,
		WIN
	} state = STATE::MAIN_SCREEN;

	enum class Mode {
		Normal,
		OffRoad
	} mode;

	bool isMouseDown = false;
	bool btnSelect = false;
	int selectorIndex = 0;

	void MainScreenHandler();
	void LevelModeHandler();
	void LevelDifficultyHandler();
	void EndgameHandler();
	bool IsBackClicked();

	void ArrowKeyUpHandler(int key);

	void DrawHomeScreen();
	void DrawModeSelectScreen();
	void DrawDifficultySelectScreen();

	// Prints score during endgame state
	void PrintScore(Surface& buff);
};

}; // namespace Tmpl8