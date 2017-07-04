#pragma once
#include "Grid.h"
#include <diesel.h>
#include <SpriteBatchBuffer.h>
#include <vector>
#include "Constants.h"

struct Score;
struct GameSettings;

// -------------------------------------------------------
// tile state
// -------------------------------------------------------
enum TileState {
	TS_NORMAL,
	TS_WIGGLE,
	TS_SHRINKING
};

// -------------------------------------------------------
// our specialized grid entry
// -------------------------------------------------------
struct MyEntry {
	int color;
	bool hidden;
	float scale;
	float timer;
	float ttl;
	TileState state;
};

// -------------------------------------------------------
// moving cell
// -------------------------------------------------------
struct MovingCell {

	int x;
	int y;
	ds::vec2 start;
	ds::vec2 end;
	ds::vec2 current;
	int color;
};

// -------------------------------------------------------
// Color grid
// -------------------------------------------------------
class ColorGrid : public ds::Grid<MyEntry> {

public:
	ColorGrid() : ds::Grid<MyEntry>(MAX_X, MAX_Y) {}
	virtual ~ColorGrid() {}
	bool isMatch(const MyEntry& first, const MyEntry& right) {
		return first.color == right.color;
	}
};

// -------------------------------------------------------
// Board
// -------------------------------------------------------
class Board {

struct Message {
	float timer;
	float ttl;
	float scale;
	ds::Color color;
	ds::vec4 texture;
	float rotation;
	bool active;
};

enum BoardMode {
	BM_PREPARE,
	BM_FILLING,
	BM_FLASHING,
	BM_MOVING,
	BM_READY,
	BM_CLEARING,
	BM_IDLE
};

enum ScaleMode {
	SM_UP,
	SM_DOWN
};

typedef std::vector<ds::vec2> Points;
typedef std::vector<ds::DroppedCell<MyEntry>> DroppedCells;
typedef std::vector<MovingCell> MovingCells;

public:
	Board(SpriteBatchBuffer* buffer, RID textureID, GameSettings* settings);
	virtual ~Board();
	void fill(int maxColors);
	bool select(Score* score);
	void move(const ds::vec2& mousePos);
	int getMovesLeft() {
		return 100;
	}
	void init();
	void update(float elasped);
	void render();
	bool isReady() const {
		return m_Mode != BM_FILLING;
	}
	void clearBoard();
private:
	void activateMessage(int idx);
	bool scalePieces(float elapsed, ScaleMode scaleMode);
	ColorGrid m_Grid;
	Points m_Points;
	DroppedCells m_DroppedCells;
	MovingCells m_MovingCells;
	BoardMode m_Mode;
	float m_Timer;
	int m_CellCounter;
	ds::vec4 _gridTex[4];
	int m_Counter;
	GameSettings* _settings;
	int _flashCount;

	int _selectedX;
	int _selectedY;
	SpriteBatchBuffer* _buffer;
	RID _textureID;
	ds::Color _piecesColors[8];
	Message _messages[2];
	
};

