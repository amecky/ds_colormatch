#pragma once
#include <diesel.h>
#include <SpriteBatchBuffer.h>


struct Score {
	int itemsCleared;
	int seconds;
	int minutes;
	int points;
	int totalPoints;
	int highestCombo;
};

class HUD {

public:
	HUD(SpriteBatchBuffer* buffer, RID textureID, Score* score);
	~HUD();
	void render();
	void tick(float dt);
	void reset();
	void rebuildScore();
private:
	float _timer;
	int _numbers[6];
	int _minutes[2];
	int _seconds[2];
	SpriteBatchBuffer* _buffer;
	RID _textureID;
	Score* _score;
};

