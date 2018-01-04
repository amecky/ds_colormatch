#pragma once
#include <diesel.h>
#include <SpriteBatchBuffer.h>

enum TimerMode {
	TM_INC,
	TM_DEC
};

struct GameContext;

class HUD {

public:
	HUD(SpriteBatchBuffer* buffer, GameContext* context);
	~HUD();
	void render();
	void tick(float dt);
	void reset();
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
	float _mode_x_pos[2];
	SpriteBatchBuffer* _buffer;
	TimerMode _timerMode;
	GameContext* _gameContext;
};

