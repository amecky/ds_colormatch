#pragma once
#include <diesel.h>
#include "utils\highscore.h"
// ---------------------------------------------------------------
// Game play modes
// ---------------------------------------------------------------
struct GamePlayMode {

	enum Enum {
		GPM_ZEN,
		GPM_TIMER
	};
};

// ---------------------------------------------------------------
// Game modes
// ---------------------------------------------------------------
struct GameMode {
	enum Enum {
		GM_MENU,
		GM_GAME_MODE,
		GM_RUNNING,
		GM_GAMEOVER,
		GM_HIGHSCORES,
		GM_NEW_HIGHSCORE
	};
};

struct GameSettings {
	float prepareTTL;
	float messageScale;
	float scaleUpMinTTL;
	float scaleUpMaxTTL;
	float flashTTL;
	float droppingTTL;
	float wiggleTTL;
	float wiggleScale;
	float clearMinTTL;
	float clearMaxTTL;
	float highlightTime;
	float logoSlideTTL;
	float menuTTL;	
	float higschoreSwitchTTL;
};

struct Score {
	int itemsCleared;
	int seconds;
	int minutes;
	int points;
	int totalPoints;
	int highestCombo;
	int piecesLeft;
};

class Board;
class HUD;

struct GameContext {
	ds::Color colors[8];
	GamePlayMode::Enum game_play_mode;
	GameSettings settings;
	Board* board;
	Score score;
	HUD* hud;
	GameMode::Enum mode;
	int moves;
	float menuTimer;
	bool running;
	bool pressed;
	char user[16];
	HighscoreContext highscoreContext;
	int ranking;
};

