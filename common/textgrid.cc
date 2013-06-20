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
#include <stdarg.h>

#include "app.h"
#include "util.h"
#include "matrix.h"
#include "textgrid.h"

#define BUILTIN 0

#if BUILTIN
#include "TextVS.h"
#include "TextPS.h"
#endif

//static AttribInfo text_layout_desc[] = {
//	{ "POSITION",  0, FMT_32x2_FLOAT, 0, 0, VERTEX_DATA, 0 },
//	{ "TEXCOORD",  0, FMT_32x2_FLOAT, 0, 8, VERTEX_DATA, 0 },
//	{ "CHARACTER", 0, FMT_8x1_UINT,   1, 0, INSTANCE_DATA, 1 },
//};

// idx, src, dst, count, offset, stride, divisor
static VertexAttrDesc layout[] = {
	{ 0, SRC_FLOAT, DST_FLOAT,   2, 0, 16, 0 },
	{ 1, SRC_FLOAT, DST_FLOAT,   2, 8, 16, 0 },
	{ 2, SRC_UINT8, DST_INTEGER, 1, 0,  1, 1 },
};

static float unit_box_2d[] = {
	0, 1,  0, 0,
	0, 0,  0, 1,
	1, 0,  1, 1,
	1, 0,  1, 1,
	1, 1,  1, 0,
	0, 1,  0, 0,
};

int TextGrid::init(App *a, int w, int h) {
	struct {
		mat4 proj;
		unsigned cw;
		unsigned ch;
		unsigned pad0;
		unsigned pad1;
	} cb;
	VertexBuffer *data[] = {
		&vtx,
		&vtx,
		&cbuf,
	};
	width = w;
	height = h;
	dirty = 0;

	grid = (unsigned char*) malloc(w * h);
	if (!grid)
		return -1;
	clear();

	if (texture.load("font-vincent-8x8.png", 0))
		return -1;

	if (ps.load("TextPS.glsl"))
		return -1;
	if (vs.load("TextVS.glsl"))
		return -1;
	if (pgm.link(&vs, &ps))
		return -1;


	cb.proj.setOrtho(0, w - 1, 0, h - 1, -1, 1);	
	cb.cw = width;
	cb.ch = height;

	vtx.load(unit_box_2d, sizeof(unit_box_2d));
	cbuf.load(grid, width * height);
	ubuf.load(&cb, sizeof(cb));

	attr.init(layout, data, sizeof(layout) / sizeof(layout[0]));

	return 0;
}

void TextGrid::render(App *a) {
	if (dirty) {
		dirty = 0;
		cbuf.load(grid, width * height);
	}
	pgm.use();
	attr.use();
	ubuf.use(0);
	texture.use(0);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, width * height);
}

void TextGrid::clear(void) {
	memset(grid, 0, width * height);
}

void TextGrid::printf(int x, int y, const char *fmt, ...) {
	char buf[128];
	int len;
	va_list ap;
	va_start(ap, fmt);
	len = vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	buf[127] = 0;
	if (len > 127) len = 127;
	if (y < 0) y = height + y;
	memcpy(grid + y * width + x, buf, len); // TODO rangecheck
	dirty = 1;
}
