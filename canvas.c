#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

#include "canvas.h"
#include "paint-squares.h"

static CanvasNode * CanvasCreateNode (Rectangle r, CanvasSplitType sp);
static void         CanvasDeleteNode (CanvasNode *n);
static void         CanvasDrawNode   (CanvasNode *n);
static void         CanvasSplitNode  (Vector2 delim, CanvasNode *n);

void
CanvasClear(void) {
	CanvasDeleteNode(gState.canvasTree);
	Rectangle r = {0, 0, gState.winWidth, gState.winHeight};
	gState.canvasTree = CanvasCreateNode(r, CANVAS_SPLIT_VERTICAL);
}

void
CanvasDraw(void) {
	CanvasDrawNode(gState.canvasTree);
}

void
CanvasExit(void) {
	gState.quit = true;
}

void
CanvasSplit(Vector2 cursor_pos) {
	CanvasSplitNode(cursor_pos, gState.canvasTree);
}

static CanvasNode *
CanvasCreateNode(Rectangle r, CanvasSplitType sp) {
	CanvasNode *n = calloc(1, sizeof (*n));
	if (!n) {
		fprintf(stderr, PROJECT_NAME ": Fatal error: %s.\n",
			strerror(errno));
		CanvasExit();
		return NULL;
	}
	n->rect = r;
	n->split = sp;
	return n;
}

void
CanvasDeleteNode(CanvasNode *n) {
	if (!n)
		return;
	CanvasDeleteNode(n->left);
	CanvasDeleteNode(n->right);
	free(n);
}

void
CanvasDrawNode(CanvasNode *n) {
	if (!n)
		return;
	if (n->left || n->right) {
		CanvasDrawNode(n->left);
		CanvasDrawNode(n->right);
		return;
	}
	DrawRectangleRec(n->rect, gState.canvasColors[n->colorId]);
	DrawRectangleLinesEx(n->rect, 5, gState.canvasColors[CANVAS_CLR_BORDER]);
}

void
CanvasSplitNode(Vector2 delim, CanvasNode *n) {
	if (!n)
		return;
	if (n->left && CheckCollisionPointRec(delim, n->left->rect)) {
		CanvasSplitNode(delim, n->left);
		return;
	}
	if (n->right && CheckCollisionPointRec(delim, n->right->rect)) {
		CanvasSplitNode(delim, n->right);
		return;
	}
	Rectangle l, r;
	switch (n->split) {
	case CANVAS_SPLIT_VERTICAL:
		l = (Rectangle){
			n->rect.x,           n->rect.y,
				delim.x - n->rect.x, n->rect.height
		};
		r = (Rectangle){
			delim.x,                 n->rect.y,
				n->rect.width - l.width, n->rect.height
		};
		n->left = CanvasCreateNode(l, CANVAS_SPLIT_HORIZONTAL);
		n->right = CanvasCreateNode(r, CANVAS_SPLIT_HORIZONTAL);
		break;
	case CANVAS_SPLIT_HORIZONTAL:
		l = (Rectangle){
			n->rect.x,     n->rect.y,
				n->rect.width, delim.y - n->rect.y
		};
		r = (Rectangle){
			n->rect.x,     delim.y,
				n->rect.width, n->rect.height - l.height
		};
		n->left = CanvasCreateNode(l, CANVAS_SPLIT_VERTICAL);
		n->right = CanvasCreateNode(r, CANVAS_SPLIT_VERTICAL);
		break;
	}
}
