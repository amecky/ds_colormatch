#pragma once
#include <diesel.h>
#include <SpriteBatchBuffer.h>

namespace font {

	void renderText(const ds::vec2& pos, const char* txt, SpriteBatchBuffer* buffer, RID textureID);

	ds::vec2 textSize(const char* txt);
}

namespace input {

	bool convertMouse2Grid(int* gridX, int* gridY);
}