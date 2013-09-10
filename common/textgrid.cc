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

int TextGrid::init(int cellw, int cellh, int columns, int rows) {
	VertexBuffer *data[] = {
		&vtx,
		&vtx,
		&cbuf,
	};
	float box_2d[4 * 6];
	width = columns;
	height = rows;
	dirty = 0;

	grid = (unsigned char*) malloc(width * height);
	if (!grid)
		return -1;
	clear();

	if (texture.load("font-vincent-8x8.png", 0))
		return -1;
	if (!(effect = Effect::load("textgrid")))
		return -1;

	// scale quad to character cell and texture cell size
	for (int n = 0; n < (4 * 6); n += 4) {
		box_2d[n + 0] = unit_box_2d[n + 0] * float(cellw);
		box_2d[n + 1] = unit_box_2d[n + 1] * float(cellh);
		box_2d[n + 2] = unit_box_2d[n + 2] * (1.0f / 16.0f);
		box_2d[n + 3] = unit_box_2d[n + 3] * (1.0f / 16.0f);
	}

	vtx.load(box_2d, sizeof(box_2d));
	cbuf.load(grid, width * height);
	attr.init(layout, data, sizeof(layout) / sizeof(layout[0]));

	return 0;
}

void TextGrid::render(void) {
	if (dirty) {
		dirty = 0;
		cbuf.load(grid, width * height);
	}
	attr.use();
	texture.use(0);
	effect->apply();
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
