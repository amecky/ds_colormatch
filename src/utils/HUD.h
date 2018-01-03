#pragma once
#include <diesel.h>
#include <SpriteBatchBuffer.h>


struct Score {
	int itemsCleared;
	int seconds;
	int minutes;
	int points;
	int totalPoints;
	int highestCombo;
	int piecesLeft;
};

enum TimerMode {
	TM_INC,
	TM_DEC
};

struct GameContext;

class HUD {

public:
	HUD(SpriteBatchBuffer* buffer, GameContext* context, Score* score);
	~HUD();
	void render();
	void tick(float dt);
	void reset(TimerMode timerMode = TimerMode::TM_INC);
	void rebuildScore(bool flash = true);
	int getMinutes() const {
		return _minutes;
	}
	int getSeconds() const {
		return _seconds;
	}
	void setPieces(int pc);
private:
	float _timer;
	float _piece_timers[4];
	int _pieces;
	int _points;
	int _minutes;
	int _seconds;
	SpriteBatchBuffer* _buffer;
	Score* _score;
	TimerMode _timerMode;
	GameContext* _gameContext;
};

