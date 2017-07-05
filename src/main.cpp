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
	GM_RUNNING,
	GM_GAMEOVER
};

// ---------------------------------------------------------------
// Background settings
// ---------------------------------------------------------------
struct BackgroundSettings {
	ds::Color colors[8];
	float timer;
	int colorIndex;
	ds::Color startColor;
	ds::Color endColor;
	float ttl;
	float minAlpha;
	float maxAlpha;
};

// ---------------------------------------------------------------
// DebugPanel
// ---------------------------------------------------------------
struct DebugPanel {
	char key;
	bool pressed;
	bool active;
	int state;
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
// show game over menu
// ---------------------------------------------------------------
int showGameOverMenu(const Score& score) {
	int ret = 0;
	char buffer[256];
	dialog::begin();
	dialog::Image(ds::vec2(512, 620), ds::vec4(0, 870, 530, 45));
	ds::vec2 mp = ds::getMousePosition();
	dialog::Image(ds::vec2(512, 550), ds::vec4(420, 570, 500, 42));
	sprintf_s(buffer, 256, "Pieces cleared: %d", score.itemsCleared);
	dialog::Text(ds::vec2(400, 550), buffer);
	dialog::Image(ds::vec2(512, 500), ds::vec4(420, 570, 500, 42));
	sprintf_s(buffer, 256, "Time: %02d:%02d", score.minutes, score.seconds);
	dialog::Text(ds::vec2(400, 500), buffer);
	dialog::Image(ds::vec2(512, 450), ds::vec4(420, 570, 500, 42));
	sprintf_s(buffer, 256, "Score: %d", score.points);
	dialog::Text(ds::vec2(400, 450), buffer);
	dialog::Image(ds::vec2(512, 400), ds::vec4(420, 570, 500, 42));
	sprintf_s(buffer, 256, "Highest combo: %d", score.highestCombo);
	dialog::Text(ds::vec2(400, 400), buffer);
	if (dialog::Button(ds::vec2(512, 320), ds::vec4(0, 70, 260, 60))) {
		ret = 1;
	}
	if (dialog::Button(ds::vec2(512, 230), ds::vec4(270, 130, 260, 60))) {
		ret = 2;
	}
	dialog::end();
	return ret;
}

// ---------------------------------------------------------------
// show main menu
// ---------------------------------------------------------------
int showMainMenu() {
	int ret = 0;
	dialog::begin();
	dialog::Image(ds::vec2(512, 35), ds::vec4(0, 955, 530, 12));
	if (dialog::Button(ds::vec2(512, 438), ds::vec4(0, 70, 260, 60))) {
		ret = 1;
	}
	if (dialog::Button(ds::vec2(512, 298), ds::vec4(270, 130, 260, 60))) {
		ret = 2;
	}
	dialog::end();
	return ret;
}

// ---------------------------------------------------------------
// handle input for debug panel
// ---------------------------------------------------------------
void handleDebugInput(DebugPanel* panel) {
	if (ds::isKeyPressed(panel->key)) {
		panel->pressed = true;
	}
	else if (panel->pressed) {
		panel->pressed = false;
		panel->active = !panel->active;
	}
}
// ---------------------------------------------------------------
// main method
// ---------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
	
	initialize();

	gui::init();

	// load image using stb_image
	RID textureID = loadImage("content\\TextureArray.png");

	// create the sprite batch buffer
	SpriteBatchBufferInfo sbbInfo = { 2048, textureID, ds::TextureFilters::LINEAR };
	SpriteBatchBuffer spriteBuffer(sbbInfo);

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

	BackgroundSettings bgSettings;
	color::pick_colors(bgSettings.colors,8);
	bgSettings.colorIndex = 1;
	bgSettings.minAlpha = 0.6f;
	bgSettings.maxAlpha = 0.8f;
	bgSettings.ttl = 2.0f;
	bgSettings.startColor = bgSettings.colors[0];
	bgSettings.endColor = bgSettings.colors[1];

	Board* board = new Board(&spriteBuffer,textureID, &settings);

	Score score;

	HUD hud(&spriteBuffer, textureID, &score);
	hud.reset();

	char txt[256];

	bool pressed = false;

	GameMode mode = GM_MENU;

	dialog::init(&spriteBuffer, textureID);

	int moves = 0;

	bool running = true;

	DebugPanel backDbgPanel  = { 'D', false, false, 1 };
	DebugPanel infoDbgPanel  = { 'I', false, false, 1 };
	DebugPanel settingsPanel = { 'S', false, false, 1 };

	while (ds::isRunning() && running) {

		handleDebugInput(&backDbgPanel);
		handleDebugInput(&infoDbgPanel);
		handleDebugInput(&settingsPanel);

		if (ds::isKeyPressed('C')) {
			board->clearBoard();
			mode = GM_GAMEOVER;
		}

		ds::begin();

		spriteBuffer.begin();

		//
		// Background
		//
		bgSettings.timer += ds::getElapsedSeconds();
		float norm = bgSettings.timer / bgSettings.ttl;
		ds::Color clr = tweening::interpolate(tweening::linear, bgSettings.startColor, bgSettings.endColor, bgSettings.timer, bgSettings.ttl);
		clr.a = tweening::interpolate(tweening::linear, bgSettings.minAlpha, bgSettings.maxAlpha, bgSettings.timer, bgSettings.ttl);
		if (norm >= 1.0f) {
			bgSettings.timer = 0.0f;
			bgSettings.ttl = ds::random(2.0f,4.0f);
			bgSettings.minAlpha = bgSettings.maxAlpha;
			bgSettings.maxAlpha = ds::random(0.6f, 0.8f);
			bgSettings.startColor = bgSettings.colors[bgSettings.colorIndex];
			++bgSettings.colorIndex;
			if (bgSettings.colorIndex >= 8) {
				bgSettings.colorIndex = 0;
			}
			bgSettings.endColor = bgSettings.colors[bgSettings.colorIndex];
		}
		spriteBuffer.add(ds::vec2(512, 384), ds::vec4(512, 200, 512, 316),ds::vec2(2,2),0.0f,clr);

		// top and bottom bar
		spriteBuffer.add(ds::vec2(512, 734), ds::vec4(0, 720, 1024, 68));
		spriteBuffer.add(ds::vec2(512, 34), ds::vec4(0, 800, 1024, 68));

		if (mode == GM_RUNNING) {
			board->render();
		}

		if (mode == GM_MENU) {
			int ret = showMainMenu();
			if (ret == 1) {
				board->fill(4);
				mode = GM_RUNNING;
			}
			else  if (ret == 2) {
				running = false;
			}
		}
		else if (mode == GM_GAMEOVER) {
			board->render();
			int ret = showGameOverMenu(score);
			if (ret == 1) {
				board->fill(4);
				mode = GM_RUNNING;
			}
			else if (ret == 2) {
				mode = GM_MENU;
			}
		}
		else if (mode == GM_RUNNING) {			
			if (ds::isMouseButtonPressed(0) && !pressed) {
				if ( board->select(&score)) {
					hud.rebuildScore();
					
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
				mode = GM_GAMEOVER;
			}
		}

		if (mode == GM_RUNNING || mode == GM_GAMEOVER) {
			board->update(static_cast<float>(ds::getElapsedSeconds()));
		}

		if (mode == GM_RUNNING) {
			hud.render();
		}
		
		spriteBuffer.flush();

		if (infoDbgPanel.active || backDbgPanel.active || settingsPanel.active) {
			gui::start(ds::vec2(0, 755));
			if (infoDbgPanel.active) {
				gui::begin("Debug", &infoDbgPanel.state);
				gui::Value("FPS", ds::getFramesPerSecond());
				int cx = -1;
				int cy = -1;
				input::convertMouse2Grid(&cx, &cy);
				gui::Value("MPG", ds::vec2(cx, cy));
				gui::Value("Moves", moves);
				if (mode == GM_RUNNING) {
					if (gui::Button("Hightlight")) {
						board->highlightBlock();
					}
					if (gui::Button("Move")) {
						board->move();
					}
				}
			}
			if (backDbgPanel.active) {
				gui::begin("Background", &backDbgPanel.state);
				gui::Value("TTL", bgSettings.ttl);
				gui::Value("Min A", bgSettings.minAlpha);
				gui::Value("Max A", bgSettings.maxAlpha);
				gui::Value("Start", bgSettings.startColor);
				gui::Value("End", bgSettings.endColor);
				gui::Value("Current", clr);
				gui::Value("Index", bgSettings.colorIndex);
			}
			if (settingsPanel.active) {
				gui::begin("Settings", &settingsPanel.state);
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
			gui::end();
		}
		ds::end();
	}
	delete board;
	ds::shutdown();
}