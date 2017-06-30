#pragma once
#include <diesel.h>
//#define DS_IMGUI_IMPLEMENTATION

namespace gui {

	struct IMGUISettings {
		ds::Color headerBoxColor;
		ds::Color buttonColor;
		ds::Color backgroundColor;
		ds::Color labelBoxColor;
		ds::Color enabledBoxColor;
		ds::Color disabledBoxColor;
		ds::Color activeInputBoxColor;
		ds::Color inputBoxColor;
		ds::Color boxBackgroundColor;
		ds::Color boxSelectionColor;
		ds::Color sliderColor;
		ds::Color scrollSliderColor;
		float lineSpacing;
	};

	void init(IMGUISettings* settings = 0);

	void start(const ds::vec2& pos);

	void begin(const char* header, int* state);

	void beginGroup();

	void endGroup();

	bool Button(const char* text);

	void Text(const char* text);

	void Label(const char* label, const char* text);

	void Value(const char* label, int v);

	void Value(const char* label, uint32_t v);

	void Value(const char* label, float v);

	void Value(const char* label, float v, const char* format);

	void Value(const char* label, const ds::vec2& v);

	void Value(const char* label, const ds::vec3& v);

	void Value(const char* label, const ds::vec4& v);

	void Value(const char* label, const ds::Color& v);

	void FormattedText(const char* fmt, ...);

	void Message(const char* fmt, ...);

	bool Input(const char* label, char* str, int maxLength);

	void Input(const char* label, int* v);

	void Input(const char* label, float* v);

	void Input(const char* label, ds::vec2* v);

	void Input(const char* label, ds::vec3* v);

	void Input(const char* label, ds::vec4* v);

	void Input(const char* label, ds::Color* v);

	void StepInput(const char* label, int* v, int minValue, int maxValue, int steps);

	void Checkbox(const char* label, bool* state);

	void ListBox(const char* label, const char** entries, int num, int* selected, int *offset, int max);

	void DropDownBox(const char* label, const char** entries, int num, int* state, int* selected, int *offset, int max, bool closeOnSelection = false);

	void Slider(const char* label, int* v, int minValue, int maxValue, float width = 200.0f);

	void Slider(const char* label, float* v, float minValue, float maxValue, int precision = 0, float width = 200.0f);

	void SliderAngle(const char* label, float* v, float width = 200.0f);

	void Separator();

	void Histogram(float* values, int num, float minValue, float maxValue, float step, float width = 200.0f, float height = 100.0f);

	void Diagram(float* values, int num, float minValue, float maxValue, float step, float width = 200.0f, float height = 100.0f);

	void Diagram(const char* label, float* values, int num, float minValue, float maxValue, float step, float width = 200.0f, float height = 100.0f);

	void beginMenu();

	bool MenuBar(const char* name);

	bool MenuItem(const char* label);

	void MenuBar(const char** names, int num, int* state);

	void endMenu();

	void end();

	void shutdown();

}

#ifdef DS_IMGUI_IMPLEMENTATION
#include <SpriteBatchBuffer.h>
#include <string.h>
#include <stdarg.h>

const unsigned char DS_IMGUI_FONT[128][16] = {
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x60, 0x60, 0x06, 0x1B, 0x3C, 0x6C, 0xC0, 0x01, 0x06, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0xF0, 0x60, 0x06, 0x1B, 0x66, 0x6C, 0x61, 0x03, 0x06, 0x18, 0x60, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0xF0, 0x60, 0x86, 0x3F, 0x06, 0xB8, 0x61, 0x03, 0x06, 0x18, 0x60, 0xC0, 0x06, 0x06, 0x00 },
	{ 0x00, 0xF0, 0x00, 0x00, 0x1B, 0x0C, 0xC0, 0xC0, 0x01, 0x00, 0x0C, 0xC0, 0x80, 0x03, 0x06, 0x00 },
	{ 0x00, 0x60, 0x00, 0x00, 0x1B, 0x18, 0x60, 0x60, 0x00, 0x00, 0x0C, 0xC0, 0xE0, 0x8F, 0x1F, 0x00 },
	{ 0x00, 0x60, 0x00, 0x00, 0x1B, 0x30, 0x30, 0x60, 0x0F, 0x00, 0x0C, 0xC0, 0x80, 0x03, 0x06, 0x00 },
	{ 0x00, 0x00, 0x00, 0x80, 0x3F, 0x60, 0xD8, 0x61, 0x06, 0x00, 0x0C, 0xC0, 0xC0, 0x06, 0x06, 0x00 },
	{ 0x00, 0x60, 0x00, 0x00, 0x1B, 0x66, 0x68, 0x63, 0x06, 0x00, 0x0C, 0xC0, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x60, 0x00, 0x00, 0x1B, 0x3C, 0x60, 0xC3, 0x0D, 0x00, 0x18, 0x60, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0xC0, 0x01, 0x00, 0x00, 0x18, 0x60, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x18, 0x78, 0xC0, 0xC0, 0x03, 0x0F, 0x0C, 0xF8, 0x81, 0x83, 0x1F, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x18, 0xCC, 0xE0, 0x60, 0x86, 0x19, 0x0C, 0x18, 0x80, 0x01, 0x18, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x0C, 0xEC, 0xF8, 0x60, 0x86, 0x19, 0x6C, 0x18, 0xC0, 0x00, 0x0C, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x0C, 0xEC, 0xC0, 0x00, 0x06, 0x18, 0x6C, 0x18, 0xE0, 0x03, 0x0C, 0x00 },
	{ 0x00, 0xF8, 0x01, 0x00, 0x06, 0xCC, 0xC0, 0x00, 0x03, 0x0E, 0x6C, 0xF8, 0x60, 0x06, 0x06, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x06, 0xDC, 0xC0, 0x80, 0x01, 0x18, 0x66, 0x80, 0x61, 0x06, 0x06, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x03, 0xDC, 0xC0, 0xC0, 0x80, 0x19, 0xFE, 0x80, 0x61, 0x06, 0x03, 0x00 },
	{ 0x38, 0x00, 0x80, 0x03, 0x03, 0xCC, 0xC0, 0x60, 0x80, 0x19, 0x60, 0xC0, 0x60, 0x06, 0x03, 0x00 },
	{ 0x38, 0x00, 0x80, 0x83, 0x01, 0x78, 0xC0, 0xE0, 0x07, 0x0F, 0x60, 0x78, 0xC0, 0x03, 0x03, 0x00 },
	{ 0x30, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x3C, 0xF0, 0x00, 0x00, 0x00, 0x60, 0x00, 0x60, 0x00, 0x0F, 0x7E, 0x60, 0xE0, 0x03, 0x0F, 0x00 },
	{ 0x66, 0x98, 0x01, 0x00, 0x00, 0x30, 0x00, 0xC0, 0x80, 0x19, 0xC3, 0xF0, 0x60, 0x86, 0x19, 0x00 },
	{ 0x66, 0x98, 0x81, 0x03, 0x0E, 0x18, 0x00, 0x80, 0x81, 0x19, 0xC3, 0x98, 0x61, 0x86, 0x19, 0x00 },
	{ 0x6E, 0x98, 0x81, 0x03, 0x0E, 0x0C, 0xF8, 0x01, 0x03, 0x0C, 0xF3, 0x98, 0x61, 0x86, 0x01, 0x00 },
	{ 0x3C, 0x98, 0x01, 0x00, 0x00, 0x06, 0x00, 0x00, 0x06, 0x06, 0xDB, 0x98, 0xE1, 0x83, 0x01, 0x00 },
	{ 0x76, 0xF0, 0x01, 0x00, 0x00, 0x0C, 0xF8, 0x01, 0x03, 0x06, 0xDB, 0xF8, 0x61, 0x86, 0x01, 0x00 },
	{ 0x66, 0xC0, 0x00, 0x00, 0x00, 0x18, 0x00, 0x80, 0x01, 0x00, 0xF3, 0x98, 0x61, 0x86, 0x19, 0x00 },
	{ 0x66, 0x60, 0x80, 0x03, 0x0E, 0x30, 0x00, 0xC0, 0x00, 0x06, 0x03, 0x98, 0x61, 0x86, 0x19, 0x00 },
	{ 0x3C, 0x70, 0x80, 0x03, 0x0E, 0x60, 0x00, 0x60, 0x00, 0x06, 0xFE, 0x98, 0xE1, 0x03, 0x0F, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x1E, 0xF8, 0xE1, 0x07, 0x0F, 0x66, 0xF0, 0x00, 0x86, 0x19, 0x06, 0x18, 0x63, 0x0C, 0x0F, 0x00 },
	{ 0x36, 0x18, 0x60, 0x80, 0x19, 0x66, 0x60, 0x00, 0x86, 0x19, 0x06, 0x18, 0x63, 0x8C, 0x19, 0x00 },
	{ 0x66, 0x18, 0x60, 0x80, 0x19, 0x66, 0x60, 0x00, 0x86, 0x0D, 0x06, 0xB8, 0xE3, 0x8C, 0x19, 0x00 },
	{ 0x66, 0x18, 0x60, 0x80, 0x01, 0x66, 0x60, 0x00, 0x86, 0x0D, 0x06, 0x58, 0xE3, 0x8D, 0x19, 0x00 },
	{ 0x66, 0xF8, 0xE0, 0x83, 0x01, 0x7E, 0x60, 0x00, 0x86, 0x07, 0x06, 0x58, 0x63, 0x8F, 0x19, 0x00 },
	{ 0x66, 0x18, 0x60, 0x80, 0x1D, 0x66, 0x60, 0x00, 0x86, 0x0D, 0x06, 0x58, 0x63, 0x8E, 0x19, 0x00 },
	{ 0x66, 0x18, 0x60, 0x80, 0x19, 0x66, 0x60, 0x60, 0x86, 0x0D, 0x06, 0x18, 0x63, 0x8C, 0x19, 0x00 },
	{ 0x36, 0x18, 0x60, 0x80, 0x19, 0x66, 0x60, 0x60, 0x86, 0x19, 0x06, 0x18, 0x63, 0x8C, 0x19, 0x00 },
	{ 0x1E, 0xF8, 0x61, 0x00, 0x1F, 0x66, 0xF0, 0xC0, 0x83, 0x19, 0x7E, 0x18, 0x63, 0x0C, 0x0F, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x3E, 0xF0, 0xE0, 0x03, 0x0F, 0x7E, 0x98, 0x61, 0x86, 0x31, 0x66, 0x98, 0xE1, 0x07, 0x0F, 0x00 },
	{ 0x66, 0x98, 0x61, 0x86, 0x19, 0x18, 0x98, 0x61, 0x86, 0x31, 0x66, 0x98, 0x01, 0x06, 0x03, 0x00 },
	{ 0x66, 0x98, 0x61, 0x86, 0x01, 0x18, 0x98, 0x61, 0x86, 0x31, 0x2C, 0x98, 0x01, 0x06, 0x03, 0x00 },
	{ 0x66, 0x98, 0x61, 0x06, 0x03, 0x18, 0x98, 0x61, 0x86, 0x35, 0x18, 0x98, 0x01, 0x03, 0x03, 0x00 },
	{ 0x3E, 0x98, 0xE1, 0x03, 0x06, 0x18, 0x98, 0x61, 0x86, 0x35, 0x18, 0xF0, 0x80, 0x01, 0x03, 0x00 },
	{ 0x06, 0x98, 0x61, 0x03, 0x0C, 0x18, 0x98, 0x61, 0x86, 0x35, 0x34, 0x60, 0xC0, 0x00, 0x03, 0x00 },
	{ 0x06, 0x98, 0x61, 0x06, 0x18, 0x18, 0x98, 0x61, 0x06, 0x1B, 0x66, 0x60, 0x60, 0x00, 0x03, 0x00 },
	{ 0x06, 0x98, 0x61, 0x86, 0x19, 0x18, 0x98, 0xC1, 0x03, 0x1B, 0x66, 0x60, 0x60, 0x00, 0x03, 0x00 },
	{ 0x06, 0xF0, 0x60, 0x06, 0x0F, 0x18, 0xF0, 0x80, 0x01, 0x1B, 0x66, 0x60, 0xE0, 0x07, 0x03, 0x00 },
	{ 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00 },
	{ 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x80, 0x01, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0xC0, 0x03, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x06, 0xF0, 0x60, 0x06, 0x00, 0x30, 0x00, 0x60, 0x00, 0x00, 0x60, 0x00, 0x80, 0x07, 0x00, 0x00 },
	{ 0x06, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x60, 0x00, 0xC0, 0x00, 0x00, 0x00 },
	{ 0x0C, 0xC0, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xE0, 0x03, 0x0F, 0x7C, 0xF0, 0xC0, 0x00, 0x1F, 0x00 },
	{ 0x0C, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x80, 0x61, 0x86, 0x19, 0x66, 0x98, 0xC1, 0x80, 0x19, 0x00 },
	{ 0x18, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x80, 0x61, 0x86, 0x01, 0x66, 0x98, 0xE1, 0x87, 0x19, 0x00 },
	{ 0x18, 0xC0, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x61, 0x86, 0x01, 0x66, 0xF8, 0xC1, 0x80, 0x19, 0x00 },
	{ 0x30, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x98, 0x61, 0x86, 0x01, 0x66, 0x18, 0xC0, 0x80, 0x19, 0x00 },
	{ 0x30, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x98, 0x61, 0x86, 0x19, 0x66, 0x18, 0xC0, 0x80, 0x19, 0x00 },
	{ 0x60, 0xC0, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xE1, 0x03, 0x0F, 0x7C, 0xF0, 0xC0, 0x00, 0x1F, 0x00 },
	{ 0x60, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00 },
	{ 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00 },
	{ 0x00, 0xF0, 0x00, 0xC0, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x0F, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x60, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x06, 0x60, 0x00, 0x83, 0x01, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x06, 0x00, 0x00, 0x80, 0x01, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x3E, 0x78, 0xC0, 0x83, 0x19, 0x18, 0xF8, 0xE1, 0x03, 0x0F, 0x3E, 0xF0, 0x61, 0x06, 0x1F, 0x00 },
	{ 0x66, 0x60, 0x00, 0x83, 0x19, 0x18, 0x58, 0x63, 0x86, 0x19, 0x66, 0x98, 0x61, 0x87, 0x01, 0x00 },
	{ 0x66, 0x60, 0x00, 0x83, 0x0D, 0x18, 0x58, 0x63, 0x86, 0x19, 0x66, 0x98, 0xE1, 0x80, 0x01, 0x00 },
	{ 0x66, 0x60, 0x00, 0x83, 0x07, 0x18, 0x58, 0x63, 0x86, 0x19, 0x66, 0x98, 0x61, 0x00, 0x0F, 0x00 },
	{ 0x66, 0x60, 0x00, 0x83, 0x0D, 0x18, 0x58, 0x63, 0x86, 0x19, 0x66, 0x98, 0x61, 0x00, 0x18, 0x00 },
	{ 0x66, 0x60, 0x00, 0x83, 0x19, 0x18, 0x58, 0x63, 0x86, 0x19, 0x66, 0x98, 0x61, 0x00, 0x18, 0x00 },
	{ 0x66, 0xF8, 0x01, 0x83, 0x19, 0x7E, 0x18, 0x63, 0x06, 0x0F, 0x3E, 0xF0, 0x61, 0x80, 0x0F, 0x00 },
	{ 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x80, 0x01, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x80, 0x01, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0xE0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x80, 0x01, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x18, 0x30, 0xE0, 0x88, 0x1F, 0x00 },
	{ 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x18, 0x60, 0xB0, 0x8D, 0x1F, 0x00 },
	{ 0x7E, 0x98, 0x61, 0x86, 0x31, 0x66, 0x98, 0xE1, 0x07, 0x06, 0x18, 0x60, 0x10, 0x87, 0x1F, 0x00 },
	{ 0x0C, 0x98, 0x61, 0x86, 0x35, 0x66, 0x98, 0x01, 0x06, 0x06, 0x18, 0x60, 0x00, 0x80, 0x1F, 0x00 },
	{ 0x0C, 0x98, 0x61, 0x86, 0x35, 0x3C, 0x98, 0x01, 0x03, 0x03, 0x18, 0xC0, 0x00, 0x80, 0x1F, 0x00 },
	{ 0x0C, 0x98, 0x61, 0x86, 0x35, 0x18, 0x98, 0x81, 0x81, 0x01, 0x18, 0x80, 0x01, 0x80, 0x1F, 0x00 },
	{ 0x0C, 0x98, 0x61, 0x86, 0x35, 0x3C, 0x98, 0xC1, 0x00, 0x03, 0x18, 0xC0, 0x00, 0x80, 0x1F, 0x00 },
	{ 0x0C, 0x98, 0xC1, 0x03, 0x1B, 0x66, 0x98, 0x61, 0x00, 0x06, 0x18, 0x60, 0x00, 0x80, 0x1F, 0x00 },
	{ 0x78, 0xF0, 0x81, 0x01, 0x1B, 0x66, 0xF0, 0xE0, 0x07, 0x06, 0x18, 0x60, 0x00, 0x80, 0x1F, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x06, 0x18, 0x60, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x0C, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
};

namespace gui {

	typedef uint32_t HashedId;

	static HashedId NULL_HASH = 0;

	HashedId HashId(const char *id) {
		// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
		HashedId hash = 0x84222325;
		while (*id) hash = (hash ^ static_cast<uint8_t>(*id++)) * 0x000001b3;
		return hash;
	}

	HashedId HashPointer(const void *ptr) {
		HashedId hash = static_cast<HashedId>(reinterpret_cast<size_t>(ptr)) * 2654435761;
		return hash;
	}

	const static ds::vec4 INPUT_RECT = ds::vec4(0, 128, 150, 20);
	const static ds::vec4 WHITE_RECT = ds::vec4(0, 128, 128, 128);

	struct DrawCall {
		ds::vec2 pos;
		ds::vec2 size;
		ds::vec4 rect;
		ds::vec2 scale;
		ds::Color color;
		bool resize;
		int groupIndex;
	};
	
	static const IMGUISettings DEFAULT_SETTINGS = {
		ds::Color(40,117,114, 255), // header
		ds::Color(68, 68, 68, 255), // button
		ds::Color(16, 16, 16, 255), // background
		ds::Color(32, 32, 32, 255), // label 
		ds::Color(0, 192, 0, 255), // enabled
		ds::Color(192, 0, 0, 255), // disabled
		ds::Color(64,64,64,255), // active input
		ds::Color(32,32,32,255), // input
		ds::Color(38,38,38,255), // box background color
		ds::Color(167,77,75,255), // box selection color
		ds::Color(96,96,96,255), // slider color
		ds::Color(255,0,255,255), // scroll slider color
		5.0f // line spacing
	};

	struct GUIContext {
		RID textureID;
		SpriteBatchBuffer* buffer;
		std::vector<DrawCall> calls;
		bool clicked;
		ds::vec2 mousePos;
		bool buttonPressed;
		int keyInput[256];
		int keys[32];
		int numKeys;
		ds::vec2 startPos;
		ds::vec2 currentPos;
		bool grouping;
		char inputText[32];
		char tmpBuffer[256];
		HashedId active;
		int caretPos;
		ds::vec2 size;
		ds::vec2 menuPosition;
		int currentGoupIndex;
		int activeGroup;
		float itemOffset;
		IMGUISettings settings;
	};

	static GUIContext* _guiCtx = 0;

	// -------------------------------------------------------
	// check if mouse cursor is inside box
	// -------------------------------------------------------
	static bool isCursorInside(const ds::vec2& p, const ds::vec2& dim) {
		//ds::vec2 mp = ds::getMousePosition();
		ds::vec2 mp = _guiCtx->mousePos;
		if (mp.x < (p.x - dim.x * 0.5f)) {
			return false;
		}
		if (mp.x >(p.x + dim.x * 0.5f)) {
			return false;
		}
		if (mp.y < (p.y - dim.y * 0.5f)) {
			return false;
		}
		if (mp.y >(p.y + dim.y * 0.5f)) {
			return false;
		}
		return true;
	}

	// -------------------------------------------------------
	// handle mouse interaction
	// -------------------------------------------------------
	static bool isClicked(const ds::vec2& pos, const ds::vec2& size) {
		if (_guiCtx->clicked) {
			ds::vec2 p = pos;
			p.x += size.x * 0.5f + _guiCtx->itemOffset;
			if (_guiCtx->clicked && isCursorInside(p, size)) {
				return true;
			}
		}
		return false;
	}

	// -------------------------------------------------------
	// handle mouse interaction
	// -------------------------------------------------------
	static bool isHovered(const ds::vec2& pos, const ds::vec2& size) {
		ds::vec2 p = pos;
		p.x += size.x * 0.5f + _guiCtx->itemOffset;
		return isCursorInside(p, size);
	}

	// -------------------------------------------------------
	// is box selected
	// -------------------------------------------------------
	static bool isBoxSelected(HashedId id, const ds::vec2& pos, const ds::vec2& size, bool setActive = true) {
		if (_guiCtx->clicked) {
			ds::vec2 p = pos;
			p.x += size.x * 0.5f + _guiCtx->itemOffset;
			if (_guiCtx->clicked && isCursorInside(p, size)) {
				if (setActive) {
					_guiCtx->active = id;
					_guiCtx->clicked = false;
					_guiCtx->activeGroup = _guiCtx->currentGoupIndex;
				}
				return true;
			}
		}
		return false;
	}

	// -------------------------------------------------------
	// determine text size
	// -------------------------------------------------------
	ds::vec2 textSize(const char* txt) {
		int l = strlen(txt);
		ds::vec2 p(0.0f);
		for (int i = 0; i < l; ++i) {
			int idx = (int)txt[i] - 32;
			if (idx >= 0 && idx < 127) {
				int c = idx / 16;
				int r = c - idx * 16;
				ds::vec4 rect = ds::vec4(r, c, 7, 10);
				p.x += rect.z;
				if (rect.w > p.y) {
					p.y = rect.w;
				}
			}
		}
		return p;
	}

	// -------------------------------------------------------
	// determine text size
	// -------------------------------------------------------
	ds::vec2 limitedTextSize(const char* txt, int maxLength) {
		int l = strlen(txt);
		if (l > maxLength) {
			l = maxLength;
		}
		ds::vec2 p(0.0f);
		for (int i = 0; i < l; ++i) {
			int idx = (int)txt[i] - 32;
			if (idx >= 0 && idx < 127) {
				int c = idx / 16;
				int r = c - idx * 16;
				ds::vec4 rect = ds::vec4(r, c, 7, 10);
				p.x += rect.z;
				if (rect.w > p.y) {
					p.y = rect.w;
				}
			}
		}
		return p;
	}

	// -------------------------------------------------------
	// initialize GUI
	// -------------------------------------------------------
	void init(IMGUISettings* settings) {
		_guiCtx = new GUIContext;	
		_guiCtx->clicked = false;
		_guiCtx->buttonPressed = false;
		for (int i = 0; i < 255; ++i) {
			_guiCtx->keyInput[i] = 0;
		}		
		_guiCtx->grouping = false;
		_guiCtx->numKeys = 0;
		_guiCtx->currentGoupIndex = 0;
		_guiCtx->activeGroup = -1;
		_guiCtx->itemOffset = 10.0f;
		if (settings != 0) {
			_guiCtx->settings = *settings;
		}
		else {
			_guiCtx->settings = DEFAULT_SETTINGS;
		}
		uint8_t* data = new uint8_t[256 * 256 * 4];
		for (int i = 0; i < 256 * 256 * 4; ++i) {
			data[i] = 255;
		}
		int index = 0;
		for (int y = 0; y < 128; ++y) {
			for (int x = 0; x < 16; ++x) {
				uint8_t t = DS_IMGUI_FONT[y][x];
				index = x * 32 + y * 256 * 4;
				for (int z = 0; z < 8; ++z) {
					if ((t >> z) & 1) {
						data[index++] = 255;
						data[index++] = 255;
						data[index++] = 255;
						data[index++] = 255;
					}
					else {
						data[index++] = 0;
						data[index++] = 0;
						data[index++] = 0;
						data[index++] = 0;
					}
				}
			}
		}

		ds::TextureInfo texInfo = { 256, 256, 4, data, ds::TextureFormat::R8G8B8A8_UNORM, ds::BindFlag::BF_SHADER_RESOURCE };
		RID textureID = ds::createTexture(texInfo);
		SpriteBatchBufferInfo sbbInfo = { 1024,textureID, ds::TextureFilters::POINT };
		_guiCtx->buffer = new SpriteBatchBuffer(sbbInfo);
	}

	// --------------------------------------------------------
	// internal add text method
	// --------------------------------------------------------
	static void addText(const ds::vec2& pos, const char* text, float xoffset = 10, const ds::Color& color = ds::Color(1.0f, 1.0f, 1.0f, 1.0f)) {
		int l = strlen(text);
		ds::vec2 p = pos;
		p.x += xoffset;
		for (int i = 0; i < l; ++i) {
			int idx = (int)text[i] - 32;
			if (idx >= 0 && idx < 127) {
				int c = text[i] - 32;
				int t = c / 12;
				int y = t * 16;
				int x = (c - t * 12) * 10;
				ds::vec4 rect = ds::vec4(x, y, 8, 14);
				DrawCall call;
				call.pos = p;
				call.rect = rect;
				call.color = color;
				call.resize = false;
				call.scale = ds::vec2(1.0f, 1.0f);
				call.groupIndex = _guiCtx->currentGoupIndex;
				_guiCtx->calls.push_back(call);
				p.x += rect.z;
			}
		}
		if (p.x > _guiCtx->size.x) {
			_guiCtx->size.x = p.x;
		}
	}

	// --------------------------------------------------------
	// internal add box
	// --------------------------------------------------------
	static void addBox(const ds::vec2& p, const ds::vec2& size, const ds::Color& color, bool resize = false) {
		DrawCall call;
		call.pos = p;
		call.pos.x += size.x * 0.5f;
		ds::vec2 scale = ds::vec2(1.0f, 1.0f);
		ds::vec2 sz = size;
		if (size.x > WHITE_RECT.z) {
			sz.x = WHITE_RECT.z;
			scale.x = size.x / WHITE_RECT.z;
		}
		if (size.y > WHITE_RECT.w) {
			sz.y = WHITE_RECT.w;
			scale.y = size.y / WHITE_RECT.w;
		}
		call.rect = ds::vec4(256.0f, 0.0f, sz.x, sz.y);
		call.color = color;
		call.resize = resize;
		call.scale = scale;
		call.groupIndex = _guiCtx->currentGoupIndex;
		_guiCtx->calls.push_back(call);
		if ((p.x + size.x) > _guiCtx->size.x) {
			_guiCtx->size.x = (p.x + size.x);
		}
	}

	static void addBox(const ds::vec2& p, int width, int height, const ds::Color& color, bool resize = false) {
		addBox(p, ds::vec2(width, height), color, resize);
	}

	// --------------------------------------------------------
	// move current position to next position
	// --------------------------------------------------------
	static void moveForward(const ds::vec2& dim) {
		if (_guiCtx->grouping) {
			_guiCtx->currentPos.x += dim.x + 10.0f;
		}
		else {
			_guiCtx->currentPos.y -= dim.y;
			_guiCtx->size.y += dim.y;
			_guiCtx->currentPos.x = _guiCtx->startPos.x;
		}
		++_guiCtx->currentGoupIndex;
	}

	// -------------------------------------------------------
	// handle text input
	// -------------------------------------------------------
	// FIXME: provide numeric input only
	static bool handleTextInput(bool numeric = false) {
		bool ret = false;
		int len = strlen(_guiCtx->inputText);
		for (int i = 0; i < ds::getNumInputKeys(); ++i) {
			const ds::InputKey& key = ds::getInputKey(i);
			if (key.type == ds::IKT_SYSTEM) {
				if (key.value == ds::SpecialKeys::DSKEY_Backspace) {
					if (_guiCtx->caretPos > 0) {
						if (_guiCtx->caretPos < len) {
							memmove(_guiCtx->inputText + _guiCtx->caretPos - 1, _guiCtx->inputText + _guiCtx->caretPos, len - _guiCtx->caretPos);
						}
						--_guiCtx->caretPos;
						--len;
						_guiCtx->inputText[len] = '\0';

					}
				}
				else if (key.value == ds::SpecialKeys::DSKEY_LeftArrow) {
					if (_guiCtx->caretPos > 0) {
						--_guiCtx->caretPos;
					}
				}
				else if (key.value == ds::SpecialKeys::DSKEY_RightArrow) {
					if (_guiCtx->caretPos < strlen(_guiCtx->inputText)) {
						++_guiCtx->caretPos;
					}
				}
				else if (key.value == ds::SpecialKeys::DSKEY_Home) {
					_guiCtx->caretPos = 0;
				}
				else if (key.value == ds::SpecialKeys::DSKEY_End) {
					_guiCtx->caretPos = strlen(_guiCtx->inputText);
				}
				else if (key.value == ds::SpecialKeys::DSKEY_Enter) {
					_guiCtx->active = -1;
					_guiCtx->activeGroup = -1;
					ret = true;
				}
				else if (key.value == ds::SpecialKeys::DSKEY_Delete) {
					if (len > 0) {
						if (_guiCtx->caretPos < len) {
							memmove(_guiCtx->inputText + _guiCtx->caretPos, _guiCtx->inputText + _guiCtx->caretPos + 1, len - _guiCtx->caretPos);
							--len;
							_guiCtx->inputText[len] = '\0';
						}
					}
				}
			}
			else {
				if ((key.value > 47 && key.value < 128) || key.value == '.' || key.value == '-') {
					if (len < 32) {
						if (_guiCtx->caretPos < len) {
							memmove(_guiCtx->inputText + _guiCtx->caretPos + 1, _guiCtx->inputText + _guiCtx->caretPos, len - _guiCtx->caretPos);
						}
						if (numeric) {
							if ((key.value > 47 && key.value < 58) || key.value == '.' || key.value == '-') {
								_guiCtx->inputText[_guiCtx->caretPos] = key.value;
								++len;
								++_guiCtx->caretPos;
							}
						}
						else {
							_guiCtx->inputText[_guiCtx->caretPos] = key.value;
							++len;
							++_guiCtx->caretPos;
						}
					}
				}
			}
			_guiCtx->inputText[len] = '\0';
		}
		//++len;		
		return ret;
	}
	// -------------------------------------------------------
	// input scalar
	// -------------------------------------------------------
	static bool InputScalar(int id, int index, char* v, int maxLength, float width = 150.0f) {
		int new_id = id + 1024 * index;
		bool ret = false;
		ds::vec2 p = _guiCtx->currentPos;
		bool selected = isBoxSelected(new_id, p, ds::vec2(width, 20.0f));
		if (selected) {
			sprintf_s(_guiCtx->inputText, 32, "%s", v);
			_guiCtx->caretPos = strlen(_guiCtx->inputText);
			_guiCtx->active = new_id;
		}
		if (_guiCtx->active == new_id) {
			addBox(p, ds::vec2(width, 16.0f), _guiCtx->settings.activeInputBoxColor);
			ret = handleTextInput(false);
			strncpy(v, _guiCtx->inputText, maxLength);
			ds::vec2 textDim = textSize(_guiCtx->inputText);
			ds::vec2 cp = p;
			ds::vec2 cursorPos = limitedTextSize(_guiCtx->inputText, _guiCtx->caretPos);
			cp.x += (width - textDim.x) * 0.5f + cursorPos.x - 2.0f;
			cp.y -= 6.0f;
			addBox(cp, ds::vec2(10.0f, 3.0f), ds::Color(192, 0, 192, 255));
			p.x += (width - textDim.x) * 0.5f;
			p.y -= 1.0f;
			addText(p, _guiCtx->inputText, 0.0f);
			sprintf_s(v, maxLength, "%s", _guiCtx->inputText);
		}
		else {
			sprintf_s(_guiCtx->tmpBuffer, 64, "%s", v);
			addBox(p, ds::vec2(width, 16.0f), _guiCtx->settings.inputBoxColor);
			ds::vec2 textDim = textSize(_guiCtx->tmpBuffer);
			p.y -= 1.0f;
			p.x += (width - textDim.x) * 0.5f;
			addText(p, _guiCtx->tmpBuffer, 0.0f);
		}
		return ret;
	}

	// -------------------------------------------------------
	// internal input float
	// -------------------------------------------------------
	static bool InputScalar(int id, int index, float* v, const char* format = "%g", float width = 100.0f) {
		int new_id = id + 1024 * index;
		bool ret = false;
		ds::vec2 p = _guiCtx->currentPos;
		if (index > 0) {
			p.x += index * width + index * 10.0f;
		}
		bool selected = isBoxSelected(new_id, p, ds::vec2(width, 20.0f));
		if (selected) {
			sprintf_s(_guiCtx->inputText, 32, format, *v);
			_guiCtx->caretPos = strlen(_guiCtx->inputText);
			_guiCtx->active = new_id;
		}
		if (_guiCtx->active == new_id) {
			addBox(p, width, 16,_guiCtx->settings.activeInputBoxColor);
			ret = handleTextInput(true);
			*v = static_cast<float>(atof(_guiCtx->inputText));
			ds::vec2 textDim = textSize(_guiCtx->inputText);
			ds::vec2 cp = p;
			ds::vec2 cursorPos = limitedTextSize(_guiCtx->inputText, _guiCtx->caretPos);
			cp.x += (width - textDim.x) * 0.5f + cursorPos.x - 2.0f;
			cp.y -= 6.0f;
			addBox(cp, ds::vec2(10.0f, 3.0f), ds::Color(192, 0, 192, 255));
			p.x += (width - textDim.x) * 0.5f;
			p.y -= 1.0f;
			addText(p, _guiCtx->inputText, 0.0f);
		}
		else {
			sprintf_s(_guiCtx->tmpBuffer, 64, format, *v);
			ds::vec2 textDim = textSize(_guiCtx->tmpBuffer);
			addBox(p, width, 16, _guiCtx->settings.inputBoxColor);
			p.y -= 1.0f;
			p.x += (width - textDim.x) * 0.5f;
			addText(p, _guiCtx->tmpBuffer, 0.0f);
		}
		return ret;
	}

	// --------------------------------------------------------
	// begin
	// --------------------------------------------------------
	void start(const ds::vec2& pos) {
		_guiCtx->calls.clear();
		_guiCtx->startPos = pos;
		_guiCtx->currentPos = pos;
		_guiCtx->size = ds::vec2(0.0f, 0.0f);
		_guiCtx->grouping = false;
		_guiCtx->mousePos = ds::getMousePosition();
		if (_guiCtx->clicked) {
			_guiCtx->clicked = false;
		}
		if (ds::isMouseButtonPressed(0)) {
			_guiCtx->buttonPressed = true;
		}
		else {
			if (_guiCtx->buttonPressed) {
				_guiCtx->clicked = true;				
			}
			_guiCtx->buttonPressed = false;
		}
	}

	// --------------------------------------------------------
	// begin with header
	// --------------------------------------------------------
	void begin(const char* header, int* state) {
		_guiCtx->currentGoupIndex = 0;
		ds::vec2 pos = _guiCtx->currentPos;
		addBox(pos, 10, 20, _guiCtx->settings.headerBoxColor, true);
		pos.x += 30.0f;
		addText(pos, header);

		pos = _guiCtx->currentPos;
		pos.x -= 10.0f;
		addBox(pos, 20, 20, _guiCtx->settings.buttonColor);
		if (*state == 0) {
			addText(pos, "+");
		}
		else {
			addText(pos, "-");
		}
		if (isClicked(pos, ds::vec2(20, 20))) {
			if (*state == 0) {
				*state = 1;
			}
			else {
				*state = 0;
			}
		}
		float advance = 20.0f + _guiCtx->settings.lineSpacing;
		moveForward(ds::vec2(10.0f, advance));
	}

	// --------------------------------------------------------
	// begin grouping
	// --------------------------------------------------------
	void beginGroup() {
		_guiCtx->grouping = true;
	}

	// --------------------------------------------------------
	// end grouping
	// --------------------------------------------------------
	void endGroup() {
		_guiCtx->grouping = false;
		moveForward(ds::vec2(10, 30));
	}

	// --------------------------------------------------------
	// Button
	// --------------------------------------------------------
	bool Button(const char* text) {
		ds::vec2 p = _guiCtx->currentPos;
		addBox(p, 120, 20, _guiCtx->settings.buttonColor);
		ds::vec2 dim = ds::vec2(120, 20);
		ds::vec2 textDim = textSize(text);
		p.x += (120.0f - textDim.x) * 0.5f;
		addText(p, text, 0.0f);
		dim.y = 20.0f;
		ds::vec2 buttonPos = _guiCtx->currentPos;
		moveForward(dim + ds::vec2(0.0f,_guiCtx->settings.lineSpacing));
		return isClicked(buttonPos, dim);
	}

	// --------------------------------------------------------
	// Button
	// --------------------------------------------------------
	void Checkbox(const char* label, bool* state) {
		ds::vec2 p = _guiCtx->currentPos;
		addBox(p, ds::vec2(16, 16),_guiCtx->settings.buttonColor);
		p.x += 4;
		if (*state) {			
			addBox(p, 8, 8, _guiCtx->settings.enabledBoxColor);
		}
		else {
			addBox(p, 8, 8, _guiCtx->settings.disabledBoxColor);
		}
		if (isClicked(_guiCtx->currentPos, ds::vec2(16, 16))) {
			*state = !*state;
		}
		p = _guiCtx->currentPos;
		p.x += 20.0f;
		addText(p, label);
		float advance = 16.0f + _guiCtx->settings.lineSpacing;
		moveForward(ds::vec2(40.0f, advance));
	}

	// --------------------------------------------------------
	// Label
	// --------------------------------------------------------
	void Label(const char* label, const char* text) {
		addBox(_guiCtx->currentPos, ds::vec2(150.0f, 20.0f), _guiCtx->settings.labelBoxColor);
		ds::vec2 textDim = textSize(text);

		ds::vec2 textPos = _guiCtx->currentPos;
		textPos.x += (160.0f - textDim.x) * 0.5f;
		addText(textPos, text, 0.0f);

		ds::vec2 labelPos = _guiCtx->currentPos;
		labelPos.x += 160.0f;
		addText(labelPos, label);
		ds::vec2 size = textSize(label);
		moveForward(ds::vec2(size.x, 30.0f));
	}

	// --------------------------------------------------------
	// Value - vec2
	// --------------------------------------------------------
	void Value(const char* label, const ds::vec2& v) {
		sprintf_s(_guiCtx->tmpBuffer, 256, "%g %g", v.x, v.y);
		Label(label, _guiCtx->tmpBuffer);
	}

	// --------------------------------------------------------
	// Value - int
	// --------------------------------------------------------
	void Value(const char* label, uint32_t v) {
		sprintf_s(_guiCtx->tmpBuffer, 256, "%d", v);
		Label(label, _guiCtx->tmpBuffer);
	}

	// --------------------------------------------------------
	// Value - uint32_t
	// --------------------------------------------------------
	void Value(const char* label, int v) {
		sprintf_s(_guiCtx->tmpBuffer, 256, "%d", v);
		Label(label, _guiCtx->tmpBuffer);
	}

	// --------------------------------------------------------
	// Value - float
	// --------------------------------------------------------
	void Value(const char* label, float v) {
		sprintf_s(_guiCtx->tmpBuffer, 256, "%g", v);
		Label(label, _guiCtx->tmpBuffer);
	}

	// --------------------------------------------------------
	// Value - float
	// --------------------------------------------------------
	void Value(const char* label, float v, const char* format) {
		sprintf_s(_guiCtx->tmpBuffer, 256, format, v);
		Label(label, _guiCtx->tmpBuffer);
	}

	// --------------------------------------------------------
	// Value - vec3
	// --------------------------------------------------------
	void Value(const char* label, const ds::vec3& v) {
		sprintf_s(_guiCtx->tmpBuffer, 256, "%g %g %g", v.x, v.y, v.z);
		Label(label, _guiCtx->tmpBuffer);
	}

	// --------------------------------------------------------
	// Value - vec4
	// --------------------------------------------------------
	void Value(const char* label, const ds::vec4& v) {
		sprintf_s(_guiCtx->tmpBuffer, 256, "%g %g %g %g", v.x, v.y, v.z, v.w);
		Label(label, _guiCtx->tmpBuffer);
	}

	// --------------------------------------------------------
	// Value - color
	// --------------------------------------------------------
	void Value(const char* label, const ds::Color& v) {
		int r = static_cast<int>(v.r * 255.0f);
		int g = static_cast<int>(v.g * 255.0f);
		int b = static_cast<int>(v.b * 255.0f);
		int a = static_cast<int>(v.a * 255.0f);
		sprintf_s(_guiCtx->tmpBuffer, 256, "%d %d %d %d", r, g, b, a);
		ds::vec2 p = _guiCtx->currentPos;
		ds::vec2 textDim = textSize(label);
		p.x = _guiCtx->currentPos.x + textDim.x + 200.0f;
		addBox(p, ds::vec2(20, 20), v);
		Label(label, _guiCtx->tmpBuffer);

	}

	// --------------------------------------------------------
	// Text
	// --------------------------------------------------------
	void Text(const char* text) {
		size_t l = strlen(text);
		ds::vec2 p = _guiCtx->currentPos;
		addText(p, text);
		ds::vec2 size = textSize(text);
		moveForward(ds::vec2(size.x, 30.0f));
	}

	// --------------------------------------------------------
	// Formatted text
	// --------------------------------------------------------
	void FormattedText(const char* fmt, ...) {
		char buffer[1024];
		va_list args;
		va_start(args, fmt);
		vsprintf(buffer, fmt, args);
		Text(buffer);
		va_end(args);

	}

	// --------------------------------------------------------
	// Message
	// --------------------------------------------------------
	void Message(const char* fmt, ...) {
		ds::vec2 pos = _guiCtx->currentPos;
		addBox(pos, 10, 20, _guiCtx->settings.labelBoxColor, true);
		char buffer[1024];
		va_list args;
		va_start(args, fmt);
		vsprintf(buffer, fmt, args);
		Text(buffer);
		va_end(args);

	}

	// -------------------------------------------------------
	// input string
	// -------------------------------------------------------
	bool Input(const char* label, char* str, int maxLength) {
		HashedId id = HashPointer(str);
		ds::vec2 p = _guiCtx->currentPos;
		p.x += 160.0f;
		addText(p, label);
		bool ret = InputScalar(id, 0, str, maxLength, 150.0f);
		moveForward(ds::vec2(150.0f, 22.0f));
		return ret;
	}

	// -------------------------------------------------------
	// input int
	// -------------------------------------------------------
	void Input(const char* label, int* v) {
		HashedId id = HashPointer(v);
		ds::vec2 p = _guiCtx->currentPos;
		p.x += 160.0f;
		addText(p, label);
		float f = static_cast<float>(*v);
		bool ret = InputScalar(id, 0, &f, "%.0f", 150.0f);
		*v = static_cast<int>(f);
		ds::vec2 ts = textSize(label);
		moveForward(ds::vec2(150.0f + ts.x + 10.0f, 22.0f));
	}

	// -------------------------------------------------------
	// input float
	// -------------------------------------------------------
	void Input(const char* label, float* v) {
		HashedId id = HashPointer(v);
		ds::vec2 p = _guiCtx->currentPos;
		p.x += 160.0f;
		addText(p, label);
		bool ret = InputScalar(id, 0, v, "%g", 150.0f);
		ds::vec2 ts = textSize(label);
		moveForward(ds::vec2(150.0f + ts.x + 10.0f, 22.0f));
	}

	// -------------------------------------------------------
	// input vec2
	// -------------------------------------------------------
	void Input(const char* label, ds::vec2* v) {
		HashedId id = HashPointer(v);
		ds::vec2 p = _guiCtx->currentPos;
		p.x += 160.0f;
		addText(p, label);
		InputScalar(id, 0, &v->x, "%g", 70.0f);
		bool ret = InputScalar(id, 1, &v->y, "%g", 70.0f);
		ds::vec2 ts = textSize(label);
		moveForward(ds::vec2(150.0f + ts.x + 10.0f, 22.0f));
	}

	// -------------------------------------------------------
	// input vec3
	// -------------------------------------------------------
	void Input(const char* label, ds::vec3* v) {
		HashedId id = HashPointer(v);
		ds::vec2 p = _guiCtx->currentPos;
		p.x += 240.0f;
		addText(p, label);
		InputScalar(id, 0, &v->x, "%g", 70.0f);
		InputScalar(id, 1, &v->y, "%g", 70.0f);
		InputScalar(id, 2, &v->z, "%g", 70.0f);
		ds::vec2 ts = textSize(label);
		moveForward(ds::vec2(150.0f + ts.x + 10.0f, 22.0f));
	}

	// -------------------------------------------------------
	// input vec4
	// -------------------------------------------------------
	void Input(const char* label, ds::vec4* v) {
		HashedId id = HashPointer(v);
		ds::vec2 p = _guiCtx->currentPos;
		p.x += 320.0f;
		addText(p, label);
		InputScalar(id, 0, &v->x, "%g", 70.0f);
		InputScalar(id, 1, &v->y, "%g", 70.0f);
		InputScalar(id, 2, &v->z, "%g", 70.0f);
		InputScalar(id, 3, &v->w, "%g", 70.0f);
		ds::vec2 ts = textSize(label);
		moveForward(ds::vec2(150.0f + ts.x + 10.0f, 22.0f));
	}

	// -------------------------------------------------------
	// Slider
	// -------------------------------------------------------	
	void ColorSlider(float* v, char prefix, int index, float width) {
		HashedId id = HashPointer(v);
		ds::vec2 p = _guiCtx->currentPos;
		p.x += index * (width + 10.0f);
		addBox(p, ds::vec2(width, 20.0f), _guiCtx->settings.labelBoxColor);
		// calculate offset
		int val = *v * 255.0f;
		int d = 255;
		if (isClicked(p, ds::vec2(width, 20.0f))) {
			ds::vec2 mp = ds::getMousePosition();
			float dx = mp.x - p.x - _guiCtx->itemOffset;
			val = static_cast<int>(dx * d / width);
		}
		if (_guiCtx->buttonPressed && isHovered(p, ds::vec2(width, 20.0f))) {
			ds::vec2 mp = ds::getMousePosition();
			float dx = mp.x - p.x - _guiCtx->itemOffset;
			val = static_cast<int>(dx * d / width);
		}
		if (val < 0) {
			val = 0;
		}
		if (val > 255) {
			val = 255;
		}
		p.x += static_cast<float>(val) / static_cast<float>(d) * width;
		p.x -= 4.0f;
		addBox(p, ds::vec2(8.0f, 24.0f), _guiCtx->settings.sliderColor);
		p = _guiCtx->currentPos;
		p.x += index * (width + 10.0f);
		sprintf_s(_guiCtx->tmpBuffer, 256, "%c: %d", prefix, val);
		ds::vec2 textDim = textSize(_guiCtx->tmpBuffer);
		p.x += (width - textDim.x) * 0.5f;
		addText(p, _guiCtx->tmpBuffer,0.0f);
		*v = static_cast<float>(val) / 255.0f;
	}

	// -------------------------------------------------------
	// input color
	// -------------------------------------------------------
	void Input(const char* label, ds::Color* v) {
		HashedId id = HashPointer(v);
		ColorSlider(&v->r, 'R', 0, 70.0f);
		ColorSlider(&v->g, 'G', 1, 70.0f);
		ColorSlider(&v->b, 'B', 2, 70.0f);
		ColorSlider(&v->a, 'A', 3, 70.0f);
		ds::vec2 p = _guiCtx->currentPos;
		p.x = _guiCtx->currentPos.x + 320.0f;
		addBox(p, ds::vec2(20, 20), *v);
		p.x = _guiCtx->currentPos.x + 350.0f;
		addText(p, label);
		ds::vec2 ts = textSize(label);
		moveForward(ds::vec2(150.0f + ts.x + 10.0f, 22.0f));
	}

	// -------------------------------------------------------
	// Separator
	// -------------------------------------------------------	
	void Separator() {
		ds::vec2 p = _guiCtx->currentPos;
		p.y += 8.0f;
		addBox(p, ds::vec2(10, 4), ds::Color(32, 32, 32, 255), true);
		moveForward(ds::vec2(10, 16.0f));
	}

	// -------------------------------------------------------
	// int input using steps
	// -------------------------------------------------------
	void StepInput(const char* label, int* v, int minValue, int maxValue, int steps) {
		HashedId id = HashPointer(v);
		ds::vec2 p = _guiCtx->currentPos;
		// -
		addBox(p, 20, 20, _guiCtx->settings.buttonColor);
		if (isClicked(p, ds::vec2(20, 20))) {
			*v -= steps;
			if (*v < minValue) {
				*v = minValue;
			}
		}
		addText(p, "-");
		// value
		sprintf_s(_guiCtx->tmpBuffer, 256, "%d", *v);
		ds::vec2 textDim = textSize(_guiCtx->tmpBuffer);
		p = _guiCtx->currentPos;
		p.x += 20.0f;
		addBox(p, ds::vec2(110, 20), _guiCtx->settings.labelBoxColor);
		p.x += (110.0f - textDim.x) * 0.5f;
		addText(p, _guiCtx->tmpBuffer);
		// +
		p = _guiCtx->currentPos;
		p.x += 130.0f;
		addBox(p,20, 20, _guiCtx->settings.buttonColor);
		if (isClicked(p, ds::vec2(20, 20))) {
			*v += steps;
			if (*v > maxValue) {
				*v = maxValue;
			}
		}
		addText(p, "+");
		// label
		p.x += 30.0f;
		addText(p, label);
		ds::vec2 ts = textSize(label);
		moveForward(ds::vec2(150.0f + ts.x + 30.0f, 30.0f));
	}

	// -------------------------------------------------------
	// Slider
	// -------------------------------------------------------	
	void Slider(const char* label, int* v, int minValue, int maxValue, float width) {
		HashedId id = HashPointer(v);
		ds::vec2 p = _guiCtx->currentPos;
		addBox(_guiCtx->currentPos, ds::vec2(width, 20.0f), _guiCtx->settings.labelBoxColor);
		// calculate offset
		int d = maxValue - minValue;
		if (isClicked(p, ds::vec2(width, 20.0f))) {
			ds::vec2 mp = ds::getMousePosition();
			float dx = mp.x - p.x - _guiCtx->itemOffset;
			*v = static_cast<int>(dx * d / width);
		}
		if (_guiCtx->buttonPressed && isHovered(p, ds::vec2(width, 20.0f))) {
			ds::vec2 mp = ds::getMousePosition();
			float dx = mp.x - p.x - _guiCtx->itemOffset;
			*v = static_cast<int>(dx * d / width);
		}
		if (*v < minValue) {
			*v = minValue;
		}
		if (*v > maxValue) {
			*v = maxValue;
		}
		p.x += static_cast<float>(*v) / static_cast<float>(d) * width;
		addBox(p, ds::vec2(8.0f, 28.0f), _guiCtx->settings.sliderColor);
		p = _guiCtx->currentPos;
		sprintf_s(_guiCtx->tmpBuffer, 256, "%d", *v);
		ds::vec2 textDim = textSize(_guiCtx->tmpBuffer);
		p.x += (width - textDim.x) * 0.5f;
		addText(p, _guiCtx->tmpBuffer);
		p = _guiCtx->currentPos;
		p.x += width + 10.0f;
		addText(p, label);
		ds::vec2 ts = textSize(label);
		moveForward(ds::vec2(width + ts.x + 20.0f, 30.0f));
	}

	// -------------------------------------------------------
	// Slider
	// -------------------------------------------------------	
	void Slider(const char* label, float* v, float minValue, float maxValue, int precision, float width) {
		HashedId id = HashPointer(v);
		ds::vec2 p = _guiCtx->currentPos;
		addBox(_guiCtx->currentPos, ds::vec2(width, 20.0f), _guiCtx->settings.buttonColor);
		// calculate offset
		float d = maxValue - minValue;
		if (isClicked(p, ds::vec2(width, 20.0f))) {
			ds::vec2 mp = ds::getMousePosition();
			float dx = mp.x - p.x - _guiCtx->itemOffset;
			*v = dx * d / width;
		}
		if (_guiCtx->buttonPressed && isHovered(p, ds::vec2(width, 20.0f))) {
			ds::vec2 mp = ds::getMousePosition();
			float dx = mp.x - p.x - _guiCtx->itemOffset;
			*v = dx * d / width;
		}
		if (*v < minValue) {
			*v = minValue;
		}
		if (*v > maxValue) {
			*v = maxValue;
		}
		if (precision > 0) {
			float prec = 10.0f * static_cast<float>(precision);
			*v = roundf(*v * prec) / prec;
		}
		p.x += *v / d * width;
		addBox(p, ds::vec2(8.0f, 28.0f), _guiCtx->settings.sliderColor);
		p = _guiCtx->currentPos;
		sprintf_s(_guiCtx->tmpBuffer, 256, "%g", *v);
		ds::vec2 textDim = textSize(_guiCtx->tmpBuffer);
		p.x += (width - textDim.x) * 0.5f;
		addText(p, _guiCtx->tmpBuffer);
		p = _guiCtx->currentPos;
		p.x += width + 10.0f;
		addText(p, label);
		ds::vec2 ts = textSize(label);
		moveForward(ds::vec2(width + ts.x + 20.0f, 30.0f));
	}

	void SliderAngle(const char* label, float* v, float width) {
		int d = static_cast<int>(*v / ds::TWO_PI * 360.0f);
		Slider(label, &d, 0, 360, width);
		*v = d / 360.0f * ds::TWO_PI;
	}

	// -------------------------------------------------------
	// Histogram
	// -------------------------------------------------------	
	void Histogram(float* values, int num, float minValue, float maxValue, float step, float width, float height) {
		ds::vec2 p = _guiCtx->currentPos;
		HashedId id = HashPointer(values);
		float barWidth = 10.0f;
		p.y -= height / 2.0f;
		float delta = maxValue - minValue;
		if (delta == 0.0f) {
			delta = 1.0f;
		}
		float st = width / static_cast<float>(num - 1);
		float bw = width / static_cast<float>(num);
		addBox(p, ds::vec2(width + barWidth, height), ds::Color(51, 51, 51, 255));
		p.x += width + 20.0f;
		p.y += height / 2.0f;
		char buffer[16];
		sprintf_s(buffer, 16, "%g", maxValue);
		addText(p, buffer);
		p.y -= height;
		sprintf_s(buffer, 16, "%g", minValue);
		addText(p, buffer);
		for (int i = 0; i < num; ++i) {
			float v = values[i];
			if (v > maxValue) {
				v = maxValue;
			}
			float current = (v - minValue) / delta;
			float yp = current * height;
			p = _guiCtx->currentPos;
			p.y -= (height - yp * 0.5f);
			p.x += static_cast<float>(i) * bw + 2.0f;
			addBox(p, ds::vec2(bw - 4.0f, yp), ds::Color(192, 0, 0, 255));
		}
		step = delta / 10.0f;
		int d = static_cast<int>(delta / step) + 1;
		for (int i = 0; i < d; ++i) {
			p = _guiCtx->currentPos;
			float current = 1.0f - (step*i) / delta;
			float yp = current * height;
			p.y -= yp;
			addBox(p, ds::vec2(width + barWidth, 1.0f), ds::Color(16, 16, 16, 255));
		}
		moveForward(ds::vec2(width, height + 30.0f));
	}

	// -------------------------------------------------------
	// Diagram
	// -------------------------------------------------------	
	void DiagramInternal(const ds::vec2& pos, float* values, int num, float minValue, float maxValue, float step, float width, float height) {
		ds::vec2 p = pos;
		HashedId id = HashPointer(values);
		p.y -= height / 2.0f;
		float delta = (maxValue - minValue);
		if (delta == 0.0f) {
			delta = 1.0f;
		}
		float st = width / static_cast<float>(num - 1);
		addBox(p, ds::vec2(width, height), ds::Color(51, 51, 51, 255));
		p.x += width + 20.0f;
		p.y += height / 2.0f;
		char buffer[16];
		sprintf_s(buffer, 16, "%g", maxValue);
		addText(p, buffer);
		p.y -= height;
		sprintf_s(buffer, 16, "%g", minValue);
		addText(p, buffer);

		for (int i = 0; i < num; ++i) {
			float v = values[i];
			if (v > maxValue) {
				v = maxValue;
			}
			if (v < minValue) {
				v = minValue;
			}
			float norm = (v - minValue) / delta;
			float yp = norm * height;
			p = pos;
			p.y = pos.y - height + yp;
			p.x += static_cast<float>(i)* st - 2.0f;
			addBox(p, ds::vec2(4, 4), ds::Color(192, 0, 0, 255));
		}

		step = delta / 10.0f;
		int d = static_cast<int>(delta / step) + 1;
		for (int i = 0; i < d; ++i) {
			p = pos;
			float current = 1.0f - (step*i) / delta;
			float yp = current * height;
			p.y -= yp;
			addBox(p, ds::vec2(width, 1.0f), ds::Color(16, 16, 16, 255));
		}
		moveForward(ds::vec2(width, height + 30.0f));
	}

	void Diagram(float* values, int num, float minValue, float maxValue, float step, float width, float height) {
		DiagramInternal(_guiCtx->currentPos, values, num, minValue, maxValue, step, width, height);
	}

	void Diagram(const char* label, float* values, int num, float minValue, float maxValue, float step, float width, float height) {
		ds::vec2 p = _guiCtx->currentPos;
		addText(p, label);
		moveForward(ds::vec2(10.0f, 20.0f));
		DiagramInternal(p, values, num, minValue, maxValue, step, width, height);
	}

	void beginMenu() {
		// FIXME: get screen dimension!
		_guiCtx->menuPosition = ds::vec2(0, 755);
		addBox(_guiCtx->menuPosition, ds::vec2(10, 30), ds::Color(0, 0, 192, 255), true);
	}

	void MenuBar(const char** names, int num, int* state) {
		ds::vec2 pos = _guiCtx->menuPosition;
		for (int i = 0; i < num; ++i) {
			ds::vec2 dim = textSize(names[i]);
			dim.x += 20.0f;
			dim.y = 30.0f;
			addBox(pos, dim, ds::Color(192, 0, 192, 255));
			addText(pos, names[i]);

			if (isClicked(pos, dim)) {
				int idx = i + 1;
				if (*state == idx) {
					*state = 0;
				}
				else {
					*state = idx;
				}
			}
			pos.x += dim.x + 2.0f;
		}
	}

	bool MenuBar(const char* name) {
		HashedId id = HashPointer(name);
		ds::vec2 pos = _guiCtx->menuPosition;
		ds::vec2 dim = textSize(name);
		dim.x += 20.0f;
		dim.y = 30.0f;
		if (isHovered(pos, dim)) {
			addBox(pos, dim, ds::Color(192, 0, 192, 255));
		}
		else {
			addBox(pos, dim, ds::Color(192, 192, 0, 255));
		}
		addText(pos, name);
		bool clicked = isClicked(pos, dim);
		if (clicked) {
			_guiCtx->active = id;
		}
		_guiCtx->menuPosition.x += dim.x + 2.0f;
		return clicked;
	}

	bool MenuItem(const char* label) {
		return false;
	}

	void endMenu() {

	}

	// -------------------------------------------------------
	// prepare combo box
	// -------------------------------------------------------
	void prepareComboBox(int id, int* offset, int size, int max) {
		float width = 200.0f;
		ds::vec2 p = _guiCtx->currentPos;
		float height = max * 20.0f;
		p.y -= height * 0.5f - 10.0f;
		// background
		addBox(p, ds::vec2(width + 20.0f, height), _guiCtx->settings.boxBackgroundColor);
		if (size > max) {
			// up
			p = _guiCtx->currentPos;
			p.x += width;
			addBox(p, 20, 20, _guiCtx->settings.buttonColor);
			addText(p, "-");
			if (isBoxSelected(id, p, ds::vec2(20.0f, 20.0f))) {
				*offset -= 1;
				if (*offset < 0) {
					*offset = 0;
				}
			}
			float sideHeight = height - 2.0f * 20.0f;
			p.y -= sideHeight * 0.5f;
			p.y -= 20.0f * 0.5f;
			float cy = p.y;
			addBox(p, ds::vec2(20.0f, sideHeight), ds::Color(20, 20, 20, 255));

			// down
			p.y = _guiCtx->currentPos.y - (max - 1) * 20.0f;
			addBox(p, 20, 20, _guiCtx->settings.buttonColor);
			addText(p, "+");
			if (isBoxSelected(id, p, ds::vec2(20.0f, 20.0f))) {
				if ((*offset + max) < size) {
					*offset += 1;
				}
			}

			float d = 1.0f - static_cast<float>(*offset) / static_cast<float>(max);
			float dy = d * (sideHeight) - 20.0f;
			p.y = cy + dy;
			addBox(p, ds::vec2(20.0f, 6.0f), _guiCtx->settings.scrollSliderColor);
			
		}
	}
	// -------------------------------------------------------
	// ListBox
	// -------------------------------------------------------	
	void ListBox(const char* label, const char** entries, int num, int* selected, int *offset, int max) {
		HashedId id = HashPointer(&entries);
		prepareComboBox(id, offset, num, max);
		float width = 200.0f;
		ds::vec2 p = _guiCtx->currentPos;
		float height = max * 20.0f;
		//bool hot = isHot(id, p, v2(width, height));
		int start = *offset;
		int end = *offset + max;
		if (end >= num) {
			end = num;
		}
		for (int i = start; i < end; ++i) {
			if (isBoxSelected(id, p, ds::vec2(width, 20.0f))) {
				*selected = i;
			}
			if (*selected == i) {
				addBox(p, ds::vec2(width, 20.0f), _guiCtx->settings.boxSelectionColor);
			}
			addText(p, entries[i]);
			p.y -= 20.0f;
		}
		moveForward(ds::vec2(width, height + 4.0f));
	}

	// -------------------------------------------------------
	// DropDownBox
	// -------------------------------------------------------	
	void DropDownBox(const char* label, const char** entries, int num, int* state, int* selected, int *offset, int max, bool closeOnSelection) {
		HashedId id = HashPointer(&entries);
		ds::vec2 p = _guiCtx->currentPos;
		if (*state == 0) {
			addBox(p, 20, 20, _guiCtx->settings.buttonColor);
			addText(p, "+");
			if (isBoxSelected(id, p, ds::vec2(20.0f, 20.0f))) {
				*state = 1;
			}
		}
		else {
			addBox(p, 20, 20, _guiCtx->settings.buttonColor);
			addText(p, "-");
			if (isBoxSelected(id, p, ds::vec2(20.0f, 20.0f))) {
				*state = 0;
			}
		}
		p.x += 20.0f;
		addBox(p, ds::vec2(200, 20), _guiCtx->settings.labelBoxColor);
		if (*selected >= 0 && *selected < num) {
			addText(p, entries[*selected]);
		}
		else {
			addText(p, "{Select entry}");
		}
		p.x += 180.0f;
		addBox(p, 20, 20, _guiCtx->settings.buttonColor);
		addText(p, "x");
		if (isBoxSelected(id, p, ds::vec2(20.0f, 20.0f))) {
			*selected = -1;
		}
		
		p.x += 30.0f;
		addText(p, label);
		
		moveForward(ds::vec2(300.0f, 20.0f));
		if (*state == 1) {
			prepareComboBox(id, offset, num, max);
			float width = 200.0f;
			// if there is no scroll border then expand it
			if (num <= max) {
				width += 20.0f;
			}
			ds::vec2 p = _guiCtx->currentPos;
			float height = max * 20.0f;
			int start = *offset;
			int end = *offset + max;
			if (end >= num) {
				end = num;
			}
			for (int i = start; i < end; ++i) {
				if (isBoxSelected(id, p, ds::vec2(width, 20.0f))) {
					*selected = i;
					if (closeOnSelection) {
						*state = 0;
					}
				}
				if (*selected == i) {
					addBox(p, ds::vec2(width, 20.0f), _guiCtx->settings.boxSelectionColor);
				}
				addText(p, entries[i]);
				p.y -= 20.0f;
			}
			moveForward(ds::vec2(width, height + 4.0f));
		}
		else {
			moveForward(ds::vec2(300.0f, 4.0f));
		}

	}
	// --------------------------------------------------------
	// end - render all draw calls
	// --------------------------------------------------------
	void end() {
		_guiCtx->buffer->begin();
		float h = _guiCtx->size.y - 10.0f;
		float sy = h / 128.0f;
		float w = _guiCtx->size.x + 20.0f;
		float sx = w / 128.0f;
		float bpx = _guiCtx->startPos.x + w * 0.5f;
		float bpy = _guiCtx->startPos.y - h * 0.5f;

		_guiCtx->buffer->add(ds::vec2(bpx, bpy), ds::vec4(256, 0, 128, 128), ds::vec2(sx, sy), 0.0f, _guiCtx->settings.backgroundColor);

		int activeId = _guiCtx->activeGroup;
		for (size_t i = 0; i < _guiCtx->calls.size(); ++i) {
			const DrawCall& call = _guiCtx->calls[i];
			ds::vec2 p = call.pos;
			if (call.resize) {
				ds::vec4 rect = call.rect;
				rect.z = 128.0f;
				p.x = bpx;
				_guiCtx->buffer->add(p, rect, ds::vec2(sx, 1.0f), 0.0f, call.color);
			}
			else {
				p.x += _guiCtx->itemOffset;
				_guiCtx->buffer->add(p, call.rect, call.scale, 0.0f, call.color);
			}
		}
		_guiCtx->numKeys = 0;
		_guiCtx->buffer->flush();
	}

	// --------------------------------------------------------
	// shutdown GUI
	// --------------------------------------------------------
	void shutdown() {
		if (_guiCtx != 0) {
			delete _guiCtx;
		}
	}

}

#endif