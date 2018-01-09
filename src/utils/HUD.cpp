#include "HUD.h"
#include <ds_game_ui.h>
#include "tweening.h"
#include "..\Constants.h"
#include "..\GameSettings.h"
#include "utils.h"

HUD::HUD(SpriteBatchBuffer* buffer,  GameContext* context) 
	: _buffer(buffer) , _gameContext(context), _pieces(3), _points(6), _minutes(2), _seconds(2) {
	reset();
	_mode_x_pos[0] = dialog::text_size("ZEN Mode").x;
	_mode_x_pos[1] = dialog::text_size("Timer Mode").x;
}

HUD::~HUD() {
}

// ------------------------------------------------------
// reset
// ------------------------------------------------------
void HUD::reset() {
	if (_gameContext->game_play_mode == GamePlayMode::GPM_ZEN) {
		_timerMode = TimerMode::TM_INC;
		_minutes = 0;
		_seconds = 0;
	}
	else {
		_timerMode = TimerMode::TM_DEC;
		_minutes = 1;
		_seconds = 0;
	}
	_pieces = TOTAL;
	_timer = 0.0f;
	_gameContext->score.points = 0;
	_gameContext->score.minutes = 0;
	_gameContext->score.seconds = 0;
	_gameContext->score.itemsCleared = 0;
	_gameContext->score.highestCombo = 0;
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
	_points = _gameContext->score.points;
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

	dialog::begin();

	char buffer[128];
	ds::vec2 p(120, 725);
	
	sprintf_s(buffer, "%06d", _points);
	dialog::Text(p, buffer, false);
	p.x = 820.0f;	
	sprintf_s(buffer, "%02d:%02d", _minutes, _seconds);
	dialog::Text(p, buffer, false);
	if (_gameContext->game_play_mode == GamePlayMode::GPM_ZEN) {
		dialog::Text(p, "ZEN Mode");
	}
	else {
		dialog::Text(p, "Timer Mode");
	}

	p.y = 40.0f;
	p.x = 512.0f;
	float scale = 1.0f;
	sprintf_s(buffer, "%d", _pieces);	
	if (_piece_timers[3] <= 0.7f) {
		scale = tweening::interpolate(tweening::easeOutElastic, 1.6f, 1.0f, _piece_timers[3], 0.7f);
		_piece_timers[3] += ds::getElapsedSeconds();
	}
	ds::vec2 ts = dialog::text_size(buffer, 1.0f);
	p.x = (1024.0f - ts.x) * 0.5f;
	dialog::Text(p, buffer, false, ds::vec2(scale));

	dialog::end();
}
