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

// ----

#define OFF2PTR(off)  (((char*) NULL) + off)

void config_attr_info(GLAttrInfo *ai, unsigned count, int vidx, unsigned stride) {
	unsigned n;
	for (n = 0; n < count; n++, ai++) {
		if (ai->vidx != vidx) continue;
		if (ai->index < 0)
			continue;
		if (ai->kind == KIND_ATTRIB_POINTER) {
			printf("VTX ATR idx=%d sz=%d type=%d norm=%d stride=%d off=%d\n",
				ai->index, ai->size, ai->type, ai->normalized, stride, ai->pointer);
			glVertexAttribPointer(ai->index, ai->size, ai->type,
				ai->normalized, stride, OFF2PTR(ai->pointer));
			CHECK();
		} else {
			printf("VTX ATR idx=%d sz=%d type=%d stride=%d off=%d\n",
				ai->index, ai->size, ai->type, stride, ai->pointer);
			glVertexAttribIPointer(ai->index, ai->size, ai->type,
				stride, OFF2PTR(ai->pointer));
			CHECK();
		}
		if (ai->divisor) {
			glVertexAttribDivisor(ai->index, ai->divisor);
			CHECK();
		}
		glEnableVertexAttribArray(ai->index);
	}
}

void bind_attr_info(GLAttrInfo *ai, AttribInfo *in, unsigned count, unsigned pgm) {
	for (int n = 0; n < count; n++, ai++, in++) {
		ai->index = glGetAttribLocation(pgm, in->name);
		if (ai->index < 0)
			printx("WARNING: cannot find attribute '%s' (late)\n", in->name);
	}
}

/* map attribute names to indices, and adapt layout parameters for GL */
int setup_attr_info(GLAttrInfo **out, AttribInfo *in, unsigned count, unsigned pgm) {
	GLAttrInfo *ai = (GLAttrInfo*) malloc(sizeof(GLAttrInfo) * count);
	unsigned n;
	if (!ai)
		return error("Out of Memory");
	memset(ai, 0, sizeof(GLAttrInfo) * count);
	for (n = 0; n < count; n++, in++) {
		ai[n].vidx = in->slot;	
		ai[n].index = glGetAttribLocation(pgm, in->name);
		if (ai[n].index < 0)
			printx("WARNING: cannot find attribute '%s' (early)\n", in->name);
		ai[n].pointer = in->offset;
		ai[n].divisor = in->divisor;
		switch (in->format) {
		case FMT_32x4_FLOAT:
			ai[n].kind = KIND_ATTRIB_POINTER;
			ai[n].size = 4;
			ai[n].type = GL_FLOAT;
			ai[n].normalized = GL_FALSE;
			break;
		case FMT_32x3_FLOAT:
			ai[n].kind = KIND_ATTRIB_POINTER;
			ai[n].size = 3;
			ai[n].type = GL_FLOAT;
			ai[n].normalized = GL_FALSE;
			break;
		case FMT_32x2_FLOAT:
			ai[n].kind = KIND_ATTRIB_POINTER;
			ai[n].size = 2;
			ai[n].type = GL_FLOAT;
			ai[n].normalized = GL_FALSE;
			break;
		case FMT_32x1_FLOAT:
			ai[n].kind = KIND_ATTRIB_POINTER;
			ai[n].size = 1;
			ai[n].type = GL_FLOAT;
			ai[n].normalized = GL_FALSE;
			break;
		case FMT_8x4_SNORM:
			ai[n].kind = KIND_ATTRIB_POINTER;
			ai[n].size = 4;
			ai[n].type = GL_BYTE;
			ai[n].normalized = GL_TRUE;
			break;
		case FMT_8x4_UNORM:
			ai[n].kind = KIND_ATTRIB_POINTER;
			ai[n].size = 4;
			ai[n].type = GL_UNSIGNED_BYTE;
			ai[n].normalized = GL_TRUE;
			break;
		case FMT_8x2_UNORM:
			ai[n].kind = KIND_ATTRIB_POINTER;
			ai[n].size = 2;
			ai[n].type = GL_UNSIGNED_BYTE;
			ai[n].normalized = GL_TRUE;
			break;
		case FMT_8x1_UNORM:
			ai[n].kind = KIND_ATTRIB_POINTER;
			ai[n].size = 1;
			ai[n].type = GL_UNSIGNED_BYTE;
			ai[n].normalized = GL_TRUE;
			break;
		case FMT_8x4_UINT:
			ai[n].kind = KIND_ATTRIB_IPOINTER;
			ai[n].size = 4;
			ai[n].type = GL_BYTE;
			ai[n].normalized = GL_FALSE;
			break;
		case FMT_8x2_UINT:
			ai[n].kind = KIND_ATTRIB_IPOINTER;
			ai[n].size = 2;
			ai[n].type = GL_BYTE;
			ai[n].normalized = GL_FALSE;
			break;
		case FMT_8x1_UINT:
			ai[n].kind = KIND_ATTRIB_IPOINTER;
			ai[n].size = 1;
			ai[n].type = GL_BYTE;
			ai[n].normalized = GL_FALSE;
			break;
		default:
			return error("unknown format %d\n", in->format);
		}
	}
	*out = ai;
	return 0;
};

void dump_compile_error(unsigned id) {
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

void dump_link_error(unsigned id) {
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

int App::compileShader(VertexShader *vs, const char *fn,
	void *data, unsigned len, int raw,
	AttribInfo *layout, unsigned lcount) {

	unsigned pgm, id;
	int r;

	id = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(id, 1, (const char**) &data, NULL);
	glCompileShader(id);
	glGetShaderiv(id, GL_COMPILE_STATUS, &r);
	if (!r) {
		dump_compile_error(id);
		glDeleteShader(id);
		return error("vertex shader '%s' compile error" ,fn);
	}
	fprintf(stderr,"COMPILE %d\n", r);
	
	/* we need to compile a program to lookup the indices... */
	pgm = glCreateProgram();
	glAttachShader(pgm, id);
	glLinkProgram(pgm);
	glGetProgramiv(pgm, GL_LINK_STATUS, &r);
	if (!r) {
		dump_link_error(id);
		glDeleteProgram(pgm);
		glDeleteShader(id);
		return error("vertex shader '%s' link error", fn);
	}

	r = setup_attr_info(&vs->ai, layout, lcount, pgm);
	glDeleteProgram(pgm);
	if (r < 0) {
		glDeleteShader(id);
		return error("vertex shader '%s' bind error", fn);
	}
	vs->info = layout;
	vs->count = lcount;
	vs->vs = id;
	printx("VertexShader %d compiled\n", id);
	return 0;
}

int App::compileShader(PixelShader *ps, const char *fn,
	void *data, unsigned len, int raw) {
	unsigned id;
	int r;
	id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(id, 1, (const char **) &data, NULL);
	glCompileShader(id);
	glGetShaderiv(id, GL_COMPILE_STATUS, &r);
	if (!r) {
		dump_compile_error(id);
		glDeleteShader(id);
		return error("pixel shader '%s' compile error", fn);
	}
	ps->ps = id;
	printx("PixelShader %d compiled\n", id);
	return 0;
}

int App::loadShader(VertexShader *vs, const char *fn,
	AttribInfo *layout, unsigned lcount) {
	void *data;
	unsigned sz;
	int r;
	if (!(data = load_file(fn, &sz)))
		return error("cannot load file '%s'", fn);
	r = compileShader(vs, fn, data, sz, 0, layout, lcount);
	free(data);
	return r;
}

int App::loadShader(PixelShader *ps, const char *fn) {
	void *data;
	unsigned sz;
	int r;
	if (!(data = load_file(fn, &sz)))
		return error("cannot load file '%s'", fn);
	r = compileShader(ps, fn, data, sz, 0);
	free(data);
	return r;
}

int App::initConfig(InputConfiguration *ic, VertexShader *vs, PixelShader *ps) {
	unsigned pgm, vao;
	int r;
	// TODO program cache
	pgm = glCreateProgram();
	glAttachShader(pgm, vs->vs);
	glAttachShader(pgm, ps->ps);
	glLinkProgram(pgm);
	glGetProgramiv(pgm, GL_LINK_STATUS, &r);
	if (!r) {
		dump_link_error(pgm);
		glDeleteProgram(pgm);
		return error("program link error");
	}
	bind_attr_info(vs->ai, vs->info, vs->count, pgm);
	glGenVertexArrays(1, &vao);
	CHECK();
	glBindVertexArray(ic->vao);
	CHECK();
	ic->vao = vao;
	ic->pgm = pgm;
	ic->vs = vs;
	ic->ps = ps;
	printx("Program %d linked\n", pgm);
	return 0;
}

void App::useConfig(InputConfiguration *ic) {
	this->ic = ic;
	glBindVertexArray(ic->vao);
	CHECK();
	glUseProgram(ic->pgm);
	CHECK();
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

int App::initBuffer(VertexBuffer *vb, void *data, int sz) {
	if (vb->buf)
		glDeleteBuffers(1, &vb->buf);
	glGenBuffers(1, &vb->buf);
	vb->sz = sz;
	if (data) {
		glBindBuffer(GL_ARRAY_BUFFER, vb->buf);	
		CHECK();
		glBufferData(GL_ARRAY_BUFFER, sz, data, GL_STATIC_DRAW);
		CHECK();
	}
	return 0;
}
int App::initBuffer(IndexBuffer *ib, void *data, int sz) {
	glGenBuffers(1, &ib->buf);
	ib->sz = sz;
	if (data) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->buf);
		CHECK();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sz, data, GL_STATIC_DRAW);
		CHECK();
	}
	return 0;
}
int App::initBuffer(UniformBuffer *ub, void *data, int sz) {
	glGenBuffers(1, &ub->buf);
	ub->sz = sz;
	if (data) {
		glBindBuffer(GL_UNIFORM_BUFFER, ub->buf);
		CHECK();
		glBufferData(GL_UNIFORM_BUFFER, sz, data, GL_STATIC_DRAW);
		CHECK();
	}
	return 0;
}

void App::updateBuffer(VertexBuffer *vb, void *data) {
	glBindBuffer(GL_ARRAY_BUFFER, vb->buf);
	CHECK();
	glBufferData(GL_ARRAY_BUFFER, vb->sz, data, GL_STATIC_DRAW);
	CHECK();
}
void App::updateBuffer(IndexBuffer *ib, void *data) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->buf);
	CHECK();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ib->sz, data, GL_STATIC_DRAW);
	CHECK();
}
void App::updateBuffer(UniformBuffer *ub, void *data) {
	glBindBuffer(GL_UNIFORM_BUFFER, ub->buf);
	CHECK();
	glBufferData(GL_UNIFORM_BUFFER, ub->sz, data, GL_STATIC_DRAW);
	CHECK();
}

void App::useBuffer(VertexBuffer *vb, int slot, unsigned stride, unsigned offset) {
	glBindBuffer(GL_ARRAY_BUFFER, vb->buf);
	CHECK();
	config_attr_info(ic->vs->ai, ic->vs->count, slot, stride);
	ic->vbuf[slot] = vb;
}
void App::useBuffer(IndexBuffer *ib) {
	ic->ibuf = ib;
}
void App::useBuffer(UniformBuffer *ub, int slot) {
	if (ic->ubuf[slot] != ub) {
		ic->ubuf[slot] = ub;
		/* simple 1:1 binding */
		// TODO: smarting binding, avoid having to glBindBufferBase() on every draw
		glUniformBlockBinding(ic->pgm, slot, slot);
	}
}
void App::useTexture(Texture2D *tex, int slot) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex->tex);
}

void _prepare(InputConfiguration *ic) {
	int n;
	if (ic->ibuf)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ic->ibuf->buf);
	for (n = 0; n < 16; n++)
		if (ic->ubuf[n])
			glBindBufferBase(GL_UNIFORM_BUFFER, n, ic->ubuf[n]->buf);
}
void App::drawIndexedInstanced(unsigned numindices, unsigned numinstances) {
	_prepare(ic);
	glDrawElementsInstanced(GL_TRIANGLES, numindices, GL_UNSIGNED_SHORT, NULL, numinstances);
	CHECK();
}
void App::drawInstanced(unsigned numvertices, unsigned numinstances) {
	_prepare(ic);
	glDrawArraysInstanced(GL_TRIANGLES, 0, numvertices, numinstances);
	CHECK();
}
void App::drawIndexed(unsigned numindices) {
	_prepare(ic);
	glDrawElements(GL_TRIANGLES, numindices, GL_UNSIGNED_SHORT, NULL);
	CHECK();
}

void App::setBlend(int enable) {
	if (enable)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
}
