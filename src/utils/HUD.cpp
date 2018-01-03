#include "HUD.h"
#include "tweening.h"
#include "..\Constants.h"
#include "..\GameSettings.h"
#include "utils.h"

HUD::HUD(SpriteBatchBuffer* buffer,  GameContext* context, Score* score) 
	: _buffer(buffer) , _gameContext(context), _score(score) , _pieces(3), _points(6), _minutes(2), _seconds(2) {
	reset();
}

HUD::~HUD() {
}

// ------------------------------------------------------
// reset
// ------------------------------------------------------
void HUD::reset(TimerMode timerMode) {
	_timerMode = timerMode;
	if (timerMode == TimerMode::TM_INC) {
		_minutes = 0;
		_seconds = 0;
	}
	else {
		_minutes = 1;
		_seconds = 0;
	}
	_pieces = TOTAL;
	_timer = 0.0f;
	_score->points = 0;
	_score->minutes = 0;
	_score->seconds = 0;
	_score->itemsCleared = 0;
	_score->highestCombo = 0;
	rebuildScore(false);
	for (int i = 0; i < 4; ++i) {
		_piece_timers[i] = 100.0f;
	}
	setPieces(TOTAL);
}

// ------------------------------------------------------
// tick
// ------------------------------------------------------
void HUD::tick(float dt) {
	_timer += dt;
	if (_timer > 1.0f) {
		_timer -= 1.0f;
		if (_timerMode == TimerMode::TM_INC) {
			++_seconds;
			if (_seconds >= 60) {
				_seconds = 0;
				++_minutes;
			}
		}
		else {
			--_seconds;
			if (_seconds < 0) {
				_seconds = 59;
				--_minutes;
			}
		}
	}
}

// ------------------------------------------------------
// set number
// ------------------------------------------------------
void HUD::rebuildScore(bool flash) {
	_points = _score->points;
}

// ------------------------------------------------------
// set pieces
// ------------------------------------------------------
void HUD::setPieces(int pc) {
	if (pc < 0) {
		pc = 0;
	}
	_pieces = pc;
	_piece_timers[3] = 0.0f;
}

// ------------------------------------------------------
// render
// ------------------------------------------------------
void HUD::render() {
	char buffer[128];
	ds::vec2 p(160, 720);
	p.x += 20.0f;
	sprintf_s(buffer, "%06d", _points);
	font::renderText(p, buffer, _buffer, 1.0f);

	p.y = 40.0f;
	p.x = 490.0f;
	p.x = 512.0f;

	float scale = 1.0f;
	
	sprintf_s(buffer, "%d", _pieces);
	if (_piece_timers[3] <= 0.7f) {
		scale = tweening::interpolate(tweening::easeOutElastic, 1.6f, 1.0f, _piece_timers[3], 0.7f);
		_piece_timers[3] += ds::getElapsedSeconds();
	}
	font::renderText(p, buffer, _buffer, scale);

	p.y = 720.0f;
	p.x = 710.0f;
	p.x = 820.0f;
	p.x = 725.0f;
	sprintf_s(buffer, "%02d:%02d", _minutes,_seconds);
	font::renderText(p, buffer, _buffer, 1.0f);
}
