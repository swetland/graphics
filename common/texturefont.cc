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

/* performance experiments to try sometime:
 * - pass color via vtx attr instead of unpacking in shader
 * - pass character vertex table via uniform instead of texturebuffer
 * - instead of DrawArraysInstanced:
 *   - call DrawElements with a fixed 1,1,1,1,1,1,2,2,2,2,2,2,3,... index buffer
 *   - call DrawArrays with unpacked vtx array (6 per character)
 * - explore different vtx formats (int vs float vs short, etc)
*/

// idx, src, dst, count, offset, stride, divisor
static VertexAttrDesc layout[] = {
	{ 0, SRC_INT32, DST_INTEGER, 4, 0, 16, 6 },
};

int TextureFont::init(App *app, const char *fontname) {
	VertexBuffer *vdata[] = { &vtx };
	char tmp[256];
	float *cdata, *cp; 
	float dim, adj;
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

	if (pgm.load("texturefont.vs", "texturefont.fs"))
		goto fail;

	cp = cdata = (float*) malloc(sizeof(float) * 4 * 6 * header->count);

	/* to adjust int coords to texture coords */
	dim = glyphs.width;
	adj = 0.5 / float(glyphs.width);

	/* generate a 2d+uv quad for each character */
	for (unsigned n = 0; n < header->count; n++) {
		*cp++ = 0;
		*cp++ = -info[n].h;
		*cp++ = float(info[n].x) / dim + adj;
		*cp++ = float(info[n].y) / dim + adj;

		*cp++ = info[n].w;
		*cp++ = -info[n].h;
		*cp++ = float(info[n].x + info[n].w - 1) / dim + adj;
		*cp++ = float(info[n].y) / dim + adj;

		*cp++ = 0;
		*cp++ = 0;
		*cp++ = float(info[n].x) / dim + adj;
		*cp++ = float(info[n].y + info[n].h - 1) / dim + adj;

		*cp++ = info[n].w;
		*cp++ = 0;
		*cp++ = float(info[n].x + info[n].w - 1) / dim + adj;
		*cp++ = float(info[n].y + info[n].h - 1) / dim + adj;

		*cp++ = 0;
		*cp++ = 0;
		*cp++ = float(info[n].x) / dim + adj;
		*cp++ = float(info[n].y + info[n].h - 1) / dim + adj;

		*cp++ = info[n].w;
		*cp++ = -info[n].h;
		*cp++ = float(info[n].x + info[n].w - 1) / dim + adj;
		*cp++ = float(info[n].y) / dim + adj;
	}
	cbuf.load(cdata, sizeof(float) * 4 * 6 * header->count);
	vtx.load(data, sizeof(CharData) * max);

	mvp.setOrtho(0, app->getWidth(), app->getHeight(), 0, -1, 1);
	ubuf.load(&mvp, sizeof(mvp));

	attr.init(layout, vdata, sizeof(layout) / sizeof(layout[0]));
	dirty = 0;

	glGenTextures(1, &tbid);
	glActiveTexture(GL_TEXTURE0 + 15);
	glBindTexture(GL_TEXTURE_BUFFER, tbid);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, cbuf.id);

	color = RGBA(255,255,255,0);
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
	ubuf.use(3);
	attr.use();
	glyphs.use(1);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_BUFFER, tbid);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, cbuf.id);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, count * 6);
	glDisable(GL_BLEND);
}

void TextureFont::measure(const char *s, unsigned *width, unsigned *height) {
	unsigned w = 0;
	unsigned h = 0;
	while (*s) {
		unsigned n = *s++;
		if (n == 0)
			break;
		if ((n < first) || (n > last))
			continue;
		n -= first;
		unsigned ch = info[n].h;
		unsigned cw = info[n].w;
		if (ch > h)
			h = ch;
		w += cw;
		if (*s)
			w += info[n].advance;
	}
	*width = w;
	*height = h;
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
		data[count].id = n * 6;
		data[count].rgba = color & 0xFFFFFF; // strip alpha
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
