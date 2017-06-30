#include "Board.h"
#include "GameSettings.h"
#include "utils\tweening.h"
#include "utils\utils.h"
#include <Windows.h>
#include "utils\colors.h"
#include "utils\HUD.h"

void log(const char* message) {
	OutputDebugString(message);
	OutputDebugString("\n");
}

const static ds::vec4 TEXTURE = ds::vec4(0, 0, CELL_SIZE, CELL_SIZE);

ds::vec2 convertFromGrid(int gx, int gy) {
	return ds::vec2(STARTX + gx * CELL_SIZE, STARTY + gy * CELL_SIZE);
}

Board::Board(SpriteBatchBuffer* buffer, RID textureID, GameSettings* settings) : _buffer(buffer) , _textureID(textureID) , _settings(settings) {
	m_GridTex[0] = ds::vec4(  0, 200, 270, 486);
	m_GridTex[1] = ds::vec4( 30, 200, 240, 486);
	m_GridTex[2] = ds::vec4( 30, 200, 240, 486);
	m_GridTex[3] = ds::vec4(300, 200, 110, 486);
}

Board::~Board(void) {}

// -------------------------------------------------------
// Init
// -------------------------------------------------------
void Board::init() {
}

// -------------------------------------------------------
// Fill board
// -------------------------------------------------------
void Board::fill(int maxColors) {
	m_CellCounter = 0;
	for ( int x = 0; x < MAX_X; ++x ) {		
		for ( int y = 0; y < MAX_Y; ++y ) {		
			int cid = ds::random(0, maxColors);
			int offset = offset = cid * CELL_SIZE;
			ds::vec2 p = convertFromGrid(x, y);
			MyEntry& e = m_Grid.get(x, y);
			e.color = cid;
			e.hidden = false;
			e.scale = 1.0f;
			e.state = TS_NORMAL;
			e.timer = 0.0f;
			m_Grid.set(x, y, e);
			++m_CellCounter;
		}
	}
	m_Mode = BM_FILLING;
	m_Timer = 0.0f;
	m_Counter = MAX_X * MAX_Y;
	_selectedX = -1;
	_selectedY = -1;
	_flashCount = 0;
	color::pick_colors(_piecesColors,8);
}

// -------------------------------------------------------
// Draw
// -------------------------------------------------------
void Board::render() {
	_buffer->add(ds::vec2(213, 362), m_GridTex[0]);
	_buffer->add(ds::vec2(468, 362), m_GridTex[1]);
	_buffer->add(ds::vec2(708, 362), m_GridTex[2]);
	_buffer->add(ds::vec2(883, 362), m_GridTex[3]);
	// pieces
	for (int x = 0; x < MAX_X; ++x) {
		for (int y = 0; y < MAX_Y; ++y) {
			if (!m_Grid.isFree(x, y)) {
				MyEntry& e = m_Grid.get(x, y);
				if (m_Mode == BM_FILLING) {
					float norm = m_Timer / _settings->moveInTTL;
					ds::vec2 wp = convertFromGrid(x, y);
					ds::vec2 sp = wp;
					sp.y += _settings->moveInYAdd + y * _settings->moveInYOffset;
					_buffer->add(tweening::interpolate(&tweening::linear, sp, wp, m_Timer,_settings->moveInTTL), TEXTURE, ds::vec2(1,1), 0.0f, _piecesColors[e.color]);
				}
				else if (!e.hidden) {
					_buffer->add(convertFromGrid(x, y), TEXTURE,ds::vec2(e.scale),0.0f,_piecesColors[e.color]);
				}
			}
		}
	}

	// moving cells
	for (size_t i = 0; i < m_MovingCells.size(); ++i) {
		_buffer->add(m_MovingCells[i].current, TEXTURE,ds::vec2(1,1),0.0f,_piecesColors[m_MovingCells[i].color]);
	}
}

// -------------------------------------------------------
// Update
// -------------------------------------------------------
void Board::update(float elapsed) {
	
	if (m_Mode == BM_FILLING) {
		m_Timer += elapsed;
		if (m_Timer > _settings->moveInTTL) {
			m_Mode = BM_READY;
			m_Timer = 0.0f;
		}
	}
	else if (m_Mode == BM_FLASHING) {
		m_Timer += elapsed;
		if (m_Timer > _settings->flashTTL) {
			m_Mode = BM_READY;
			m_Timer = 0.0f;
			m_Grid.remove(m_Points,true);
			m_DroppedCells.clear();
			m_Grid.dropCells(m_DroppedCells);
			for (size_t i = 0; i < m_DroppedCells.size(); ++i) {
				const ds::DroppedCell<MyEntry>& dc = m_DroppedCells[i];
				ds::vec2 to = dc.to;
				MyEntry& e = m_Grid.get(to.x, to.y);
				e.hidden = true;
				MovingCell m;
				m.x = to.x;
				m.y = to.y;
				m.color = e.color;
				m.start = convertFromGrid(dc.from.x, dc.from.y);
				m.end = convertFromGrid(to.x, to.y);
				m_MovingCells.push_back(m);
			}
			if (!m_DroppedCells.empty()) {
				m_Mode = BM_MOVING;
				m_Timer = 0.0f;
			}			
		}
		
	}
	else if (m_Mode == BM_MOVING) {
		m_Timer += elapsed;
		if (m_Timer > _settings->droppingTTL) {
			m_Mode = BM_READY;
			m_Timer = 0.0f;
			for (size_t i = 0; i < m_MovingCells.size(); ++i) {
				MovingCell& m = m_MovingCells[i];
				MyEntry& e = m_Grid.get(m.x, m.y);
				e.hidden = false;
			}
			m_MovingCells.clear();
		}
		else {
			if (m_Timer <= _settings->droppingTTL) {
				float norm = m_Timer / _settings->droppingTTL;
				for (size_t i = 0; i < m_MovingCells.size(); ++i) {
					MovingCell& m = m_MovingCells[i];
					m.current = tweening::interpolate(&tweening::linear, m.start, m.end, m_Timer, _settings->droppingTTL);
				}
			}
		}
	}
	else if (m_Mode == BM_READY) {
		ds::vec2 mousePos = ds::getMousePosition();
		int mx = -1;
		int my = -1;
		if ( input::convertMouse2Grid(&mx,&my)) {
			if (mx != _selectedX || my != _selectedY) {
				_selectedX = mx;
				_selectedY = my;
				MyEntry& me = m_Grid(mx, my);
				if (me.state == TS_NORMAL) {
					me.timer = 0.0f;
					me.state = TS_WIGGLE;
				}
			}
		}
	}

	else if (m_Mode == BM_CLEARING) {
		if (m_Timer < _settings->clearTTL) {
			m_Timer += elapsed;
			float norm = m_Timer / _settings->clearTTL;
			for (int x = 0; x < MAX_X; ++x) {
				for (int y = 0; y < MAX_Y; ++y) {
					if (!m_Grid.isFree(x, y)) {
						MyEntry& e = m_Grid.get(x, y);
						e.timer += elapsed;
						if (e.timer > 1.0f) {
							e.timer = 1.0f;
						}
						e.scale = 1.0f - e.timer / _settings->clearTTL;
					}
				}
			}
		}
	}
	
	for (int x = 0; x < MAX_X; ++x) {
		for (int y = 0; y < MAX_Y; ++y) {
			if (!m_Grid.isFree(x, y)) {
				MyEntry& e = m_Grid.get(x, y);
				if (e.state == TS_SHRINKING) {
					e.timer += elapsed;
					if (e.timer >= _settings->flashTTL) {
						e.state = TS_NORMAL;
						e.scale = 1.0f;
						--_flashCount;
					}
					else {
						float norm = e.timer /_settings->flashTTL;
						e.scale = 1.0f - norm * 0.9f;
					}
				}
				else if (e.state == TS_WIGGLE) {
					e.timer += elapsed;
					if (e.timer >= _settings->wiggleTTL) {
						e.state = TS_NORMAL;
						e.scale = 1.0f;
					}
					else {
						float norm = e.timer / _settings->wiggleTTL;
						e.scale = 1.0f + sin(norm * ds::TWO_PI * 2.0f) * _settings->wiggleScale;
					}
				}
			}
		}
	}
}

void Board::clearBoard() {
	m_Mode = BM_CLEARING;
	m_Timer = 0.0f;
	for (int x = 0; x < MAX_X; ++x) {
		for (int y = 0; y < MAX_Y; ++y) {
			if (!m_Grid.isFree(x, y)) {
				MyEntry& e = m_Grid.get(x, y);
				e.timer = ds::random(0.0f, 0.1f);
			}
		}
	}
}

// -------------------------------------------------------
// move
// -------------------------------------------------------
void Board::move(const ds::vec2& mousePos) {
	int mx = -1;
	int my = -1;
	if(input::convertMouse2Grid(&mx,&my)) {
		m_Grid.shiftColumns(mx);
	}
}

// -------------------------------------------------------
// Select
// -------------------------------------------------------
bool Board::select(Score* score) {
	if ( m_Mode == BM_READY ) {
		int cx = -1;
		int cy = -1;
		if (input::convertMouse2Grid(&cx,&cy)) {
			MyEntry& me = m_Grid(cx, cy);
			m_Points.clear();		
			m_Grid.findMatchingNeighbours(cx,cy,m_Points);
			if ( m_Points.size() > 1 ) {
				m_Timer = 0.0f;
				m_Mode = BM_FLASHING;
				score->points += m_Points.size() * 10;				
				if (m_Points.size() > score->highestCombo) {
					score->highestCombo = m_Points.size();
				}
				score->itemsCleared += m_Points.size();
				for ( size_t i = 0; i < m_Points.size(); ++i ) {
					ds::vec2* gp = &m_Points[i];
					MyEntry& c = m_Grid.get(gp->x, gp->y);
					c.state = TS_SHRINKING;
					c.timer = 0.0f;
					++_flashCount;
				}
				return true;
			}
		}
	}
	return false;
}
