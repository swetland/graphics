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

int Texture2D::load(const char *fn, int options) {
	void *data;
	unsigned dw, dh;
	int r;
	if (!(data = load_png_rgba(fn, &dw, &dh, options)))
		return error("cannot load '%s'", fn);
	r = load(data, dw, dh, options);
	free(data);
	return r;
}

int Texture2D::load(void *data, unsigned w, unsigned h, int options) {
	if (id == 0)
		glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	if (options & OPT_TEX2D_GRAY)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
			GL_RED, GL_UNSIGNED_BYTE, data);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, data);
	if (options & OPT_TEX2D_GEN_MIPMAP)
		glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	width = w;
	height = h;
	return 0;
}

