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

#include "util.h"
#include "matrix.h"
#include "app.h"
#include "texturefont.h"

// idx, src, dst, count, offset, stride, divisor
static VertexAttrDesc layout[] = {
	{ 0, SRC_INT16, DST_INTEGER,    4,  0, 16, 0 },
	{ 1, SRC_UINT16, DST_INTEGER,   2,  8, 16, 0 },
	{ 2, SRC_UINT8, DST_NORMALIZED, 4, 12, 16, 0 },
};

int TextureFont::init(App *app, const char *fontname) {
	VertexBuffer *vdata[] = {
		&vtx, &vtx, &vtx,
	};
	char tmp[256];
	unsigned sz;

	header = NULL;
	max = 128;
	count = 0;
	data = (CharData *) malloc(sizeof(CharData) * max);
	if (!data)
		goto fail;
	next = data;

	sprintf(tmp, "%s.font.png", fontname);
	if (glyphs.load(tmp, OPT_TEX2D_GRAY))
		goto fail;

	sprintf(tmp, "%s.font.dat", fontname);
	header = (FontInfo*) load_file(tmp, &sz);
	info = header->info;
	if (sz < sizeof(FontInfo)) {
		error("invalid font header"); 
		goto fail;
	}
	if (header->magic != TEXTUREFONT_MAGIC) {
		error("invalid font magic");
		goto fail;
	}
	if (sz < (sizeof(FontInfo) + sizeof(CharInfo) * header->count)) {
		error("missing font data");
		goto fail;
	}
	first = header->first;
	last = first + header->count - 1;

	if (pgm.load("texturefont.vs", "texturefont.gs", "texturefont.fs"))
		goto fail;

	u.mvp.setOrtho(0, app->getWidth(), app->getHeight(), 0, -1, 1);
	u.dim = glyphs.width;
	u.adj[0] = 0.5 / float(glyphs.width);
	u.adj[1] = 0.5 / float(glyphs.width);
	vtx.load(data, sizeof(CharData) * max);
	ubuf.load(&u, sizeof(u));

	attr.init(layout, vdata, sizeof(layout) / sizeof(layout[0]));

	dirty = 0;

	color = RGBA(255,255,255,255);
	return 0;
fail:
	if (data) {
		free(data);
		data = NULL;
	}
	if (header) {
		free(header);
		header = NULL;
	}

	return 0;
}

void TextureFont::clear(void) {
	next = data;
	count = 0;
}

void TextureFont::setColor(unsigned rgba) {
	color = rgba;
}

void TextureFont::render(App *app) {
	if (count == 0)
		return;

	if (dirty) {
		vtx.load(data, sizeof(CharData) * count);
		dirty = 0;
	}

	pgm.use();
	ubuf.use(0);
	attr.use();
	glyphs.use(0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_POINTS, 0, count);
	glDisable(GL_BLEND);
}

void TextureFont::puts(int x, int y, const char *s) {
	while (count < max) {
		unsigned n = *s++;
		if (n == 0)
			break;
		if ((n < first) || (n > last))
			continue;
		n -= first;
		data[count].x = x + info[n].dx;
		data[count].y = y - info[n].dy;
		data[count].w = info[n].w;
		data[count].h = info[n].h;
		data[count].s = info[n].x;
		data[count].t = info[n].y;
		data[count].rgba = color;
		count++;
		x += info[n].advance;
	}
	dirty = 1;
}

void TextureFont::printf(int x, int y, const char *fmt, ...) {
	char buf[128];
	int len;
	va_list ap;
	va_start(ap, fmt);
	len = vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	buf[127] = 0;
	if (len > 127) len = 127;
	puts(x, y, buf);
}
