#pragma once

struct Highscore {
	int points;
	char name[16];
};

struct HighscoreContext {
	int mode;
	int offset;
	float offsetTimer;
	// 0 - 10 zen / 11 - 20 timer
	Highscore highscores[20];
};

struct GameContext;

void loadHighscores(GameContext* ctx);

void saveHighscores(GameContext* ctx);

int getHighscoreRanking(GameContext* ctx);

void insertHighscore(GameContext* ctx, int rank);