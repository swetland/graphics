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
	SRC_INT8,
	SRC_UINT8,
	SRC_INT16,
	SRC_UINT16,
	SRC_INT32,
	SRC_UINT32,
	SRC_FLOAT,
};

enum {
	DST_FLOAT,
	DST_NORMALIZED,
	DST_INTEGER,
};

struct VertexAttrDesc {
	unsigned index;
	unsigned src_type;	// data type
	unsigned dst_type;	// data type
	unsigned count;		// number of components (1-4)
	unsigned offset;	// offset of first attr
	unsigned stride;	// offset from first attr to next
	unsigned divisor;	// how often to step forward
	unsigned unused;
};

struct PixelShader {
	unsigned id;
	PixelShader() : id(0) {};
	~PixelShader() { if (id) glDeleteShader(id); };
	int load(const char *fn);
};

struct VertexShader {
	unsigned id;
	VertexShader() : id(0) {};
	~VertexShader() { if (id) { glDeleteShader(id); } };
	int load(const char *fn);
};

struct Program {
	unsigned id;
	Program() : id(0) {};
	~Program() { if (id) { glDeleteProgram(id); } };
	void use(void) { glUseProgram(id); }
	int link(VertexShader *vs, PixelShader *ps);
};

struct Texture2D {
	unsigned tex;
	Texture2D() : tex(0) {};
	~Texture2D() { if (tex) { glDeleteTextures(1, &tex); } };
};

struct UniformBuffer {
	unsigned id;
	unsigned sz;
	UniformBuffer() : id(0) {};
	~UniformBuffer() { if (id) glDeleteBuffers(1, &id); };
	void use(unsigned index) { glBindBufferBase(GL_UNIFORM_BUFFER, index, id); };
	void load(void *data, unsigned size);
};

// TODO: when do we glUniformBLockBinding() ?

struct VertexBuffer {
	unsigned id;
	unsigned sz;
	VertexBuffer() : id(0) {};
	~VertexBuffer() { if (id) glDeleteBuffers(1, &id); };
	void use(void) { glBindBuffer(GL_ARRAY_BUFFER, id); };
	void load(void *data, unsigned size);
};

struct IndexBuffer {
	unsigned id;
	unsigned sz;
	IndexBuffer() : id(0) {};
	~IndexBuffer() { if (id) glDeleteBuffers(1, &id); };
	void use(void) { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id); };
	void load(void *data, unsigned size);
};

struct VertexAttributes {
	unsigned vao;
	VertexAttributes() : vao(0) {};
	~VertexAttributes() { /* todo */ };
	void init(VertexAttrDesc *desc, VertexBuffer **data, unsigned count);
	void use(void) { glBindVertexArray(vao); };
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

	int loadTextureRGBA(Texture2D *tex, const char *fn, int genmips);
	int createTextureRGBA(Texture2D *tex, void *data, unsigned w, unsigned h, int genmips);

	void useTexture(Texture2D *tex, int slot);
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
