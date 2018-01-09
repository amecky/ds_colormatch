#include "Dialog.h"
#include "Constants.h"
#include <ds_game_ui.h>
#include "utils\utils.h"
#include "utils\tweening.h"
#include "GameSettings.h"
#include "utils\HUD.h"

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

	dialog::FormattedText(ds::vec2(x + 540, y), false, ds::vec2(1.0f), "%d", ctx->score.itemsCleared);
	dialog::FormattedText(ds::vec2(x + 480, y - 60), false, ds::vec2(1.0f), "%02d %02d", ctx->score.minutes, ctx->score.seconds);
	dialog::FormattedText(ds::vec2(x + 540, y - 120), false, ds::vec2(1.0f), "%d", ctx->score.highestCombo);
	dialog::FormattedText(ds::vec2(x + 460, y - 180), false, ds::vec2(1.0f), "%06d", ctx->score.points);
	
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
		dialog::FormattedText(ds::vec2(512, 500), true, ds::vec2(1.0f), "New highscore at rank %d", r);
	}

	dialog::Text(ds::vec2(512, 400),"Please enter your name");
	dialog::Image(ds::vec2(512,350), ds::vec4(540, 160, 580, 40));
	dialog::Input(ds::vec2(512, 350), ctx->user, 16);

	int dx = floatButton(time, ttl, FloatInDirection::FID_LEFT);
	if (dialog::Button(ds::vec2(dx, 260), ds::vec4(470, 350, 260, 60))) {
		insertHighscore(ctx, ctx->ranking);
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
		dialog::FormattedText(ds::vec2(512, 460), true, ds::vec2(1.0f), "Try to clean up the board");
		dialog::FormattedText(ds::vec2(512, 430), true, ds::vec2(1.0f), "without timelimit");
	}
	dx = floatButton(time, ttl, FloatInDirection::FID_RIGHT);
	if (dialog::Button(ds::vec2(dx, 350), ds::vec4(210, 350, 260, 60))) {
		ret = 2;
	}
	if (dialog::isCursorInside(ds::vec2(dx, 350), ds::vec2(260, 60))) {
		dialog::FormattedText(ds::vec2(512, 290), true, ds::vec2(1.0f), "Try to clean up as much as you");
		dialog::FormattedText(ds::vec2(512, 260), true, ds::vec2(1.0f), "can within 2 minutes");
	}
	dx = floatButton(time, ttl, FloatInDirection::FID_LEFT);
	if (dialog::Button(ds::vec2(dx, 180), ds::vec4(470, 290, 260, 60))) {
		ret = 3;
	}
	dialog::end();
	return ret;
}

