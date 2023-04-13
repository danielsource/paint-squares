#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"

#include "assets.h"
#include "canvas.h"
#include "paint-squares.h"

#define LENGTH(array) ((int)(sizeof (array) / sizeof *(array)))

static void Cleanup       (void);
static void Draw          (void);
static void HandleInput   (void);
static void Run           (void);
static void Setup         (void);
static void SignalHandler (int sig);

GlobalState gState = {
	.canvasColors = {
		[CANVAS_CLR_BG]     = {0xe3, 0xe1, 0xe2, 0xff}, // #e3e1e2
		[CANVAS_CLR_BORDER] = {0x05, 0x0a, 0x06, 0xff}, // #050a06
		[CANVAS_CLR_1]      = {0xdf, 0x29, 0x1e, 0xff}, // #df291e
		[CANVAS_CLR_2]      = {0xee, 0xdd, 0x6f, 0xff}, // #eedd6f
		[CANVAS_CLR_3]      = {0x01, 0x58, 0x9b, 0xff}, // #01589b
	},
	.winWidth = 640,
	.winHeight = 360,
	.winTitle = PROJECT_NAME,
	.targetFPS = 30
};

static const char *const gProjectDescription =
"paint-squares\n"
"=============\n"
"A program made with raylib (v4.5) to paint compositions similar to some made\n"
"by Piet Mondrian. Inspired by Mondrian And Me (https://github.com/tholman/mondrian-and-me).\n"
"\n"
"This program was written by daniel.source and is Copyright (C) 2023.\n"
"The source code is released under the zlib License.\n"
#ifdef PROJECT_REPO_LINK
"\n"
"The code can be found at " PROJECT_REPO_LINK ".\n"
#endif
;

static const KeyBinding gKeyBindings[] = {
	{.key = KEY_ESCAPE, .cb = CanvasExit},
	{.key = KEY_C,      .cb = CanvasClear},
};

static const MouseBinding gMouseBindings[] = {
	{.btn = MOUSE_BUTTON_LEFT, .cb = CanvasSplit},
};

void
Cleanup(void) {
	CanvasClear();
	UnloadFont(gState.font);
	CloseWindow();
}

void
Draw(void) {
	BeginDrawing();
	ClearBackground(BLACK);
	CanvasDraw();
	EndDrawing();
}

void
HandleInput(void) {
	if (WindowShouldClose()) {
		gState.quit = true;
		return;
	}
	int key;
	while ((key = GetKeyPressed()) != KEY_NULL) {
		for (int i = 0; i < LENGTH(gKeyBindings); i++) {
			if (key == gKeyBindings[i].key) {
				gKeyBindings[i].cb();
				break;
			}
		}
	}
	for (int i = 0; i < LENGTH(gMouseBindings); i++)
		if (IsMouseButtonPressed(gMouseBindings[i].btn))
			gMouseBindings[i].cb(GetMousePosition());
}

void
Run(void) {
	HandleInput();
	Draw();
}

void
Setup(void) {
	signal(SIGINT, SignalHandler);
	SetTraceLogLevel(LOG_WARNING);
	InitWindow(gState.winWidth, gState.winHeight, gState.winTitle);
	SetExitKey(KEY_NULL);
	SetTargetFPS(gState.targetFPS);
	gState.font = LoadFontFromMemory(".ttf",
					 ASSET_START(font),
					 ASSET_SIZE(font),
					 PROJECT_FONT_SIZE,
					 NULL,
					 0);
	SetTextureFilter(gState.font.texture, TEXTURE_FILTER_BILINEAR);
	CanvasClear();
}

void
SignalHandler(int sig) {
	if (sig == SIGINT)
		CanvasExit();
}

int
main(int argc, char **argv) {
	if (argc > 1) {
		fputs(gProjectDescription, stderr);
		return TextIsEqual(argv[1], "--help") ? EXIT_SUCCESS : EXIT_FAILURE;
	}
	fputs(PROJECT_NAME ": Use option '--help' to show more "
	      "information.\n", stderr);
	Setup();
	while (!gState.quit)
		Run();
	fputs(PROJECT_NAME ": Exiting...\n", stderr);
	Cleanup();
	return EXIT_SUCCESS;
}
