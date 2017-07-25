#pragma once
#include <diesel.h>
#include <SpriteBatchBuffer.h>

namespace dialog {

	void init(SpriteBatchBuffer* buffer);

	void begin();

	void Image(const ds::vec2& pos, const ds::vec4& rect);

	bool Button(const ds::vec2& pos, const ds::vec4& rect);

	void Text(const ds::vec2& pos, const char* text);

	void FormattedText(const ds::vec2& pos, const char* fmt, ...);

	void end();

	void shutdown();

}

struct GameSettings;

class HighscoreDialog {

public:
	HighscoreDialog(GameSettings* settings);
	~HighscoreDialog();
	void start();
	int tick(float dt);
private:
	GameSettings* _settings;
	float _timer;
	int _mode;
	int _offset;
	float _offsetTimer;

};
