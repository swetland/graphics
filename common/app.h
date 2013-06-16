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

#ifndef _GL_APP_H_
#define _GL_APP_H_

#ifdef _WIN32
#define NO_SDL_GLEXT 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "opengl.h"
#else
#define GL_GLEXT_PROTOTYPES 1
#define NO_SDL_GLEXT 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glcorearb.h"
#endif

#include <math.h>

enum {
	FMT_32x4_FLOAT = 1,
	FMT_32x3_FLOAT = 2,
	FMT_32x2_FLOAT = 3,
	FMT_32x1_FLOAT = 4,

	FMT_8x4_SNORM = 5,

	FMT_8x4_UNORM = 6,
	FMT_8x2_UNORM = 7,
	FMT_8x1_UNORM = 8,

	FMT_8x4_UINT = 9,
	FMT_8x2_UINT = 10,
	FMT_8x1_UINT = 11,
};

enum {
	VERTEX_DATA = 1,
	INSTANCE_DATA = 2,
};

struct AttribInfo {
	const char *name;
	unsigned nidx;
	unsigned format;
	unsigned slot;
	unsigned offset;
	unsigned type;
	unsigned divisor;
};


struct GLAttrInfo {
#define KIND_ATTRIB_POINTER 0
#define KIND_ATTRIB_IPOINTER 1
	int kind;

/* which VBO {0...} to associate with */
	int vidx;

/* arguments for VertexAttrib[I]Pointer() */
	int index;
	int size;
	int type;
	int normalized;
	int pointer;
	int divisor;
};

struct PixelShader {
	unsigned ps;
	PixelShader() : ps(0) {};
	~PixelShader() { if (ps) glDeleteShader(ps); };
};

struct VertexShader {
	unsigned vs;
	GLAttrInfo *ai;
	AttribInfo *info;
	unsigned count;
	VertexShader() : vs(0), ai(NULL), info(NULL), count(0) {};
	~VertexShader() { if (vs) { glDeleteShader(vs); free(ai); } };
};

struct Texture2D {
	unsigned tex;
	Texture2D() : tex(0) {};
	~Texture2D() { if (tex) { glDeleteTextures(1, &tex); } };
};

struct UniformBuffer {
	unsigned buf;
	unsigned sz;
	UniformBuffer() : buf(0) {};
	~UniformBuffer() { if (buf) glDeleteBuffers(1, &buf); };
};

struct VertexBuffer {
	unsigned buf;
	unsigned sz;
	VertexBuffer() : buf(0) {};
	~VertexBuffer() { if (buf) glDeleteBuffers(1, &buf); };
};

struct IndexBuffer {
	unsigned buf;
	unsigned sz;
	IndexBuffer() : buf(0) {};
	~IndexBuffer() { if (buf) glDeleteBuffers(1, &buf); };
};

struct InputConfiguration {
	PixelShader *ps;
	VertexShader *vs;
	unsigned vao;
	unsigned pgm;
	VertexBuffer *vbuf[16];
	UniformBuffer *ubuf[16];
	IndexBuffer *ibuf;
	InputConfiguration() : ps(NULL), vs(NULL), vao(0), pgm(0), ibuf(NULL) {
		memset(vbuf, 0, sizeof(vbuf)); memset(ubuf, 0, sizeof(ubuf)); };
	~InputConfiguration() { if (vao) glDeleteVertexArrays(1, &vao); if (pgm) glDeleteProgram(pgm); };
};

class App {
public:
	App();
	virtual ~App();

	virtual int init(void) = 0;
	virtual void render(void) = 0;
	virtual void release(void) {};

	/* glue - do not use */
	int start(void);
	void handleEvents(void);

	// TODO: move away from D3D10_INPUT...
	int compileShader(VertexShader *vs, const char *fn,
		void *data, unsigned len, int raw,
		AttribInfo *layout, unsigned lcount);
	int compileShader(PixelShader *ps, const char *fn,
		void *data, unsigned len, int raw);
	int loadShader(VertexShader *vs, const char *fn,
		AttribInfo *layout, unsigned lcount);
	int loadShader(PixelShader *ps, const char *fn);

	int loadTextureRGBA(Texture2D *tex, const char *fn, int genmips);
	int createTextureRGBA(Texture2D *tex, void *data, unsigned w, unsigned h, int genmips);

	int initConfig(InputConfiguration *ic, VertexShader *vs, PixelShader *ps);
	int initBuffer(VertexBuffer *vb, void *data, int sz);
	int initBuffer(IndexBuffer *ib, void *data, int sz);
	int initBuffer(UniformBuffer *ub, void *data, int sz);

	void updateBuffer(VertexBuffer *vb, void *data);
	void updateBuffer(IndexBuffer *ib, void *data);
	void updateBuffer(UniformBuffer *ub, void *data);

	void useConfig(InputConfiguration *ic);
	void useBuffer(VertexBuffer *vb, int slot, unsigned stride, unsigned offset);
	void useBuffer(IndexBuffer *ib);
	void useBuffer(UniformBuffer *ub, int slot);
	void useTexture(Texture2D *tex, int slot);
	void drawIndexedInstanced(unsigned numindices, unsigned numinstances);
	void drawInstanced(unsigned numvertices, unsigned numinstances);
	void drawIndexed(unsigned numindices);
	void setBlend(int enable);
protected:
	int width;
	int height;

	/* mouse motion since last frame */
	int mouseDX, mouseDY, mouseDZ;
	/* mouse button state */
	int mouseBTN;
	/* mouse position in window coordinates */
	int mouseWX, mouseWY;
	/* keys down */
	unsigned char keystate[256];

	InputConfiguration *ic;

private:
	int _vsync;
	SDL_Window *win;
	SDL_GLContext glcontext;
};

App *createApp(void);

void printx(const char *fmt, ...);
void printmtx(float *m, const char *name);
int error(const char *fmt, ...);

#endif
