#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "raylib.h"

#include "canvas.h"
#include "paint-squares.h"

#define LENGTH(array) ((int)(sizeof (array) / sizeof *(array)))

static void Cleanup                (void);
static void Draw                   (void);
static void HandleInput            (void);
static void Run                    (void);
static void SaveScreenImage        (void);
static void Setup                  (void);
static void ShowHelp               (void);
static void SignalHandler          (int sig);
static void UpdateWindowDimensions (void);

GlobalState gState = {
	.canvasColors = {
		[CANVAS_CLR_BG]     = {0xe3, 0xe1, 0xe2, 0xff}, // #e3e1e2
		[CANVAS_CLR_BORDER] = {0x05, 0x0a, 0x06, 0xff}, // #050a06
		[CANVAS_CLR_1]      = {0xdf, 0x29, 0x1e, 0xff}, // #df291e
		[CANVAS_CLR_2]      = {0xee, 0xdd, 0x6f, 0xff}, // #eedd6f
		[CANVAS_CLR_3]      = {0x01, 0x58, 0x9b, 0xff}  // #01589b
	},
	.canvasBorderWidth = 5,
	.winWidth = 640,
	.winHeight = 360,
	.winTitle = PROJECT_NAME " (press F1 to open the project website)",
	.targetFPS = 30
};

static const char *const gProjectDescription =
"paint-squares\n"
"=============\n"
"\n"
"A program made with raylib (v4.5) to paint compositions similar to some made\n"
"by Piet Mondrian. Inspired by Mondrian And Me (https://github.com/tholman/mondrian-and-me).\n"
"\n"
"This program was written by daniel.source and is Copyright (C) 2023.\n"
"The source code is released under the zlib License.\n"
#ifdef PROJECT_REPO_LINK
"\n"
"The code can be found at " PROJECT_REPO_LINK ".\n"
"\n"
#endif
"Controls\n"
"--------\n"
"\n"
"Mouse:\n"
" - Mouse left button -> Paint a line at the cursor position.\n"
"\n"
"Keyboard:\n"
" - P -> Take screenshot of the program window.\n"
" - C -> Clear the window.\n"
" - ESCAPE -> Exit program.\n"
;

static const KeyBinding gKeyBindings[] = {
	{.key = KEY_F1,     .cb = ShowHelp},
	{.key = KEY_ESCAPE, .cb = CanvasExit},
	{.key = KEY_C,      .cb = CanvasClear},
	{.key = KEY_P,      .cb = SaveScreenImage}
};

static const MouseBinding gMouseBindings[] = {
	{.btn = MOUSE_BUTTON_LEFT, .cb = CanvasSplit}
};

void
Cleanup(void) {
	CanvasClear();
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
	UpdateWindowDimensions();
	Draw();
}

void
SaveScreenImage(void) {
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);
	char datetime[32], s[64];
	strftime(datetime, sizeof(datetime), "%y-%m-%d-%H:%M:%S", tm);
	snprintf(s, sizeof(s), PROJECT_NAME "-%s.png", datetime);
	TakeScreenshot(s);
}

void
Setup(void) {
	signal(SIGINT, SignalHandler);
	srand(time(NULL));
	SetTraceLogLevel(LOG_WARNING);
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(gState.winWidth, gState.winHeight, gState.winTitle);
	SetExitKey(KEY_NULL);
	SetTargetFPS(gState.targetFPS);
	CanvasClear();
}

void
ShowHelp(void) {
	OpenURL(PROJECT_REPO_LINK);
}

void
SignalHandler(int sig) {
	if (sig == SIGINT)
		CanvasExit();
}

void
UpdateWindowDimensions(void) {
	gState.winWidth = GetScreenWidth();
	gState.winHeight = GetScreenHeight();
}

int
main(int argc, char **argv) {
	if (argc > 1) {
		fputs(gProjectDescription, stderr);
		return TextIsEqual(argv[1], "--help")
			? EXIT_SUCCESS : EXIT_FAILURE;
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
