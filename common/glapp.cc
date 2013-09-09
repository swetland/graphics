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

/* flag opengl.h to include the extension table */
#define GLXTN

#include "app.h"
#include "util.h"

static void gl_map_functions(void) {
	int n;
	for (n = 0; n < sizeof(fntb)/sizeof(fntb[0]); n++) {
		*fntb[n].func = SDL_GL_GetProcAddress(fntb[n].name);
		if (!(*fntb[n].func))
			die("cannot find func '%s'", fntb[n].name);
	}
}

static void quit(void) {
	SDL_Quit();
	exit(0);
}

void App::handleEvents(void) {
	unsigned code;
	SDL_Event ev;

	while (SDL_PollEvent(&ev)) {
		switch (ev.type) {
		case SDL_WINDOWEVENT:
			switch (ev.window.event) {
			case SDL_WINDOWEVENT_RESIZED:
				width = ev.window.data1;
				height = ev.window.data2;
				glViewport(0, 0, width, height);
				onResize();
			}
			break;
		case SDL_KEYDOWN:
			code = ev.key.keysym.scancode;
			keystate[code >> 5] |= (1 << (code & 0x1F));
			if (code == SDL_SCANCODE_ESCAPE)
				quit();
			onKeyDown(code);
			break;
		case SDL_KEYUP:
			code = ev.key.keysym.scancode;
			keystate[code >> 5] &= ~(1 << (code & 0x1F));
			onKeyUp(code);
			break;
		case SDL_QUIT:
			quit();
		}
	}
}

void App::setOptions(int argc, char **argv) {
	char *x;
	argc--;
	argv++;
	while (argc--) {
		if (!strcmp("-nosync", argv[0])) {
			_vsync = 0;
		} else if (!strcmp("-fullscreen", argv[0])) {
			_fullscreen = 1;
		} else if (!strcmp("-fs", argv[0])) {
			_fullscreen = 1;
		} else if (isdigit(argv[0][0]) && (x = strchr(argv[0],'x'))) {
			width = atoi(argv[0]);
			height = atoi(x + 1);
		} else {
			fprintf(stderr,"unknown argument '%s'\n",argv[0]);
		}
		argv++;
	}
}

static void dump_gl_params(void) {
#define GGI(name) { int n = -1; glGetIntegerv(GL_##name, &n); fprintf(stderr, #name ": %d\n", n); }

	GGI(MAX_TEXTURE_SIZE);
	GGI(MAX_3D_TEXTURE_SIZE);
	GGI(MAX_RECTANGLE_TEXTURE_SIZE);
	GGI(MAX_CUBE_MAP_TEXTURE_SIZE);
	GGI(MAX_ARRAY_TEXTURE_LAYERS);

	GGI(MAX_ELEMENTS_INDICES);
	GGI(MAX_ELEMENTS_VERTICES);
	GGI(MAX_DRAW_BUFFERS);
	GGI(MAX_RENDERBUFFER_SIZE);

	GGI(MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS);
	GGI(MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS);
	GGI(MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS);
	GGI(MAX_COMBINED_UNIFORM_BLOCKS);
	GGI(MAX_COMBINED_TEXTURE_IMAGE_UNITS);

	GGI(MAX_VERTEX_ATTRIBS);
	GGI(MAX_VERTEX_OUTPUT_COMPONENTS);
	GGI(MAX_VERTEX_UNIFORM_BLOCKS);
	GGI(MAX_VERTEX_UNIFORM_COMPONENTS);
	GGI(MAX_VERTEX_TEXTURE_IMAGE_UNITS);

	GGI(MAX_GEOMETRY_INPUT_COMPONENTS);
	GGI(MAX_GEOMETRY_OUTPUT_COMPONENTS);
	GGI(MAX_GEOMETRY_UNIFORM_BLOCKS);
	GGI(MAX_GEOMETRY_UNIFORM_COMPONENTS);
	GGI(MAX_GEOMETRY_TEXTURE_IMAGE_UNITS);

	GGI(MAX_VARYING_COMPONENTS);
	GGI(MAX_VARYING_FLOATS);
	GGI(MAX_FRAGMENT_INPUT_COMPONENTS);
	GGI(MAX_FRAGMENT_UNIFORM_BLOCKS);
	GGI(MAX_FRAGMENT_UNIFORM_COMPONENTS);
	GGI(MAX_TEXTURE_IMAGE_UNITS);

	GGI(MAX_UNIFORM_BUFFER_BINDINGS);
	GGI(MAX_UNIFORM_BLOCK_SIZE);
	GGI(UNIFORM_BUFFER_OFFSET_ALIGNMENT);

	GGI(NUM_EXTENSIONS);	
}

static const char *dbg_source(GLenum n) {
	switch (n) {
	case GL_DEBUG_SOURCE_API_ARB: return "api";
	case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: return "compiler";
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB: return "winsys";
	case GL_DEBUG_SOURCE_THIRD_PARTY_ARB: return "3rdpty";
	case GL_DEBUG_SOURCE_APPLICATION_ARB: return "app";
	case GL_DEBUG_SOURCE_OTHER_ARB: return "other";
	default: return "unknown";
	}
}
static const char *dbg_type(GLenum n) {
	switch (n) {
	case GL_DEBUG_TYPE_ERROR_ARB: return "error";
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB: return "undef";
	case GL_DEBUG_TYPE_PERFORMANCE_ARB: return "perf";
	case GL_DEBUG_TYPE_PORTABILITY_ARB: return "portability";
	case GL_DEBUG_TYPE_OTHER_ARB: return "other";
	default: return "unknown";
	}
}
static const char *dbg_severity(GLenum n) {
	switch (n) {
	case GL_DEBUG_SEVERITY_HIGH_ARB: return "high";
	case GL_DEBUG_SEVERITY_MEDIUM_ARB: return "medium";
	case GL_DEBUG_SEVERITY_LOW_ARB: return "low";
	default: return "unknown";
	}
}
static void APIENTRY dbg_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar *message, GLvoid *cookie) {
#if VERBOSE
	error("%s: %s: %x: %s: %s",
		dbg_source(source), dbg_type(type), id,
		dbg_severity(severity), message);
#else
	error("GL: %s", message);
#endif
}

void App::fullscreen(int yes) {
	if (yes) {
		SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN);
		_fullscreen = 1;
	} else {
		SDL_SetWindowFullscreen(win, 0);
		_fullscreen = 0;
	}
}

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

	unsigned flags = SDL_WINDOW_OPENGL;
	if (_fullscreen)
		flags |= SDL_WINDOW_FULLSCREEN;
	//flags |= SDL_WINDOW_RESIZABLE;

	win = SDL_CreateWindow("Application",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height, flags);

	SDL_GetWindowSize(win, &width, &height);
	printx("Window is %d x %d.\n", width, height);

	int minor = 3;
	while (minor > 0) {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
		glcontext = SDL_GL_CreateContext(win);
		if (glcontext)
			break;
		minor--;
	}
	if (!glcontext) {
		fprintf(stderr,"cannot obtain OpenGL 3.1+ context\n");
		exit(1);
	} else {
		fprintf(stderr,"using OpenGL 3.%d\n", minor);
		/* todo: verify extension availability */
	}

	gl_map_functions();

	{ // TODO: filter or disable in release mode
		PFNGLDEBUGMESSAGECALLBACKARBPROC fn;
		fn = (PFNGLDEBUGMESSAGECALLBACKARBPROC)
			SDL_GL_GetProcAddress("glDebugMessageCallbackARB");
		if (fn) {
			fn(dbg_callback, NULL);
			glEnable(GL_DEBUG_OUTPUT);
			// glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		} else {
			fprintf(stderr,"warning: no glDebugMessageCallbackARB()\n");
		}
	}

	fprintf(stderr, "Vender: %s\n", glGetString(GL_VENDOR));
	fprintf(stderr, "Renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stderr, "GL Version: %s\n", glGetString(GL_VERSION));
	fprintf(stderr, "GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	dump_gl_params();

	SDL_GL_SetSwapInterval(_vsync);

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

App::App() : width(800), height(600), _vsync(1), _fullscreen(0) {
}

App::~App() {
}

void init_io(void);

int main(int argc, char **argv) {
	init_io();
	return App::__main(argc, argv);
}

int App::__main(int argc, char **argv) {
	App *app = createApp();
	app->setOptions(argc, argv);
	app->start();
	app->release();
	return 0;
}

