#pragma once
#include <diesel.h>
#include <vector>
#include <Windows.h>

inline void log(char* format, ...) {
	va_list args;
	va_start(args, format);
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	int written = vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
	OutputDebugString(buffer);
	OutputDebugString("\n");
	va_end(args);

}

namespace ds {

	struct p2i {

		int x;
		int y;

		p2i() : x(0), y(0) {}
		explicit p2i(int v) : x(v), y(v) {}
		p2i(int _x, int _y) : x(_x), y(_y) {}
		p2i(const p2i& other) : x(other.x), y(other.y) {}
		//p2i(const ds::vec2& v) : x(v.x), y(v.y) {}

		void operator=(const p2i& other) {
			x = other.x;
			y = other.y;
		}
		bool operator==(const p2i& other) {
			return x == other.x && y == other.y;
		}
	};

	

// -------------------------------------------------------
// Dropped Cell
// -------------------------------------------------------
template<class T>
struct DroppedCell {

	ds::vec2 from;
	ds::vec2 to;
	T data;
};
// ------------------------------------------------
// Grid
// ------------------------------------------------
template<class T>
class Grid {

struct GridNode {
	p2i v;
    T data;
    bool used;

	GridNode() : v(-1, -1), used(false) {}

	GridNode(const GridNode& other) : v(other.v), data(other.data), used(other.used) {}

};

public:
    Grid(int width,int height) : m_Width(width), m_Height(height) {
		m_Size = width * height;
		m_Data = new GridNode[m_Size];
		for (int x = 0; x < width; ++x) {
			for (int y = 0; y < height; ++y) {
				int index = getIndex(x, y);
				GridNode* node = &m_Data[index];
				node->v = p2i(x, y);
				node->used = false;
			}
		}
		_used = new p2i[m_Size];
		_helper = new p2i[m_Size];
		_numUsed = 0;
		_validMoves = 0;
	}

    virtual ~Grid() {
		delete[] _helper;
        delete[] m_Data;
		delete[] _used;
    }

	// ------------------------------------------------
	// Clears the entire grid with given object
	// ------------------------------------------------
	void clear() {
		for (int i = 0; i < m_Size; ++i) {
			m_Data[i].used = false;
		}
	}

	// ------------------------------------------------
	// Clears the entire grid with given object
	// ------------------------------------------------
	void clear(const T& t) {
		for (int i = 0; i < m_Size; ++i) {
			m_Data[i].data = t;
			m_Data[i].used = true;
		}
	}


	// ------------------------------------------------
	// Gets the object at given position
	// ------------------------------------------------
	const T& get(int x, int y) const {
		int idx = getIndex(x, y);
		return m_Data[idx].data;
	}

	// ------------------------------------------------
	// Gets the object at given position
	// ------------------------------------------------
	T& get(const p2i& p) {
		int idx = getIndex(p);
		return m_Data[idx].data;
	}

	// ------------------------------------------------
	// Gets the object at given position
	// ------------------------------------------------
	const T& get(const p2i& p) const {
		int idx = getIndex(p);
		return m_Data[idx].data;
	}

	// ------------------------------------------------
	// Gets the object at given position
	// ------------------------------------------------
	T& get(int x, int y) {
		int idx = getIndex(x, y);
		return m_Data[idx].data;
	}
	// ------------------------------------------------
	//
	// ------------------------------------------------
	void set(int x, int y, const T& t) {
		int idx = getIndex(x, y);
		if (idx != -1) {
			GridNode* node = &m_Data[idx];
			node->data = t;
			node->used = true;
		}
	}

	// ------------------------------------------------
	// remove
	// ------------------------------------------------
	bool remove(int x, int y) {
		int idx = getIndex(x, y);
		if (idx != -1 && m_Data[idx].used) {
			m_Data[idx].used = false;
			return true;
		}
		return false;
	}

	// ------------------------------------------------
	// fill gaps
	// ------------------------------------------------
	void fillGaps() {
		int moved = 0;
		for (int i = 0; i < m_Width - 1; ++i) {
			if (isColumnEmpty(i)) {
				int end = i + 1;
				while (isColumnEmpty(end)) {
					++end;
				}
				shiftColumns(i,end);
			}
		}
	}
	// ------------------------------------------------
	// Remove grid points
	// ------------------------------------------------
	void remove(p2i* points, int num, bool shift) {
		for (int i = 0; i < num; ++i) {
			const p2i& gp = points[i];
			remove(gp.x, gp.y);
		}
		fillGaps();
		calculateValidMoves();
	}

    const int width() const {
        return m_Width;
    }

    const int height() const {
        return m_Height;
    }

	bool isValid(int x, int y) const {
		return getIndex(x, y) != -1;
	}
	
	// ------------------------------------------------
	// Checks if cell is used
	// ------------------------------------------------
	bool isUsed(int x, int y) const {
		int idx = getIndex(x, y);
		if (idx != -1) {
			return m_Data[idx].used;
		}
		return false;
	}

	// ------------------------------------------------
	// find all matching neighbours
	// ------------------------------------------------
	int findMatchingNeighbours(int x, int y, p2i* ret, int max) {
		int cnt = 0;
		if (isUsed(x, y)) {
			cnt = simpleFindMatching(x, y, ret, max);
		}
		return cnt;
	}
	
	// ------------------------------------------------
	// fill row
	// ------------------------------------------------
	void fillRow(int row, const T& t) {
		if (row >= 0 && row < m_Height) {
			for (int x = 0; x < m_Width; ++x) {
				int idx = getIndex(x, row);
				if (idx != -1) {
					m_Data[idx].data = t;
					m_Data[idx].used = true;
				}
			}
		}
	}

	// ------------------------------------------------
	// copy row
	// ------------------------------------------------
	void copyRow(int oldRow, int newRow) {
		for (int x = 0; x < m_Width; ++x) {
			int oldIndex = getIndex(x, oldRow);
			int newIndex = getIndex(x, newRow);
			m_Data[newIndex] = m_Data[oldIndex];
		}
	}

	// ------------------------------------------------
	// copy column
	// ------------------------------------------------
	void copyColumn(int oldColumn, int newColumn) {
		for (int y = 0; y < m_Height; ++y) {
			int oldIndex = getIndex(oldColumn, y);
			int newIndex = getIndex(newColumn, y);
			if (m_Data[oldIndex].used) {
				m_Data[newIndex].data = m_Data[oldIndex].data;
				m_Data[newIndex].used = true;
			}
			else {
				m_Data[newIndex].used = false;
			}
		}
	}
	// ------------------------------------------------
	// shift columns
	// ------------------------------------------------
	void shiftColumns(int startColumn) {
		if (startColumn >= 0 && startColumn < m_Width) {
			int sx = startColumn - 1;
			if (sx < 0) {
				sx = 0;
			}
			for (int x = sx; x < m_Width - 1; ++x) {
				copyColumn(x + 1, x);
			}
			clearColumn(m_Width - 1);
		}
	}

	// ------------------------------------------------
	// shift columns
	// ------------------------------------------------
	void shiftColumns(int target, int source) {
		if (target >= 0 && target < m_Width) {
			copyColumn(source, target);
			clearColumn(source);
		}
	}
	// ------------------------------------------------
	// clear column
	// ------------------------------------------------
	void clearColumn(int column) {
		if (column >= 0 && column < m_Width) {
			for (int y = 0; y < m_Height; ++y) {
				int idx = getIndex(column, y);
				m_Data[idx].used = false;
			}
		}
	}
	// ------------------------------------------------
	// fill column
	// ------------------------------------------------
	void fillColumn(int column, const T& t) {
		for (int y = 0; y < m_Height; ++y) {
			int idx = getIndex(column, y);
			m_Data[idx].data = t;
			m_Data[idx].used = true;
		}
	}

	// ------------------------------------------------
	//
	// ------------------------------------------------
	T& operator() (int x, int y) {
		int index = getIndex(x, y);
		return m_Data[index].data;
	}

	// ------------------------------------------------
	//
	// ------------------------------------------------
	const T& operator() (int x, int y) const {
		int index = getIndex(x, y);
		return m_Data[index].data;
	}

	// ------------------------------------------------
	// Checks if cell is free
	// ------------------------------------------------
	bool isFree(int x, int y) const {
		int idx = getIndex(x, y);
		if (idx != -1) {
			return !m_Data[idx].used;
		}
		return false;
	}

	// -------------------------------------------------------
	// Is column empty
	// -------------------------------------------------------
	bool isColumnEmpty(int col) const {
		int count = 0;
		for (int i = 0; i < m_Height; ++i) {
			if (!isFree(col, i)) {
				++count;
			}
		}
		return count == 0;
	}

	// -------------------------------------------------------
	// Is row empty
	// -------------------------------------------------------
	bool isRowEmpty(int row) const {
		int count = 0;
		for (int i = 0; i < m_Width; ++i) {
			if (!isFree(i, row)) {
				++count;
			}
		}
		return count == 0;
	}

	// -------------------------------------------------------
	// swap
	// -------------------------------------------------------
	void swap(const p2i& first, const p2i& second) {
		int fi = getIndex(first);
		int si = getIndex(second);
		GridNode n = m_Data[fi];
		m_Data[fi] = m_Data[si];
		m_Data[si] = n;
	}
    
	
	// ------------------------------------------------
	// Drop row
	// ------------------------------------------------
	void dropRow(int x) {
		for (int y = (m_Height - 1); y >= 0; --y) {
			dropCell(x, y);
		}
	}

	// ------------------------------------------------
	// Drop cell
	// ------------------------------------------------
	void dropCell(int x, int y) {
		int idx = getIndex(x, y);
		if (isUsed(x, y)) {
			int finalY = 0;
			for (int yp = y + 1; yp < m_Height; ++yp) {
				if (isFree(x, yp)) {
					++finalY;
				}
				else {
					break;
				}
			}
			if (finalY != 0) {
				int nidx = getIndex(x, finalY);
				m_Data[nidx].data = m_Data[idx].data;
				m_Data[nidx].used = true;
				m_Data[idx].used = false;
			}
		}
	}

	// -------------------------------------------------------
	// Drop cells - remove empty cells in between
	// -------------------------------------------------------
	int dropCells(DroppedCell<T>* droppedCells, int num) {
		int cnt = 0;
		for (int x = 0; x < m_Width; ++x) {
			for (int y = 0; y < m_Height - 1; ++y) {
				if (isFree(x, y)) {
					int sy = y + 1;
					while (isFree(x, sy) && sy < m_Height - 1) {
						++sy;
					}
					if (isUsed(x, sy) && cnt < num) {
						DroppedCell<T> dc;
						dc.data = get(x, sy);
						dc.from = ds::vec2(x, sy);
						dc.to = ds::vec2(x, y);
						droppedCells[cnt++] = dc;
						set(x, y, get(x, sy));
						remove(x, sy);
					}
				}
			}
		}
		return cnt;
	}

	// ------------------------------------------------
	// is valid
	// ------------------------------------------------
	bool isValid(const p2i& p) const {
		return getIndex(p) != -1;
	}

	// ------------------------------------------------
	// get max row
	// ------------------------------------------------
	int getMaxRow() const {
		for (int y = m_Height - 1; y >= 0; --y) {
			if (!isRowEmpty(y)) {
				return y;
			}
		}
		return 0;
	}

	// ------------------------------------------------
	// get max column
	// ------------------------------------------------
	int getMaxColumn() const {
		for (int x = 0; x < m_Width; ++x) {
			if (isColumnEmpty(x)) {
				return x;
			}
		}
		return m_Width;
	}

	// ------------------------------------------------
	// get number of valid moves
	// ------------------------------------------------
	int getNumberOfMoves() const {
		return _validMoves;
	}

	// ------------------------------------------------
	// pick a random matching block 
	// ------------------------------------------------
	int getMatchingBlock(p2i* ret, int max) {
		int idx = ds::random(0, _numUsed);
		p2i p = _used[idx];
		return findMatchingNeighbours(p.x, p.y, ret, max);
	}

	// ------------------------------------------------
	// calculate valid moves
	// ------------------------------------------------
	void calculateValidMoves() {
		_numUsed = 0;
		_validMoves = 0;
		for (int x = 0; x < m_Width; ++x) {
			for (int y = 0; y < m_Height; ++y) {
				if (!isAlreadyProcessed(p2i(x, y), _used, _numUsed)) {
					if (isUsed(x, y)) {
						int num = simpleFindMatching(x, y, _helper, m_Size);
						if (num > 1) {
							++_validMoves;
							for (int i = 0; i < num; ++i) {
								const p2i& c = _helper[i];
								if (!isAlreadyProcessed(c, _used, _numUsed)) {
									_used[_numUsed++] = c;
								}
							}
						}
					}
				}
			}
		}
	}

protected:
    virtual bool isMatch(const T& first,const T& right) = 0;

private:
	// ------------------------------------------------
	// check if point is already in the used list
	// ------------------------------------------------
	bool isAlreadyProcessed(const p2i& p, p2i* array, int num) {
		for (int i = 0; i < num; ++i) {
			if (array[i] == p) {
				return true;
			}
		}
		return false;
	}

	// ------------------------------------------------
	// internal findMatching
	// ------------------------------------------------
	int simpleFindMatching(int x, int y, p2i* ret, int max) {
		_numMatches = 0;
		int idx = getIndex(x, y);
		if (idx != -1) {
			const GridNode& providedNode = m_Data[idx];
			if (isUsed(x - 1, y)) {
				findMatching(x - 1, y, providedNode, ret, max);
			}
			if (isUsed(x + 1, y)) {
				findMatching(x + 1, y, providedNode, ret, max);
			}
			if (isUsed(x, y - 1)) {
				findMatching(x, y - 1, providedNode, ret, max);
			}
			if (isUsed(x, y + 1)) {
				findMatching(x, y + 1, providedNode, ret, max);
			}
		}
		return _numMatches;
	}

	// ------------------------------------------------
	// internal findMatching
	// ------------------------------------------------
	void findMatching(int x, int y, const GridNode& providedNode, p2i* ret, int max) {
		int idx = getIndex(x, y);
		if (idx != -1) {
			const GridNode& currentNode = m_Data[idx];
			if (currentNode.used && isMatch(currentNode.data, providedNode.data)) {
				if (!isAlreadyProcessed(p2i(currentNode.v),ret,_numMatches)) {
					if (_numMatches < max) {
						ret[_numMatches++] = p2i(currentNode.v);
					}
					if (isUsed(x - 1, y)) {
						findMatching(x - 1, y, currentNode, ret, max);
					}
					if (isUsed(x + 1, y)) {
						findMatching(x + 1, y, currentNode, ret, max);
					}
					if (isUsed(x, y - 1)) {
						findMatching(x, y - 1, currentNode, ret, max);
					}
					if (isUsed(x, y + 1)) {
						findMatching(x, y + 1, currentNode, ret, max);
					}
				}
			}
		}
	}
	
	// ------------------------------------------------
	// Returns the index into the array if valid or -1
	// ------------------------------------------------
	int getIndex(int x, int y) const {
		if (x < 0 || x >= m_Width) {
			return -1;
		}
		if (y < 0 || y >= m_Height) {
			return -1;
		}
		return y * m_Width + x;
	}

	// ------------------------------------------------
	// Returns the index into std::vector if valid or -1
	// ------------------------------------------------
	int getIndex(const p2i& p) const {
		return getIndex(p.x, p.y);
	}

    int m_Width;
    int m_Height;
    int m_Size;
    GridNode* m_Data;
	p2i* _used;
	p2i* _helper;
	int _numUsed;
	int _numMatches;
	int _validMoves;
};

}

