/* Copyright 2013 Brian Swetland <swetland@frotz.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>

#include <time.h>

#ifdef _WIN32
#define GLUE_DEFINE_EXTENSIONS 1
#endif

#include "app.h"
#include "util.h"

#ifdef _WIN32
static void gl_map_functions(void) {
	int n;
	if (!SDL_GL_ExtensionSupported("GL_ARB_shader_objects") ||
		!SDL_GL_ExtensionSupported("GL_ARB_shading_language_100") ||
		!SDL_GL_ExtensionSupported("GL_ARB_vertex_shader") ||
		!SDL_GL_ExtensionSupported("GL_ARB_fragment_shader"))
		die("missing glsl extensions");
	for (n = 0; n < sizeof(fntb)/sizeof(fntb[0]); n++) {
		*fntb[n].func = SDL_GL_GetProcAddress(fntb[n].name);
		if (!(*fntb[n].func))
			die("cannot find func '%s'", fntb[n].name);
	}
}
#else
void gl_map_functions(void) {}
#endif

static void quit(void) {
	SDL_Quit();
	exit(0);
}

void App::handleEvents(void) {
	unsigned code;
	SDL_Event ev;

	while (SDL_PollEvent(&ev)) {
		switch (ev.type) {
		case SDL_KEYDOWN:
			code = ev.key.keysym.scancode;
			keystate[code >> 5] |= (1 << (code & 0x1F));
			if (code == SDL_SCANCODE_ESCAPE)
				quit();
			break;
		case SDL_KEYUP:
			code = ev.key.keysym.scancode;
			keystate[code >> 5] &= ~(1 << (code & 0x1F));
			break;
		case SDL_QUIT:
			quit();
		}
	}
}

#if 0
void App::setOptions(int argc, char **argv) {
	char *x;
	argc--;
	argv++;
	while (argc--) {
		if (!strcmp("-nosync",argv[0])) {
			_vsync = 0;
		} else if (isdigit(argv[0][0]) && (x = strchr(argv[0],'x'))) {
			_width = atoi(argv[0]);
			_height = atoi(x + 1);
		} else {
			fprintf(stderr,"unknown argument '%s'\n",argv[0]);
		}
		argv++;
	}
}
#endif

int App::start(void) {
	memset(keystate, 0, sizeof(keystate));

	if (SDL_Init(SDL_INIT_VIDEO))
		die("sdl video init failed");

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
 
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);

	/* enable vsync */
	//SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, _vsync);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, _vsync);

	win = SDL_CreateWindow("Application",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height, SDL_WINDOW_OPENGL
	       	// | SDL_WINDOW_RESIZABLE
		// | SDL_WINDOW_FULLSCREEN
		);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	glcontext = SDL_GL_CreateContext(win);

#if FALLBACK_TO_GL32
	if (!glcontext) {
		fprintf(stderr, "oops?\n");
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
		glcontext = SDL_GL_CreateContext(win);
		/* query needed extensions, etc */
	}
#endif

	SDL_GL_SetSwapInterval(_vsync);

	gl_map_functions();

	if (init())
		return -1;

	time_t t0, t1;
	unsigned count;
	t0 = t1 = time(NULL);
	count = 0;

	for (;;) {
		handleEvents();

		unsigned b = SDL_GetRelativeMouseState(&mouseDX, &mouseDY);
		mouseBTN = (b & 1) | ((b & 2) << 1) | ((b & 4) >> 1);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		render();

		if (_vsync) {
			SDL_GL_SwapWindow(win);
		} else {
			glFlush();
		}
		count++;
		t1 = time(NULL);
		if (t0 != t1) {
			fps = count;
			count = 0;
			t0 = t1;
		}
	}
	return -1;
}

App::App() : width(800), height(600), _vsync(1) {
}

App::~App() {
}

void init_io(void);

int main(int argc, char **argv) {
	init_io();
	App *app = createApp();
	app->start();
	app->release();
	return 0;
}

