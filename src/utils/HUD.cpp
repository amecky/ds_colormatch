#include "HUD.h"
#include "tweening.h"

HUD::HUD(SpriteBatchBuffer* buffer, RID textureID, Score* score) : _buffer(buffer) , _textureID(textureID) , _score(score) {
	reset();
}

HUD::~HUD() {
}

// ------------------------------------------------------
// reset
// ------------------------------------------------------
void HUD::reset() {
	for (int i = 0; i < 6; ++i) {
		_numbers[i] = 0;
		_scaleFlags[i] = 0;
		_scalingTimers[i] = 0;
	}
	_minutes[0] = 0;
	_minutes[1] = 0;
	_seconds[0] = 0;
	_seconds[1] = 0;
	_timer = 0.0f;
	_score->points = 0;
	_score->minutes = 0;
	_score->seconds = 0;
	_score->itemsCleared = 0;
	_score->highestCombo = 0;
	rebuildScore(false);
}

// ------------------------------------------------------
// tick
// ------------------------------------------------------
void HUD::tick(float dt) {
	_timer += dt;
	if (_timer > 1.0f) {
		_timer -= 1.0f;
		_seconds[1] += 1;
		if (_seconds[1] >= 10) {
			_seconds[1] = 0;
			_seconds[0] += 1;
			if (_seconds[0] >= 6) {
				_seconds[0] = 0;
				_minutes[1] += 1;
				if (_minutes[1] >= 10) {
					_minutes[1] = 0;
					_minutes[0] += 1;
				}
			}
		}
	}
	for (int i = 0; i < 6; ++i) {
		if (_scaleFlags[i] == 1) {
			_scalingTimers[i] += dt;
			if (_scalingTimers[i] >= 0.4f) {
				_scaleFlags[i] = 0;
			}
		}
	}
}

// ------------------------------------------------------
// set number
// ------------------------------------------------------
void HUD::rebuildScore(bool flash) {
	int tmp = _score->points;
	int div = 1;
	for (int i = 0; i < 6; ++i) {
		if (i > 0) {
			div *= 10;
		}
	}
	for (int i = 0; i < 6; ++i) {
		int r = tmp / div;
		if (r != _numbers[i] && flash) {
			_scaleFlags[i] = 1;
			_scalingTimers[i] = 0.0f;
		}
		else {
			_scaleFlags[i] = 0;
		}
		_numbers[i] = r;
		tmp = tmp - r * div;
		div /= 10;
	}
}

// ------------------------------------------------------
// render
// ------------------------------------------------------
void HUD::render() {
	
	ds::vec2 p(103, 30);
	for (int i = 0; i < 6; ++i) {
		float scale = 1.0f;
		if (_scaleFlags[i] == 1) {
			scale = tweening::interpolate(tweening::easeOutElastic, 2.5f, 1.0f, _scalingTimers[i], 0.4f);
		}
		_buffer->add(p, ds::vec4(340 + _numbers[i] * 48, 0, 46, 42), ds::vec2(1.0f,scale));
		p.x += 48.0f;
	}
	p.x = 725.0f;
	for (int i = 0; i < 2; ++i) {
		_buffer->add(p, ds::vec4(340 + _minutes[i] * 48, 0, 46, 42));
		p.x += 48.0f;
	}
	_buffer->add(p, ds::vec4(820, 0, 46, 42));
	p.x += 48.0f;
	for (int i = 0; i < 2; ++i) {
		_buffer->add(p, ds::vec4(340 + _seconds[i] * 48, 0, 46, 42));
		p.x += 48.0f;
	}
}
