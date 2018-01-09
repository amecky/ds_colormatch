#include "highscore.h"
#include "..\GameSettings.h"
// ---------------------------------------------------------------
// load highscores
// ---------------------------------------------------------------
void loadHighscores(GameContext* ctx) {
	FILE* fp = fopen("scores", "rb");
	if (fp) {
		for (int i = 0; i < 20; ++i) {
			fread(&ctx->highscoreContext.highscores[i].points, sizeof(int), 1, fp);
			fread(&ctx->highscoreContext.highscores[i].name, sizeof(char) * 16, 1, fp);
		}
		fclose(fp);
	}
	else {
		for (int i = 0; i < 20; ++i) {
			ctx->highscoreContext.highscores[i].points = -1;
			sprintf(ctx->highscoreContext.highscores[i].name, "Name %d", (i + 1));
		}
	}
}

// ---------------------------------------------------------------
// save highscores
// ---------------------------------------------------------------
void saveHighscores(GameContext* ctx) {
	FILE* fp = fopen("scores", "wb");
	if (fp) {
		for (int i = 0; i < 20; ++i) {
			fwrite(&ctx->highscoreContext.highscores[i].points, sizeof(int), 1, fp);
			fwrite(&ctx->highscoreContext.highscores[i].name, sizeof(char) * 16, 1, fp);
		}
		fclose(fp);
	}
}

// ---------------------------------------------------------------
// get highscore ranking
// ---------------------------------------------------------------
int getHighscoreRanking(GameContext* ctx) {
	int offset = 0;
	if (ctx->game_play_mode == GamePlayMode::GPM_TIMER) {
		offset += 10;
	}
	for (int i = 0; i < 10; ++i) {
		int current = ctx->highscoreContext.highscores[offset + i].points;
		if (current == -1 || current < ctx->score.points) {
			return offset + i;
		}
	}
	return -1;
}

// ---------------------------------------------------------------
// insert highscore
// ---------------------------------------------------------------
void insertHighscore(GameContext* ctx, int rank) {
	int start = rank;
	int end = 9;
	if (rank > 10) {
		end = 19;
	}
	for (int i = end; i > start; --i) {
		ctx->highscoreContext.highscores[i] = ctx->highscoreContext.highscores[i - 1];
	}
	ctx->highscoreContext.highscores[rank].points = ctx->score.points;
	sprintf_s(ctx->highscoreContext.highscores[rank].name, "%s", ctx->user);
}
