#pragma once

#include "raylib.h"

typedef enum {
	CANVAS_CLR_BG,
	CANVAS_CLR_BORDER,
	CANVAS_CLR_1,
	CANVAS_CLR_2,
	CANVAS_CLR_3,
	CANVAS_COLOR_COUNT
} CanvasColor;

typedef enum {
	CANVAS_SPLIT_VERTICAL,
	CANVAS_SPLIT_HORIZONTAL
} CanvasSplitType;

typedef struct CanvasNode {
	Rectangle rect;
	CanvasColor colorId;
	CanvasSplitType split;
	struct CanvasNode *left, *right;
} CanvasNode;

void CanvasClear (void);
void CanvasDraw  (void);
void CanvasExit  (void);
void CanvasSplit (Vector2 cursor_pos);
