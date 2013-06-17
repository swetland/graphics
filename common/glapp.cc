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

#ifdef _WIN32
#define GLUE_DEFINE_EXTENSIONS 1
#endif

#include "app.h"
#include "util.h"

#if 1
#define CHECK() do {} while (0)
#else
const char *__gl_error_string(unsigned e) {
	static char buf[16];
	switch (e) {
	case GL_INVALID_ENUM: return "INVALID ENUM";
	case GL_INVALID_VALUE: return "INVALID VALUE";
	case GL_INVALID_OPERATION: return "INVALID OPERATION";
	case GL_INVALID_FRAMEBUFFER_OPERATION: return "INVALID FRAMEBUFFER OPERATION";
	case GL_OUT_OF_MEMORY: return "OUT OF MEMORY";
	default:
		sprintf(buf,"0x%08x", e);
		return buf;
	}
}

void __check_gl_error(const char *fn, int line) {
	unsigned e;
	while ((e = glGetError()) != GL_NO_ERROR)
		printx("%s:%d: GL ERROR %s\n", fn, line, __gl_error_string(e));
}
#define CHECK()  __check_gl_error(__func__, __LINE__)
#endif

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
	SDL_Event ev;

	while (SDL_PollEvent(&ev)) {
		switch (ev.type) {
		case SDL_KEYDOWN:
			if (ev.key.keysym.sym < 256)
				keystate[ev.key.keysym.sym] = 1;
			if (ev.key.keysym.sym == SDLK_ESCAPE)
				quit();
			break;
		case SDL_KEYUP:
			if (ev.key.keysym.sym < 256)
				keystate[ev.key.keysym.sym] = 0;
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
		width, height, SDL_WINDOW_OPENGL /* | SDL_WINDOW_RESIZABLE */);

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

static unsigned vattr_type(unsigned type) {
	switch (type) {
	case SRC_INT8:   return GL_BYTE;
	case SRC_UINT8:  return GL_UNSIGNED_BYTE;
	case SRC_INT16:  return GL_SHORT;
	case SRC_UINT16: return GL_UNSIGNED_SHORT;
	case SRC_INT32:  return GL_INT;
	case SRC_UINT32: return GL_UNSIGNED_INT;
	case SRC_FLOAT:  return GL_FLOAT;
	default:         return GL_FLOAT;
	}
}

#define OFF2PTR(off)  (((char*) NULL) + off)

void VertexAttributes::init(VertexAttrDesc *desc, VertexBuffer **data, unsigned count) {
	unsigned id = 0, n;
	glGenVertexArrays(1, &vao);
	CHECK();
	glBindVertexArray(vao);
	CHECK();
	for (n = 0; n < count; n++) {
		if (data && (data[n]->id != id)) {
			id = data[n]->id;
			glBindBuffer(GL_ARRAY_BUFFER, id);
			CHECK();
		}
		switch (desc->dst_type) {
		case DST_FLOAT:
			glVertexAttribPointer(desc->index, desc->count,
				vattr_type(desc->src_type), GL_FALSE, desc->stride,
				OFF2PTR(desc->offset));
			CHECK();
			break;
		case DST_NORMALIZED:
			glVertexAttribPointer(desc->index, desc->count,
				vattr_type(desc->src_type), GL_TRUE, desc->stride,
				OFF2PTR(desc->offset));
			CHECK();
			break;
		case DST_INTEGER:
			glVertexAttribIPointer(desc->index, desc->count,
				vattr_type(desc->src_type), desc->stride,
				OFF2PTR(desc->offset));
			CHECK();
			break;
		}
		glVertexAttribDivisor(desc->index, desc->divisor);
		CHECK();
		glEnableVertexAttribArray(desc->index);
		CHECK();
		desc++;
	}
}


void VertexBuffer::load(void *data, unsigned size) {
	if (id == 0)
		glGenBuffers(1, &id);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	sz = size;
}

void IndexBuffer::load(void *data, unsigned size) {
	if (id == 0)
		glGenBuffers(1, &id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	sz = size;
}

void UniformBuffer::load(void *data, unsigned size) {
	if (id == 0)
		glGenBuffers(1, &id);
	glBindBuffer(GL_UNIFORM_BUFFER, id);
	glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW);
	sz = size;
}

static void dump_compile_error(unsigned id) {
	int len;
	char *buf;
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
	buf = (char*) malloc(len + 1);
	if (buf != 0) {
		memset(buf, 0, len);
		glGetShaderInfoLog(id, len, &len, buf);
		buf[len] = 0;
		fprintf(stderr,"-- shader compiler error --\n%s\n", buf);
		free(buf);
	}
}

static void dump_link_error(unsigned id) {
	int len;
	char *buf;
	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &len);
	buf = (char*) malloc(len + 1);
	if (buf != 0) {
		memset(buf, 0, len);
		glGetProgramInfoLog(id, len, &len, buf);
		buf[len] = 0;
		fprintf(stderr,"-- shader link error --\n%s\n", buf);
		free(buf);
	}
}

int Program::link(VertexShader *vs, PixelShader *ps) {
	unsigned n;
	int r;
	n = glCreateProgram();
	glAttachShader(n, vs->id);
	glAttachShader(n, ps->id);
	glLinkProgram(n);
	glGetProgramiv(n, GL_LINK_STATUS, &r);
	if (!r) {
		dump_link_error(n);
		glDeleteProgram(n);
		return error("shader program link error");
	}
	if (id)
		glDeleteProgram(id);
	id = n;
	return 0;
}

static int _load_shader(unsigned *out, const char *fn, unsigned type) {
	void *data;
	unsigned sz;
	unsigned id;
	int r;
	if (!(data = load_file(fn, &sz)))
		return error("cannot load shader source '%s'", fn);
	id = glCreateShader(type);
	glShaderSource(id, 1, (const char **) &data, NULL);
	glCompileShader(id);
	glGetShaderiv(id, GL_COMPILE_STATUS, &r);
	if (!r) {
		dump_compile_error(id);
		glDeleteShader(id);
		return error("shader '%s' compile error", fn);
	}
	if (*out)
		glDeleteShader(*out);
	*out = id;
	return 0;
}

int PixelShader::load(const char *fn) {
	return _load_shader(&id, fn, GL_FRAGMENT_SHADER);
}

int VertexShader::load(const char *fn) {
	return _load_shader(&id, fn, GL_VERTEX_SHADER);
}

int App::loadTextureRGBA(Texture2D *tex, const char *fn, int genmips) {
	void *data;
	unsigned dw, dh;
	int r;
	if (!(data = load_png_rgba(fn, &dw, &dh, 0)))
		return error("cannot load '%s'", fn);
	r = createTextureRGBA(tex, data, dw, dh, genmips);
	free(data);
	return r;
}
int App::createTextureRGBA(Texture2D *tex, void *data, unsigned w, unsigned h, int genmips) {
	unsigned id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	if (genmips)
		glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	tex->tex = id;
	return 0;
}

void App::useTexture(Texture2D *tex, int slot) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex->tex);
}

void App::setBlend(int enable) {
	if (enable)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
}
