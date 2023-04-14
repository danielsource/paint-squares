#pragma once

#include <signal.h>

#include "raylib.h"

#include "canvas.h"

typedef void KeyBindingCallback   (void);
typedef void MouseBindingCallback (Vector2 cursorPos);

typedef struct {
	CanvasTree *canvas;
	int canvasBorderWidth;
	Color canvasColors[CANVAS_COLOR_COUNT];
	Font font;
	int winWidth, winHeight;
	const char *winTitle;
	int targetFPS;
	volatile sig_atomic_t quit;
} GlobalState;

typedef struct {
	int key;
	KeyBindingCallback *cb;
} KeyBinding;

typedef struct {
	int btn;
	MouseBindingCallback *cb;
} MouseBinding;

extern GlobalState gState;
