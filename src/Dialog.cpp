#include "Dialog.h"
#include <vector>
#include "Constants.h"
#include <stdarg.h>
#include "utils\utils.h"
#include "utils\tweening.h"
#include "GameSettings.h"
#include "utils\HUD.h"

namespace dialog {

	struct DrawCall {
		ds::vec2 pos;
		ds::vec4 rect;
	};

	const int MAX_DRAW_CALLS = 1024;

	struct GUIContext {
		SpriteBatchBuffer* buffer;
		DrawCall calls[MAX_DRAW_CALLS];
		int num_calls;
		bool clicked;
		bool buttonPressed;
	};

	static GUIContext _guiCtx;

	// -------------------------------------------------------
	// check if mouse cursor is inside box
	// -------------------------------------------------------
	static bool isCursorInside(const ds::vec2& p, const ds::vec2& dim) {
		ds::vec2 mp = ds::getMousePosition();
		if (mp.x < (p.x - dim.x * 0.5f)) {
			return false;
		}
		if (mp.x >(p.x + dim.x * 0.5f)) {
			return false;
		}
		if (mp.y < (p.y - dim.y * 0.5f)) {
			return false;
		}
		if (mp.y >(p.y + dim.y * 0.5f)) {
			return false;
		}
		return true;
	}

	// -------------------------------------------------------
	// handle mouse interaction
	// -------------------------------------------------------
	static bool isClicked(const ds::vec2& pos, const ds::vec2& size) {
		if (_guiCtx.clicked) {
			ds::vec2 p = pos;
			if (_guiCtx.clicked && isCursorInside(p, size)) {
				return true;
			}
		}
		return false;
	}

	// ---------------------------------------------------------------
	// init
	// ---------------------------------------------------------------
	void init(SpriteBatchBuffer* buffer) {
		_guiCtx.buffer = buffer;
		_guiCtx.clicked = false;
		_guiCtx.buttonPressed = false;
		_guiCtx.num_calls = 0;
	}

	// ---------------------------------------------------------------
	// begin
	// ---------------------------------------------------------------
	void begin() {
		_guiCtx.num_calls = 0;
		if (_guiCtx.clicked) {
			_guiCtx.clicked = false;
		}
		if (ds::isMouseButtonPressed(0)) {
			_guiCtx.buttonPressed = true;
		}
		else {
			if (_guiCtx.buttonPressed) {
				_guiCtx.clicked = true;
			}
			_guiCtx.buttonPressed = false;
		}
	}

	// ---------------------------------------------------------------
	// button
	// ---------------------------------------------------------------
	bool Button(const ds::vec2& pos, const ds::vec4& rect) {
		if (_guiCtx.num_calls < MAX_DRAW_CALLS) {
			DrawCall& call = _guiCtx.calls[_guiCtx.num_calls++];
			call.pos = pos;
			call.rect = rect;
			ds::vec2 dim = ds::vec2(rect.z, rect.w);
			return isClicked(pos, dim);
		}
		return false;
	}

	// ---------------------------------------------------------------
	// image
	// ---------------------------------------------------------------
	void Image(const ds::vec2& pos, const ds::vec4& rect) {
		if (_guiCtx.num_calls < MAX_DRAW_CALLS) {
			DrawCall& call = _guiCtx.calls[_guiCtx.num_calls++];
			call.pos = pos;
			call.rect = rect;
		}
	}

	// ---------------------------------------------------------------
	// text
	// ---------------------------------------------------------------
	void Text(const ds::vec2& pos, const char* text, bool centered) {
		int l = strlen(text);
		ds::vec2 p = pos;
		ds::vec2 size = font::textSize(text);
		if (centered) {
			p.x = (ds::getScreenWidth() - size.x) * 0.5f;
		}
		float lw = 0.0f;
		for (int i = 0; i < l; ++i) {
			ds::vec4 r = font::get_rect(text[i]);
			p.x += lw * 0.5f + r.z * 0.5f;
			if (_guiCtx.num_calls < MAX_DRAW_CALLS) {
				DrawCall& call = _guiCtx.calls[_guiCtx.num_calls++];
				call.pos = p;
				call.rect = r;
			}
			lw = r.z;
		}
	}

	// ---------------------------------------------------------------
	// formatted text
	// ---------------------------------------------------------------
	void FormattedText(const ds::vec2& pos, bool centered, const char* fmt, ...) {
		char buffer[1024];
		va_list args;
		va_start(args, fmt);
		vsprintf(buffer, fmt, args);
		ds::vec2 size = font::textSize(buffer);
		ds::vec2 p = pos;
		if (centered) {
			p.x = (ds::getScreenWidth() - size.x) * 0.5f;
		}
		Text(p, buffer, centered);
		va_end(args);		
	}

	// ---------------------------------------------------------------
	// input
	// ---------------------------------------------------------------
	void Input(const ds::vec2& pos, char* text, int maxLength) {
		size_t len = strlen(text);
		for (int i = 0; i < ds::getNumInputKeys(); ++i) {
			const ds::InputKey& key = ds::getInputKey(i);
			if (key.type == ds::IKT_SYSTEM) {
				if (key.value == ds::SpecialKeys::DSKEY_Backspace) {
					--len;
					text[len] = '\0';
				}
			}
			else {
				if ((key.value >= 32 && key.value < 128)) {
					if (len < maxLength) {
						text[len] = key.value;
						++len;
					}
				}
			}
			text[len] = '\0';
		}
		Text(pos, text, true);
	}

	// ---------------------------------------------------------------
	// end
	// ---------------------------------------------------------------
	void end() {
		for (size_t i = 0; i < _guiCtx.num_calls; ++i) {
			const DrawCall& call = _guiCtx.calls[i];
			_guiCtx.buffer->add(call.pos, call.rect);
		}
	}
}

enum FloatInDirection {
	FID_LEFT,
	FID_RIGHT
};

static int floatButton(float time, float ttl, FloatInDirection dir) {
	if (time <= ttl) {
		if (dir == FloatInDirection::FID_LEFT) {
			return tweening::interpolate(tweening::easeOutElastic, -200, 512, time, ttl);
		}
		else {
			return tweening::interpolate(tweening::easeOutElastic, 1020, 512, time, ttl);
		}
	}
	return 512;
}

const static int LOGO_Y_POS = 600;

int showHighscoresMenu(GameContext* ctx, float time, float ttl) {
	HighscoreContext& hctx = ctx->highscoreContext;
	int ret = 0;
	dialog::begin();
	int dy = LOGO_Y_POS;
	if (time <= ttl) {
		dy = tweening::interpolate(tweening::easeOutElastic, 1000, LOGO_Y_POS, time, ttl);
	}
	dialog::Image(ds::vec2(512, dy), ds::vec4(200, 560, 560, 55));
	int dx = floatButton(time, ttl, FloatInDirection::FID_LEFT);
	dialog::Image(ds::vec2(518, 500), ds::vec4(540, 160, 860, 36));
	if (hctx.mode == 0) {
		dialog::Text(ds::vec2(512, 500), "ZEN Modus");
	}
	else {
		dialog::Text(ds::vec2(512, 500), "Timer Modus");
	}
	hctx.offsetTimer += ds::getElapsedSeconds();
	if (hctx.offsetTimer > ctx->settings.higschoreSwitchTTL) {
		hctx.offsetTimer -= ctx->settings.higschoreSwitchTTL;
		hctx.offset += 5;
		if (hctx.offset >= 10) {
			hctx.offset = 0;
			hctx.mode = (hctx.mode + 1) & 1;
		}
	}
	char buffer[128];
	for (int i = 0; i < 5; ++i) {
		int idx = hctx.offset + hctx.mode * 10;
		int points = hctx.highscores[idx + i].points;
		// rank
		dialog::Image(ds::vec2(130, 450 - i * 50), ds::vec4(0, 825, 80, 36));
		sprintf(buffer, "%2d", (hctx.offset + i + 1));
		dialog::Text(ds::vec2(100, 450 - i * 50), buffer, false);
		// name
		dialog::Image(ds::vec2(460, 450 - i * 50), ds::vec4(0, 825, 500, 36));
		if (points != -1) {
			sprintf(buffer, "%s",hctx.highscores[idx + i].name);
			dialog::Text(ds::vec2(220, 450 - i * 50), buffer, false);
		}
		// points
		dialog::Image(ds::vec2(850, 450 - i * 50), ds::vec4(0, 825, 200, 36));
		if (points != -1) {
			sprintf(buffer, "%6d", hctx.highscores[idx + i].points);
			dialog::Text(ds::vec2(760, 450 - i * 50), buffer, false);
		}
	}
	if (dialog::Button(ds::vec2(dx, 160), ds::vec4(470, 290, 260, 60))) {
		ret = 2;
	}
	dialog::end();
	return ret;
}

static const char* GAME_OVER_LABELS[] = { "Pieces cleared", "Time", "Highest combo", "Total Score" };
// ---------------------------------------------------------------
// show game over menu
// ---------------------------------------------------------------
int showGameOverMenu(GameContext* ctx, float time, float ttl) {
	int ret = 0;
	dialog::begin();
	int dy = LOGO_Y_POS;
	if (time <= ttl) {
		dy = tweening::interpolate(tweening::easeOutElastic, 1000, LOGO_Y_POS, time, ttl);
	}
	dialog::Image(ds::vec2(512, dy), ds::vec4(200, 500, 540, 55));

	int y = 540;
	int x = 180;
	for (int i = 0; i < 4; ++i) {
		dialog::Image(ds::vec2(x + 200, y - i * 60), ds::vec4(540, 160, 450, 40));
		dialog::Image(ds::vec2(x + 550, y - i * 60), ds::vec4(540, 160, 200, 40));
		dialog::Text(ds::vec2(x, y - i * 60), GAME_OVER_LABELS[i], false);
	}

	dialog::FormattedText(ds::vec2(x + 540, y), false, "%d", ctx->score.itemsCleared);
	dialog::FormattedText(ds::vec2(x + 480, y - 60), false, "%02d %02d", ctx->score.minutes, ctx->score.seconds);
	dialog::FormattedText(ds::vec2(x + 540, y - 120), false, "%d", ctx->score.highestCombo);
	dialog::FormattedText(ds::vec2(x + 460, y - 180), false, "%06d", ctx->score.points);
	
	int dx = floatButton(time, ttl, FloatInDirection::FID_LEFT);
	if (dialog::Button(ds::vec2(dx, 260), ds::vec4(730, 290, 260, 60))) {
		ret = 1;
	}
	dx = floatButton(time, ttl, FloatInDirection::FID_RIGHT);
	if (dialog::Button(ds::vec2(dx, 180), ds::vec4(470, 290, 260, 60))) {
		ret = 2;
	}
	dialog::end();
	return ret;
}

// ---------------------------------------------------------------
// show new highscore menu
// ---------------------------------------------------------------
int showNewHighscoreMenu(GameContext* ctx, float time, float ttl) {
	int ret = 0;
	dialog::begin();
	int dy = LOGO_Y_POS;
	if (time <= ttl) {
		dy = tweening::interpolate(tweening::easeOutElastic, 1000, LOGO_Y_POS, time, ttl);
	}
	dialog::Image(ds::vec2(512, dy), ds::vec4(200, 500, 540, 55));

	int y = 540;
	int x = 180;

	if (ctx->ranking != -1) {
		int r = ctx->ranking + 1;
		if (r > 10) {
			r -= 10;
		}
		dialog::FormattedText(ds::vec2(512, 500), true, "New highscore at rank %d", r);
	}

	dialog::Text(ds::vec2(512, 400),"Please enter your name");
	dialog::Image(ds::vec2(512,350), ds::vec4(540, 160, 580, 40));
	dialog::Input(ds::vec2(512, 350), ctx->user, 16);

	int dx = floatButton(time, ttl, FloatInDirection::FID_LEFT);
	if (dialog::Button(ds::vec2(dx, 260), ds::vec4(470, 350, 260, 60))) {
		ret = 1;
	}	
	dialog::end();
	return ret;
}

// ---------------------------------------------------------------
// show main menu
// ---------------------------------------------------------------
int showMainMenu(float time, float ttl) {
	int ret = 0;
	dialog::begin();
	int dy = LOGO_Y_POS;
	if (time <= ttl) {
		dy = tweening::interpolate(tweening::easeOutElastic, 1000, LOGO_Y_POS, time, ttl);
	}
	dialog::Image(ds::vec2(512, dy), ds::vec4(225, 5, 770, 55));
	dialog::Image(ds::vec2(512, 35), ds::vec4(0, 955, 530, 12));
	int dx = floatButton(time, ttl, FloatInDirection::FID_LEFT);
	if (dialog::Button(ds::vec2(dx, 420), ds::vec4(0, 70, 260, 60))) {
		ret = 1;
	}
	dx = floatButton(time, ttl, FloatInDirection::FID_RIGHT);
	if (dialog::Button(ds::vec2(dx, 290), ds::vec4(270, 70, 260, 60))) {
		ret = 3;
	}
	dx = floatButton(time, ttl, FloatInDirection::FID_LEFT);
	if (dialog::Button(ds::vec2(dx, 160), ds::vec4(270, 130, 260, 60))) {
		ret = 2;
	}
	dialog::end();
	return ret;
}

// ---------------------------------------------------------------
// show game mode menu
// ---------------------------------------------------------------
int showGameModeMenu(float time, float ttl) {
	int ret = 0;
	dialog::begin();
	int dy = LOGO_Y_POS;
	if (time <= ttl) {
		dy = tweening::interpolate(tweening::easeOutElastic, 1000, LOGO_Y_POS, time, ttl);
	}
	dialog::Image(ds::vec2(512, dy), ds::vec4(200, 620, 564, 60));
	dialog::Image(ds::vec2(512, 35), ds::vec4(0, 955, 530, 12));

	int dx = floatButton(time, ttl, FloatInDirection::FID_LEFT);
	if (dialog::Button(ds::vec2(dx, 520), ds::vec4(210, 290, 260, 60))) {
		ret = 1;
	}
	if (dialog::isCursorInside(ds::vec2(dx, 520), ds::vec2(260, 60))) {
		dialog::FormattedText(ds::vec2(512, 460), true, "Try to clean up the board");
		dialog::FormattedText(ds::vec2(512, 430), true, "without timelimit");
	}
	dx = floatButton(time, ttl, FloatInDirection::FID_RIGHT);
	if (dialog::Button(ds::vec2(dx, 350), ds::vec4(210, 350, 260, 60))) {
		ret = 2;
	}
	if (dialog::isCursorInside(ds::vec2(dx, 350), ds::vec2(260, 60))) {
		dialog::FormattedText(ds::vec2(512, 290), true, "Try to clean up as much as you");
		dialog::FormattedText(ds::vec2(512, 260), true, "can within 2 minutes");
	}
	dx = floatButton(time, ttl, FloatInDirection::FID_LEFT);
	if (dialog::Button(ds::vec2(dx, 180), ds::vec4(470, 290, 260, 60))) {
		ret = 3;
	}
	dialog::end();
	return ret;
}

