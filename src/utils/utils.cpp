#include "utils.h"
#include "..\Constants.h"

namespace input {

	bool convertMouse2Grid(int* gridX, int* gridY) {
		ds::vec2 mousePos = ds::getMousePosition();
		float fy = (mousePos.y - static_cast<float>(STARTY - HALF_CELL_SIZE)) / static_cast<float>(CELL_SIZE);
		float fx = (mousePos.x - static_cast<float>(STARTX - HALF_CELL_SIZE)) / static_cast<float>(CELL_SIZE);
		if (fx >= 0.0f && fy >= 0.0f) {
			int mx = static_cast<int>(fx);
			int my = static_cast<int>(fy);
			if (mx < MAX_X && my < MAX_Y) {
				*gridX = mx;
				*gridY = my;
				return true;
			}
		}
		return false;
	}
}