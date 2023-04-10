#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"

#include "assets.h"

static struct {
	Font font;
	int win_w, win_h;
	const char *win_title;
	int target_fps;
	bool quit;
} State = {
	.win_w = 640,
	.win_h = 360,
	.win_title = PROJECT_NAME,
	.target_fps = 30
};

static const char *const project_description =
"paint-squares\n"
"=============\n"
"A program made with raylib (v4.5) to paint compositions similar to some made\n"
"by Piet Mondrian. Inspired by Mondrian And Me (https://github.com/tholman/mondrian-and-me).\n"
"\n"
"This program was written by daniel.source and is Copyright (C) 2023.\n"
"The source code is released under the zlib License.\n"
#ifdef PROJECT_REPO_LINK
"\n"
"The code can be found at " PROJECT_REPO_LINK ".\n";
#else
;
#endif

static void
setup(void) {
	SetTraceLogLevel(LOG_WARNING);
	InitWindow(State.win_w, State.win_h, State.win_title);
	SetTargetFPS(State.target_fps);
	State.font = LoadFontFromMemory(".ttf",
					ASSET_START(font_monospace),
					ASSET_SIZE(font_monospace),
					20,
					NULL,
					0);
	SetTextureFilter(State.font.texture, TEXTURE_FILTER_BILINEAR);
}

static void
draw(void) {
	BeginDrawing();
	ClearBackground(BLACK);
	const char *text = "Congrats! You created your first window!";
	const int font_size = 20;
	Vector2 text_size = MeasureTextEx(State.font, text, font_size, 0);
	Vector2 text_pos = (Vector2){State.win_w/2 - text_size.x/2,
		State.win_h/2 - text_size.y/2};
	DrawTextEx(State.font, text, text_pos, font_size, 0, WHITE);
	EndDrawing();
}

static void
run(void) {
	if (WindowShouldClose())
		State.quit = true;
	draw();
}

static void
cleanup(void) {
	UnloadFont(State.font);
	CloseWindow();
}

int
main(int argc, char **argv) {
	if (argc > 1) {
		fputs(project_description, stderr);
		return EXIT_FAILURE;
	}
	fprintf(stderr, "%s: Use option '--help' to show more "
		"information.\n", argv[0]);
	setup();
	while (!State.quit)
		run();
	cleanup();
	return EXIT_SUCCESS;
}
