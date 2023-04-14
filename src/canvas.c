#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

#include "canvas.h"
#include "paint-squares.h"

static CanvasNode *    CanvasCreateNode    (CanvasSplitType sp, CanvasColor col);
static void            CanvasDeleteNode    (CanvasNode *n);
static void            CanvasDrawNode      (CanvasNode *n, Rectangle area);
static CanvasNodeAreas CanvasGetNodeAreasH (CanvasNode *n, Rectangle parentArea);
static CanvasNodeAreas CanvasGetNodeAreasV (CanvasNode *n, Rectangle parentArea);
static CanvasColor     CanvasGetSomeColor  (void);
static void            CanvasSplitNode     (CanvasNode *n, Vector2 delim);
static void            CanvasSplitNodeH    (CanvasNode *n, Vector2 delim);
static void            CanvasSplitNodeV    (CanvasNode *n, Vector2 delim);

void
CanvasClear(void) {
	CanvasDeleteNode(gState.canvas);
	gState.canvas = CanvasCreateNode(CANVAS_SPLIT_VERTICAL,
					 CANVAS_CLR_BG);
}

void
CanvasDraw(void) {
	Rectangle area = {0, 0, gState.winWidth, gState.winHeight};
	CanvasDrawNode(gState.canvas, area);
}

void
CanvasExit(void) {
	gState.quit = true;
}

void
CanvasSplit(Vector2 cursor_pos) {
	if (!gState.canvas)
		return;
	CanvasSplitNode(gState.canvas, cursor_pos);
}

CanvasNode *
CanvasCreateNode(CanvasSplitType sp, CanvasColor col) {
	CanvasNode *n = calloc(1, sizeof (*n));
	if (!n) {
		fprintf(stderr, PROJECT_NAME ": Fatal error: %s.\n",
			strerror(errno));
		CanvasExit();
		return NULL;
	}
	n->split = sp;
	n->colorId = col;
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
CanvasDrawNode(CanvasNode *n, Rectangle area) {
	if (!n)
		return;
	if (!(n->left && n->right)) {
		DrawRectangleRec(area, gState.canvasColors[n->colorId]);
		DrawRectangleLinesEx(area, gState.canvasBorderWidth,
				     gState.canvasColors[CANVAS_CLR_BORDER]);
		return;
	}
	CanvasNodeAreas areas;
	switch (n->split) {
	case CANVAS_SPLIT_VERTICAL:
		areas = CanvasGetNodeAreasV(n, area);
		break;
	case CANVAS_SPLIT_HORIZONTAL:
		areas = CanvasGetNodeAreasH(n, area);
		break;
	}
	CanvasDrawNode(n->left, areas.left);
	CanvasDrawNode(n->right, areas.right);
}

CanvasNodeAreas
CanvasGetNodeAreasH(CanvasNode *n, Rectangle parentArea) {
	Rectangle leftArea = {
		parentArea.x,
		parentArea.y,
		parentArea.width,
		n->split_pos - parentArea.y
	};
	Rectangle rightArea = {
		parentArea.x,
		n->split_pos,
		parentArea.width,
		parentArea.height - leftArea.height
	};
	return (CanvasNodeAreas){leftArea, rightArea};
}

CanvasNodeAreas
CanvasGetNodeAreasV(CanvasNode *n, Rectangle parentArea) {
	Rectangle leftArea = {
		parentArea.x,
		parentArea.y,
		n->split_pos - parentArea.x,
		parentArea.height
	};
	Rectangle rightArea = {
		n->split_pos,
		parentArea.y,
		parentArea.width - leftArea.width,
		parentArea.height
	};
	return (CanvasNodeAreas){leftArea, rightArea};
}

CanvasColor
CanvasGetSomeColor(void) {
	return CANVAS_CLR_BG;
}

void
CanvasSplitNodeH(CanvasNode *n, Vector2 delim) {
	if (!(n->left && n->right)) {
		n->split_pos = delim.y;
		n->left = CanvasCreateNode(CANVAS_SPLIT_VERTICAL,
					   CanvasGetSomeColor());
		n->right = CanvasCreateNode(CANVAS_SPLIT_VERTICAL,
					    CanvasGetSomeColor());
	} else if (delim.y <= n->split_pos) {
		CanvasSplitNode(n->left, delim);
	} else {
		CanvasSplitNode(n->right, delim);
	}
}

void
CanvasSplitNodeV(CanvasNode *n, Vector2 delim) {
	if (!(n->left && n->right)) {
		n->split_pos = delim.x;
		n->left = CanvasCreateNode(CANVAS_SPLIT_HORIZONTAL,
					   CanvasGetSomeColor());
		n->right = CanvasCreateNode(CANVAS_SPLIT_HORIZONTAL,
					    CanvasGetSomeColor());
	} else if (delim.x <= n->split_pos) {
		CanvasSplitNode(n->left, delim);
	} else {
		CanvasSplitNode(n->right, delim);
	}
}

void
CanvasSplitNode(CanvasNode *n, Vector2 delim) {
	if (!n)
		return;
	switch (n->split) {
	case CANVAS_SPLIT_VERTICAL:
		CanvasSplitNodeV(n, delim);
		break;
	case CANVAS_SPLIT_HORIZONTAL:
		CanvasSplitNodeH(n, delim);
		break;
	}
}
