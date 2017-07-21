#pragma once
#include <diesel.h>

const float MOVE_TTL = 1.2f;
const int CELL_SIZE = 40;
const int HALF_CELL_SIZE = 20;
const int STARTX = 130;
const int STARTY = 164;
const int MAX_X = 20;
const int MAX_Y = 12;
const int TOTAL = MAX_X * MAX_Y;

const float MESSAGE_TTL = 0.8f;
const float MESSAGE_SCALE = 0.4f;

// x-offset / width
const ds::vec2 FONT_DEF[] = {
	ds::vec2(0,35), // A
	ds::vec2(35,29),
	ds::vec2(66,27),
	ds::vec2(94,30),
	ds::vec2(125,27),
	ds::vec2(152,28),
	ds::vec2(180,29),
	ds::vec2(210,30),
	ds::vec2(240,12),
	ds::vec2(252,17),
	ds::vec2(269,28),
	ds::vec2(297,28),
	ds::vec2(325,39),
	ds::vec2(365,31), // N
	ds::vec2(396,29),
	ds::vec2(426,30),
	ds::vec2(456,29),
	ds::vec2(485,30),
	ds::vec2(515,27),
	ds::vec2(541,26),
	ds::vec2(567,29),
	ds::vec2(596,33),
	ds::vec2(629,41),
	ds::vec2(670,31),
	ds::vec2(701,32),
	ds::vec2(733,27)
};
