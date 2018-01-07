#pragma once
#include <diesel.h>
#include <SpriteBatchBuffer.h>

namespace dialog {

	void init(SpriteBatchBuffer* buffer);

	void begin();

	void Image(const ds::vec2& pos, const ds::vec4& rect);

	bool Button(const ds::vec2& pos, const ds::vec4& rect);

	void Text(const ds::vec2& pos, const char* text, bool centered = true);

	void FormattedText(const ds::vec2& pos, bool centered, const char* fmt, ...);

	void Input(const ds::vec2& pos, char* text, int maxLength);

	void end();

}

struct GameContext;

int showGameOverMenu(GameContext* ctx, float time, float ttl);

int showNewHighscoreMenu(GameContext* ctx, float time, float ttl);

int showMainMenu(float time, float ttl);

int showGameModeMenu(float time, float ttl);

int showHighscoresMenu(GameContext* ctx, float time, float ttl);

