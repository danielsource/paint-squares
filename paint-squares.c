#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

#include "assets.h"

#define LENGTH(arr) ((int)(sizeof (arr) / sizeof *(arr)))
#define UNUSED(x) (void)(x)

enum canvas_split {
	CANVAS_VERTICAL,
	CANVAS_HORIZONTAL
};

enum canvas_color {
	CANVAS_CLR_BG,
	CANVAS_CLR_BORDER,
	CANVAS_CLR_1,
	CANVAS_CLR_2,
	CANVAS_CLR_3,
	CANVAS_COLOR_COUNT,
};

union arg {
	void *v;
};

struct canvas_node {
	Rectangle rect;
	enum canvas_split direction;
	struct canvas_node *left, *right;
	enum canvas_color color_id; // TODO: generate random COLOR_CLR_[1-9]+
};

struct global_state {
	struct canvas_node *canvas_tree;
	Color canvas_colors[CANVAS_COLOR_COUNT];
	Font font;
	int win_w, win_h;
	const char *win_title;
	int target_fps;
	volatile sig_atomic_t quit;
};

typedef void keybinding_callback(const union arg *arg);

struct keybinding {
	int key;
	keybinding_callback *cb;
	const union arg *arg;
};

typedef void mousebinding_callback(Vector2 cursor_pos,
				   const union arg *arg);

struct mousebinding {
	int btn;
	mousebinding_callback *cb;
	const union arg *arg;
};

static struct global_state state = {
	.canvas_colors = {
		[CANVAS_CLR_BG]     = {0xe3, 0xe1, 0xe2, 0xff}, // #e3e1e2
		[CANVAS_CLR_BORDER] = {0x05, 0x0a, 0x06, 0xff}, // #050a06
		[CANVAS_CLR_1]      = {0xdf, 0x29, 0x1e, 0xff}, // #df291e
		[CANVAS_CLR_2]      = {0xee, 0xdd, 0x6f, 0xff}, // #eedd6f
		[CANVAS_CLR_3]      = {0x01, 0x58, 0x9b, 0xff}, // #01589b
	},
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
"The code can be found at " PROJECT_REPO_LINK ".\n"
#endif
;

static void
canvas_delete_node(struct canvas_node *n) {
	if (!n)
		return;
	canvas_delete_node(n->left);
	canvas_delete_node(n->right);
	free(n);
}

static void
canvas_exit(const union arg *arg) {
	UNUSED(arg);
	state.quit = true;
}

static struct canvas_node *
canvas_create_node(Rectangle r, enum canvas_split direction) {
	struct canvas_node *n = calloc(1, sizeof (*n));
	if (!n) {
		fprintf(stderr, PROJECT_NAME ": Fatal error: %s.\n",
			strerror(errno));
		canvas_exit(NULL);
		return NULL;
	}
	n->rect = r;
	n->direction = direction;
	return n;
}

static void
canvas_clear(const union arg *arg) {
	UNUSED(arg);
	canvas_delete_node(state.canvas_tree);
	Rectangle r = {0, 0, state.win_w, state.win_h};
	state.canvas_tree = canvas_create_node(r, CANVAS_VERTICAL);
}

static void
canvas_split_node(Vector2 delim, struct canvas_node *n) {
	if (!n)
		return;
	if (n->left &&
	    delim.x >= n->left->rect.x &&
	    delim.y >= n->left->rect.y &&
	    delim.x <= n->left->rect.x + n->left->rect.width &&
	    delim.y <= n->left->rect.y + n->left->rect.height) {
		canvas_split_node(delim, n->left);
		return;
	}
	if (n->right &&
	    delim.x >= n->right->rect.x &&
	    delim.y >= n->right->rect.y &&
	    delim.x <= n->right->rect.x + n->right->rect.width &&
	    delim.y <= n->right->rect.y + n->right->rect.height) {
		canvas_split_node(delim, n->right);
		return;
	}
	Rectangle l, r;
	switch (n->direction) {
	case CANVAS_VERTICAL:
		l = (Rectangle){
			n->rect.x,           n->rect.y,
			delim.x - n->rect.x, n->rect.height
		};
		r = (Rectangle){
			delim.x,                 n->rect.y,
			n->rect.width - l.width, n->rect.height
		};
		n->left = canvas_create_node(l, CANVAS_HORIZONTAL);
		n->right = canvas_create_node(r, CANVAS_HORIZONTAL);
		break;
	case CANVAS_HORIZONTAL:
		l = (Rectangle){
			n->rect.x,     n->rect.y,
			n->rect.width, delim.y - n->rect.y
		};
		r = (Rectangle){
			n->rect.x,     delim.y,
			n->rect.width, n->rect.height - l.height
		};
		n->left = canvas_create_node(l, CANVAS_VERTICAL);
		n->right = canvas_create_node(r, CANVAS_VERTICAL);
		break;
	}
#if 0 // TEMP: debug
	fprintf(stderr, "\n");
	fprintf(stderr, "%f %f %f %f\n", l.x, l.y, l.width, l.height);
	fprintf(stderr, "%f %f %f %f\n", r.x, r.y, r.width, r.height);
	fprintf(stderr, "\n");
#endif // TEMP: debug
}

static void
canvas_split_rect(Vector2 cursor_pos, const union arg *arg) {
	UNUSED(arg);
#if 0 // TEMP: debug
	fprintf(stderr, "%fx%f\n", cursor_pos.x, cursor_pos.y);
#endif // TEMP: debug
	canvas_split_node(cursor_pos, state.canvas_tree);
}

static void
canvas_draw_node(struct canvas_node *n) {
	if (!n)
		return;
	if (n->left || n->right) {
		canvas_draw_node(n->left);
		canvas_draw_node(n->right);
		return;
	}
	DrawRectangleRec(n->rect, state.canvas_colors[n->color_id]);
	DrawRectangleLinesEx(n->rect, 5, state.canvas_colors[CANVAS_CLR_BORDER]);
}

static void
canvas_draw(void) {
	canvas_draw_node(state.canvas_tree);
}

static const struct keybinding keybindings[] = {
	{.key = KEY_ESCAPE, .cb = canvas_exit},
	{.key = KEY_C,      .cb = canvas_clear},
};

static const struct mousebinding mousebindings[] = {
	{.btn = MOUSE_BUTTON_LEFT, .cb = canvas_split_rect},
};

static void
signal_handler(int sig) {
	if (sig == SIGINT)
		canvas_exit(NULL);
}

static void
setup(void) {
	signal(SIGINT, signal_handler);
	SetTraceLogLevel(LOG_WARNING);
	InitWindow(state.win_w, state.win_h, state.win_title);
	SetExitKey(KEY_NULL);
	SetTargetFPS(state.target_fps);
	state.font = LoadFontFromMemory(".ttf",
					ASSET_START(font_monospace),
					ASSET_SIZE(font_monospace),
					50,
					NULL,
					0);
	SetTextureFilter(state.font.texture, TEXTURE_FILTER_BILINEAR);
	canvas_clear(NULL);
}

static void
handle_input() {
	if (WindowShouldClose()) {
		state.quit = true;
		return;
	}
	int key;
	while ((key = GetKeyPressed()) != KEY_NULL) {
		for (int i = 0; i < LENGTH(keybindings); i++) {
			if (key == keybindings[i].key) {
				keybindings[i].cb(keybindings[i].arg);
				break;
			}
		}
	}
	for (int i = 0; i < LENGTH(mousebindings); i++)
		if (IsMouseButtonPressed(mousebindings[i].btn))
			mousebindings[i].cb(GetMousePosition(), mousebindings[i].arg);
}

static void
draw(void) {
	BeginDrawing();
	ClearBackground(BLACK);
	canvas_draw();
	EndDrawing();
}

static void
run(void) {
	handle_input();
	draw();
}

static void
cleanup(void) {
	canvas_clear(NULL);
	UnloadFont(state.font);
	CloseWindow();
}

int
main(int argc, char **argv) {
	UNUSED(argv);
	if (argc > 1) {
		fputs(project_description, stderr);
		return EXIT_FAILURE;
	}
	fputs(PROJECT_NAME ": Use option '--help' to show more "
	      "information.\n", stderr);
	setup();
	while (!state.quit)
		run();
	fputs(PROJECT_NAME ": Exiting...\n", stderr);
	cleanup();
	return EXIT_SUCCESS;
}
