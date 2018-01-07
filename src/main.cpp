#define DS_IMPLEMENTATION
#include <diesel.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define SPRITE_IMPLEMENTATION
#include <SpriteBatchBuffer.h>
#include "Board.h"
#include "GameSettings.h"
#include "utils\utils.h"
#include "utils\HUD.h"
#include "Dialog.h"
#define DS_IMGUI_IMPLEMENTATION
#include <ds_imgui.h>
#include "..\resource.h"
#include "utils\colors.h"
#include "utils\tweening.h"

// ---------------------------------------------------------------
// load image from the resources
// ---------------------------------------------------------------
RID loadImage(const char* name) {
	int x, y, n;
	HRSRC resourceHandle = ::FindResource(NULL, MAKEINTRESOURCE(IDB_PNG2), "PNG");
	if (resourceHandle == 0) {
		return NO_RID;
	}
	DWORD imageSize = ::SizeofResource(NULL, resourceHandle);
	if (imageSize == 0) {
		return NO_RID;
	}
	HGLOBAL myResourceData = ::LoadResource(NULL, resourceHandle);
	void* pMyBinaryData = ::LockResource(myResourceData);
	unsigned char *data = stbi_load_from_memory((const unsigned char*)pMyBinaryData, imageSize, &x, &y, &n, 4);
	ds::TextureInfo info = { x,y,n,data,ds::TextureFormat::R8G8B8A8_UNORM , ds::BindFlag::BF_SHADER_RESOURCE };
	RID textureID = ds::createTexture(info, name);
	stbi_image_free(data);
	UnlockResource(myResourceData);
	FreeResource(myResourceData);
	return textureID;
}

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

// ---------------------------------------------------------------
// show main menu
// ---------------------------------------------------------------
void showMainMenu(GameContext* ctx) {
	ctx->menuTimer += static_cast<float>(ds::getElapsedSeconds());
	int ret = showMainMenu(ctx->menuTimer, ctx->settings.menuTTL);
	if (ret == 1) {
		ctx->menuTimer = 0.0f;
		ctx->mode = GameMode::GM_GAME_MODE;
	}
	else  if (ret == 2) {
		ctx->running = false;
	}
	else if (ret == 3) {
		ctx->menuTimer = 0.0f;
		ctx->highscoreContext.mode = 0;
		ctx->highscoreContext.offset = 0;
		ctx->highscoreContext.offsetTimer = 0.0f;
		ctx->mode = GameMode::GM_HIGHSCORES;
	}
}

// ---------------------------------------------------------------
// show game over
// ---------------------------------------------------------------
void showGameOver(GameContext* ctx) {
	ctx->menuTimer += static_cast<float>(ds::getElapsedSeconds());
	ctx->board->render();
	int ret = showGameOverMenu(ctx, ctx->menuTimer, ctx->settings.menuTTL);
	if (ret == 1) {
		color::pick_colors(ctx->colors, 8);
		ctx->board->fill(4);
		ctx->mode = GameMode::GM_RUNNING;
	}
	else if (ret == 2) {
		ctx->menuTimer = 0.0f;
		ctx->mode = GameMode::GM_MENU;
	}
	ctx->board->update(static_cast<float>(ds::getElapsedSeconds()));
}

// ---------------------------------------------------------------
// show game mode selection
// ---------------------------------------------------------------
void showGameModeSelection(GameContext* ctx) {
	ctx->menuTimer += static_cast<float>(ds::getElapsedSeconds());
	int ret = showGameModeMenu(ctx->menuTimer, ctx->settings.menuTTL);
	if (ret == 1) {
		color::pick_colors(ctx->colors, 8);
		ctx->game_play_mode = GamePlayMode::GPM_ZEN;
		ctx->hud->reset();
		ctx->board->fill(4);
		ctx->mode = GameMode::GM_RUNNING;
	}
	else if (ret == 2) {
		color::pick_colors(ctx->colors, 8);
		ctx->board->fill(4);
		ctx->game_play_mode = GamePlayMode::GPM_TIMER;
		ctx->hud->reset();
		ctx->mode = GameMode::GM_RUNNING;
	}
	else if (ret == 3) {
		ctx->menuTimer = 0.0f;
		ctx->mode = GameMode::GM_MENU;
	}
}

// ---------------------------------------------------------------
// show highscores
// ---------------------------------------------------------------
void showHighscores(GameContext* ctx) {
	ctx->menuTimer += static_cast<float>(ds::getElapsedSeconds());
	int ret = showHighscoresMenu(ctx, ctx->menuTimer, ctx->settings.menuTTL);
	if (ret == 2) {
		ctx->menuTimer = 0.0f;
		ctx->mode = GameMode::GM_MENU;
	}
}

// ---------------------------------------------------------------
// show highscores
// ---------------------------------------------------------------
void showNewHighscore(GameContext* ctx) {
	ctx->menuTimer += static_cast<float>(ds::getElapsedSeconds());
	int ret = showNewHighscoreMenu(ctx, ctx->menuTimer, ctx->settings.menuTTL);
	if (ret == 1) {
		ctx->menuTimer = 0.0f;
		ctx->mode = GameMode::GM_GAMEOVER;
	}
}

// ---------------------------------------------------------------
// main game
// ---------------------------------------------------------------
void showMainGame(GameContext* ctx) {
	if (ds::isMouseButtonPressed(0) && !ctx->pressed) {
		if (ctx->board->select(&ctx->score)) {
			ctx->hud->rebuildScore();
			ctx->hud->setPieces(ctx->score.piecesLeft);
		}
		ctx->pressed = true;
	}
	if (!ds::isMouseButtonPressed(0) && ctx->pressed) {
		ctx->pressed = false;
	}
	ctx->moves = ctx->board->getNumberOfMoves();

	if (ctx->board->isReady()) {
		ctx->hud->tick(static_cast<float>(ds::getElapsedSeconds()));
	}

	if (ctx->moves == 0) {
		ctx->board->clearBoard();
		ctx->score.minutes = ctx->hud->getMinutes();
		ctx->score.seconds = ctx->hud->getSeconds();
		ctx->menuTimer = 0.0f;
		ctx->ranking = getHighscoreRanking(ctx);
		if (ctx->ranking != -1) {
			ctx->mode = GameMode::GM_NEW_HIGHSCORE;
		}
		else {
			ctx->mode = GameMode::GM_GAMEOVER;
		}
	}
	if (ctx->game_play_mode == GamePlayMode::GPM_TIMER) {
		if (ctx->hud->getMinutes() == 0 && ctx->hud->getSeconds() == 0) {
			ctx->board->clearBoard();
			ctx->menuTimer = 0.0f;
			ctx->score.minutes = 2;
			ctx->score.seconds = 0;
			ctx->ranking = getHighscoreRanking(ctx);
			if (ctx->ranking != -1) {
				ctx->mode = GameMode::GM_NEW_HIGHSCORE;
			}
			else {
				ctx->mode = GameMode::GM_GAMEOVER;
			}
		}
	}
	ctx->board->render();
	ctx->hud->render();
	ctx->board->update(static_cast<float>(ds::getElapsedSeconds()));
}

// ---------------------------------------------------------------
// Debug GUI
// ---------------------------------------------------------------
void showDebugGUI(GameContext* ctx, int* dialogsStates) {
	p2i sp = p2i(10, 760);
	gui::start(&sp, 300);

	if (gui::begin("Debug", &dialogsStates[0])) {
		gui::Value("FPS", ds::getFramesPerSecond());
		int cx = -1;
		int cy = -1;
		input::convertMouse2Grid(&cx, &cy);
		gui::Value("MPG", ds::vec2(cx, cy));
		gui::Value("Moves", ctx->moves);
		if (ctx->mode == GameMode::GM_RUNNING) {
			if (gui::Button("Highlight")) {
				ctx->board->highlightBlock();
			}
			if (gui::Button("Move")) {
				ctx->board->move();
			}
		}		
		gui::Input("Button ttl", &ctx->settings.menuTTL);
		if (gui::Button("Reset timer")) {
			ctx->menuTimer = 0.0f;
		}
		if (gui::Button("Game Over")) {
			ctx->board->clearBoard();
			ctx->mode = GameMode::GM_GAMEOVER;
			ctx->menuTimer = 0.0f;
			ctx->score.minutes = ctx->hud->getMinutes();
			ctx->score.seconds = ctx->hud->getSeconds();
			ctx->menuTimer = 0.0f;
			ctx->ranking = getHighscoreRanking(ctx);
			if (ctx->ranking != -1) {
				ctx->mode = GameMode::GM_NEW_HIGHSCORE;
			}
			else {
				ctx->mode = GameMode::GM_GAMEOVER;
			}
		}
		if (gui::Button("New colors")) {
			color::pick_colors(ctx->colors, 8);
		}
	}
	if (gui::begin("Settings", &dialogsStates[1])) {
		gui::Input("Prepare TTL", &ctx->settings.prepareTTL);
		gui::Input("Message scale", &ctx->settings.messageScale);
		gui::Input("Min SU TTL", &ctx->settings.scaleUpMinTTL);
		gui::Input("Max SU TTL", &ctx->settings.scaleUpMaxTTL);
		gui::Input("Flash TTL", &ctx->settings.flashTTL);
		gui::Input("Dropping TTL", &ctx->settings.droppingTTL);
		gui::Input("Wiggle TTL", &ctx->settings.wiggleTTL);
		gui::Input("Wiggle Scale", &ctx->settings.wiggleScale);
		gui::Input("Min Clear TTL", &ctx->settings.clearMinTTL);
		gui::Input("Max Clear TTL", &ctx->settings.clearMaxTTL);
		gui::Input("Highlight Time", &ctx->settings.highlightTime);
		gui::Input("HS switch TTL", &ctx->settings.higschoreSwitchTTL);
		if (gui::Button("Restart")) {
			if (ctx->mode == GameMode::GM_RUNNING) {
				ctx->board->fill(4);
			}
		}
		if (gui::Button("Clear")) {
			if (ctx->mode == GameMode::GM_RUNNING) {
				ctx->board->clearBoard();
			}
		}
	}
	if (gui::begin("Board", &dialogsStates[2])) {
		ctx->board->debug();
	}
	gui::end();
}


// ---------------------------------------------------------------
// initialize rendering system
// ---------------------------------------------------------------
void initialize() {
	ds::RenderSettings rs;
	rs.width = 1024;
	rs.height = 768;
	rs.title = "Colors - match 3 game";
	rs.clearColor = ds::Color(0.125f, 0.125f, 0.125f, 1.0f);
	rs.multisampling = 4;
#ifdef DEBUG
	rs.supportDebug = true;
#endif
	ds::init(rs);
}

// ---------------------------------------------------------------
// main method
// ---------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
	
	//_CrtSetBreakAlloc(247);

	initialize();
	
#ifdef DEBUG
	gui::init();
	int dialogsStates[3] = { 0,0,0 };
#endif

	// load image using stb_image
	RID textureID = loadImage("content\\TextureArray.png");

	// create the sprite batch buffer
	SpriteBatchBufferInfo sbbInfo = { 2048, textureID, ds::TextureFilters::LINEAR };
	SpriteBatchBuffer spriteBuffer(sbbInfo);

	GameContext gameContext;
	color::pick_colors(gameContext.colors, 8);
	gameContext.game_play_mode = GamePlayMode::GPM_ZEN;
	// prepare the game settings
	gameContext.settings.flashTTL = 0.3f;
	gameContext.settings.droppingTTL = 0.2f;
	gameContext.settings.wiggleTTL = 0.4f;
	gameContext.settings.wiggleScale = 0.2f;
	gameContext.settings.clearMinTTL = 0.2f;
	gameContext.settings.clearMaxTTL = 0.8f;
	gameContext.settings.scaleUpMinTTL = 0.2f;
	gameContext.settings.scaleUpMaxTTL = 0.8f;
	gameContext.settings.prepareTTL = 1.0f;
	gameContext.settings.messageScale = 0.8f;
	gameContext.settings.highlightTime = 5.0f;
	gameContext.settings.logoSlideTTL = 1.6f;
	gameContext.settings.menuTTL = 1.6f;
	gameContext.settings.higschoreSwitchTTL = 2.0f;
	gameContext.ranking = -1;
	gameContext.highscoreContext.mode = 0;
	gameContext.highscoreContext.offset = 0;
	gameContext.highscoreContext.offsetTimer = 0.0f;
	loadHighscores(&gameContext);

	gameContext.board = new Board(&spriteBuffer, &gameContext);
	gameContext.hud = new HUD(&spriteBuffer, &gameContext);
	gameContext.hud->reset();
	gameContext.pressed = false;
	gameContext.mode = GameMode::GM_GAME_MODE;
	gameContext.moves = 0;
	gameContext.running = true;
	gameContext.menuTimer = 0.0f;
	gameContext.user[0] = '\0';

	dialog::init(&spriteBuffer);

	bool showDialog = true;
	bool guiKeyPressed = false;

	// FIXME:
	gameContext.ranking = 4;
	/*
	gameContext.game_play_mode = GamePlayMode::GPM_TIMER;
	gameContext.score.points = 1362;
	sprintf_s(gameContext.user, "My Test Name7");
	int nr = getHighscoreRanking(&gameContext);
	if (nr != -1) {
		insertHighscore(&gameContext, nr);
	}
	*/
	while (ds::isRunning() && gameContext.running) {

		ds::begin();

		spriteBuffer.begin();		

		for (int i = 0; i < 6; ++i) {
			spriteBuffer.add(ds::vec2(-50 + i * 200, 384), ds::vec4(0, 200, 200, 600));
		}

		if (gameContext.mode == GameMode::GM_MENU) {
			showMainMenu(&gameContext);
		}
		else if (gameContext.mode == GameMode::GM_GAME_MODE) {
			showGameModeSelection(&gameContext);
		}
		else if (gameContext.mode == GameMode::GM_GAMEOVER) {
			showGameOver(&gameContext);
		}
		else if (gameContext.mode == GameMode::GM_HIGHSCORES) {
			showHighscores(&gameContext);
		}
		else if (gameContext.mode == GameMode::GM_NEW_HIGHSCORE) {
			showNewHighscore(&gameContext);
		}
		else if (gameContext.mode == GameMode::GM_RUNNING) {
			showMainGame(&gameContext);
		}
		
		spriteBuffer.flush();

#ifdef DEBUG
		if (showDialog) {
			showDebugGUI(&gameContext, dialogsStates);
		}

		if (ds::isKeyPressed('D')) {
			if (!guiKeyPressed) {
				showDialog = !showDialog;
				guiKeyPressed = true;
			}
		}
		else {
			guiKeyPressed = false;
		}

		ds::dbgPrint(0, 37, "FPS: %d", ds::getFramesPerSecond());
#endif

		ds::end();
	}
	saveHighscores(&gameContext);
	gui::shutdown();
	delete gameContext.board;
	delete gameContext.hud;
	ds::shutdown();
}