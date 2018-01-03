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
// Game modes
// ---------------------------------------------------------------
enum GameMode {
	GM_MENU,
	GM_GAME_MODE,
	GM_RUNNING,
	GM_GAMEOVER,
	GM_HIGHSCORES
};

enum GamePlayMode {
	GPM_ZEN,
	GPM_TIMER
};
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
// initialize rendering system
// ---------------------------------------------------------------
void initialize() {
	ds::RenderSettings rs;
	rs.width = 1024;
	rs.height = 768;
	rs.title = "Colors - match 3 game";
	rs.clearColor = ds::Color(0.125f, 0.125f, 0.125f, 1.0f);
	rs.multisampling = 4;
	ds::init(rs);
}

// ---------------------------------------------------------------
// main method
// ---------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
	
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

	// prepare the game settings
	GameSettings settings;
	settings.flashTTL = 0.3f;
	settings.droppingTTL = 0.2f;
	settings.wiggleTTL = 0.4f;
	settings.wiggleScale = 0.2f;
	settings.clearMinTTL = 0.2f;
	settings.clearMaxTTL = 0.8f;
	settings.scaleUpMinTTL = 0.2f;
	settings.scaleUpMaxTTL = 0.8f;
	settings.prepareTTL = 1.0f;
	settings.messageScale = 0.8f;
	settings.highlightTime = 5.0f;
	settings.logoSlideTTL = 1.6f;

	GamePlayMode gamePlayMode;

	Board* board = new Board(&spriteBuffer, &gameContext, &settings);

	Score score;

	HUD hud(&spriteBuffer, &gameContext, &score);
	hud.reset();

	bool pressed = false;

	GameMode mode = GM_MENU;

	dialog::init(&spriteBuffer);

	int moves = 0;

	bool running = true;

	float menuTimer = 0.0f;
	float menuTTL = 1.6f;

	bool showDialog = true;
	bool guiKeyPressed = false;

	float dbgTimer = 0.0f;
	float dbgTTL = 0.7f;
	float dbgMaxScale = 1.6f;

	HighscoreDialog highscoreDialog(&settings);

	while (ds::isRunning() && running) {

		ds::begin();

		spriteBuffer.begin();		

		for (int i = 0; i < 6; ++i) {
			spriteBuffer.add(ds::vec2(-50 + i * 200, 384), ds::vec4(0, 200, 200, 600));
		}

		if (mode == GM_RUNNING) {
			board->render();
		}

		if (mode == GM_MENU) {
			menuTimer += static_cast<float>(ds::getElapsedSeconds());
			int ret = showMainMenu(menuTimer, menuTTL);
			if (ret == 1) {
				menuTimer = 0.0f;
				mode = GM_GAME_MODE;
			}
			else  if (ret == 2) {
				running = false;
			}
			else if (ret == 3) {
				menuTimer = 0.0f;
				highscoreDialog.start();
				mode = GM_HIGHSCORES;
			}
		}
		else if (mode == GM_GAME_MODE) {
			menuTimer += static_cast<float>(ds::getElapsedSeconds());
			int ret = showGameModeMenu(menuTimer, menuTTL);
			if (ret == 1) {
				color::pick_colors(gameContext.colors, 8);
				gamePlayMode = GPM_ZEN;
				hud.reset(TimerMode::TM_INC);
				board->fill(4);
				mode = GM_RUNNING;
			}
			else if (ret == 2) {
				color::pick_colors(gameContext.colors, 8);
				board->fill(4);
				hud.reset(TimerMode::TM_DEC);
				gamePlayMode = GPM_TIMER;
				mode = GM_RUNNING;
			}
		}
		else if (mode == GM_GAMEOVER) {
			menuTimer += static_cast<float>(ds::getElapsedSeconds());
			board->render();
			int ret = showGameOverMenu(score,menuTimer,menuTTL);
			if (ret == 1) {
				color::pick_colors(gameContext.colors, 8);
				board->fill(4);
				mode = GM_RUNNING;
			}
			else if (ret == 2) {
				menuTimer = 0.0f;
				mode = GM_MENU;
			}
		}
		else if (mode == GM_HIGHSCORES) {
			int ret = highscoreDialog.tick(static_cast<float>(ds::getElapsedSeconds()));
			if (ret == 2) {
				menuTimer = 0.0f;
				mode = GM_MENU;
			}
		}
		else if (mode == GM_RUNNING) {			
			if (ds::isMouseButtonPressed(0) && !pressed) {
				if ( board->select(&score)) {
					hud.rebuildScore();
					hud.setPieces(score.piecesLeft);
				}
				pressed = true;
			}
			if (!ds::isMouseButtonPressed(0) && pressed) {
				pressed = false;
			}
			moves = board->getNumberOfMoves();

			if (board->isReady()) {
				hud.tick(static_cast<float>(ds::getElapsedSeconds()));
			}

			if (moves == 0) {
				board->clearBoard();
				score.minutes = hud.getMinutes();
				score.seconds = hud.getSeconds();
				menuTimer = 0.0f;

				int totalScore = score.points - score.piecesLeft * 10 + score.highestCombo * 100;

				mode = GM_GAMEOVER;
			}
			if (gamePlayMode == GPM_TIMER) {
				if (hud.getMinutes() == 0 && hud.getSeconds() == 0) {
					board->clearBoard();
					menuTimer = 0.0f;
					mode = GM_GAMEOVER;
				}
			}
		}

		if (mode == GM_RUNNING || mode == GM_GAMEOVER) {
			board->update(static_cast<float>(ds::getElapsedSeconds()));
		}

		if (mode == GM_RUNNING) {
			hud.render();
		}

		float scale = 1.0f;
		if (dbgTimer <= dbgTTL) {
			scale = tweening::interpolate(tweening::easeOutElastic, dbgMaxScale, 1.0f, dbgTimer, dbgTTL);
			dbgTimer += ds::getElapsedSeconds();
		}
		font::renderText(ds::vec2(512, 70), "192", &spriteBuffer, scale);
		//font::renderText(ds::vec2(20, 90), "RSTUVWXYZ", &spriteBuffer);
		
		spriteBuffer.flush();

#ifdef DEBUG
		if (showDialog) {
			p2i sp = p2i(10, 760);
			gui::start(&sp, 300);
			
			if (gui::begin("Debug", &dialogsStates[0])) {
				gui::Value("FPS", ds::getFramesPerSecond());
				int cx = -1;
				int cy = -1;
				input::convertMouse2Grid(&cx, &cy);
				gui::Value("MPG", ds::vec2(cx, cy));
				gui::Value("Moves", moves);
				if (mode == GM_RUNNING) {
					if (gui::Button("Highlight")) {
						board->highlightBlock();
					}
					if (gui::Button("Move")) {
						board->move();
					}
				}
				gui::Input("dbgTTL", &dbgTTL);
				gui::Input("dbgMaxScale", &dbgMaxScale);
				if (gui::Button("Reset dbgTimer")) {
					dbgTimer = 0.0f;
				}
				gui::Input("Button ttl", &menuTTL);
				if (gui::Button("Reset timer")) {
					menuTimer = 0.0f;
				}
				if (gui::Button("Game Over")) {
					board->clearBoard();
					mode = GM_GAMEOVER;
					menuTimer = 0.0f;
				}
				if (gui::Button("New colors")) {
					color::pick_colors(gameContext.colors, 8);
				}
			}
			if (gui::begin("Settings", &dialogsStates[1])) {
				gui::Input("Prepare TTL", &settings.prepareTTL);
				gui::Input("Message scale", &settings.messageScale);
				gui::Input("Min SU TTL", &settings.scaleUpMinTTL);
				gui::Input("Max SU TTL", &settings.scaleUpMaxTTL);
				gui::Input("Flash TTL", &settings.flashTTL);
				gui::Input("Dropping TTL", &settings.droppingTTL);
				gui::Input("Wiggle TTL", &settings.wiggleTTL);
				gui::Input("Wiggle Scale", &settings.wiggleScale);
				gui::Input("Min Clear TTL", &settings.clearMinTTL);
				gui::Input("Max Clear TTL", &settings.clearMaxTTL);
				gui::Input("Highlight Time", &settings.highlightTime);
				if (gui::Button("Restart")) {
					if (mode == GM_RUNNING) {
						board->fill(4);
					}
				}
				if (gui::Button("Clear")) {
					if (mode == GM_RUNNING) {
						board->clearBoard();
					}
				}
			}
			if (gui::begin("Board", &dialogsStates[2])) {
				board->debug();
			}
			gui::end();
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
#endif

		ds::end();
	}
	delete board;
	ds::shutdown();
}