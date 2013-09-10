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

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_BITMAP_H
#include FT_GLYPH_H

#include <util.h>
#include <io.h>

#include <texturefont.h>

#include <algorithm>

/* from internal header... */
#define FT_PIX_FLOOR(x) ((x) & ~63)
#define FT_PIX_CEIL(x) FT_PIX_FLOOR((x) + 63)

struct Glyph {
	unsigned c;
	unsigned h;
};

void get_bbox(FT_Outline *outline, FT_BBox *bb) {
	FT_Outline_Get_CBox(outline, bb);
	/* grid fit */
	bb->xMin = FT_PIX_FLOOR(bb->xMin);
	bb->yMin = FT_PIX_FLOOR(bb->yMin);
	bb->xMax = FT_PIX_CEIL(bb->xMax);
	bb->yMax = FT_PIX_CEIL(bb->yMax);
	/* convert from 26.6 to pixel coords */
	bb->xMin >>= 6;
	bb->yMin >>= 6;
	bb->xMax >>= 6;
	bb->yMax >>= 6;
}

bool compare_glyphs(Glyph const &left, Glyph const &right) {
	return left.h < right.h;
}

#define FIRST 0x20
#define LAST 0x7F
#define MAXDIM 1024

int main(int argc, char **argv) {
	FT_Library ftl;
	FT_Face face;
	FT_Bitmap *bitmap;
	unsigned i, n;
	void *x;
	
	FT_BBox bb;
	FT_Bitmap bm;
	unsigned char data[MAXDIM * MAXDIM];
	int w, h, tx, ty, nx, ny, rh, max;
	CharInfo info[128];
	Glyph list[LAST];
	char tmp[256];

	io_ignore_asset_paths();

	if (argc < 4)
		return error("usage: mkfont <truetypefont> <height> <outputname>");

	if (FT_Init_FreeType(&ftl))
		return -1;

	x = load_file(argv[1], &n);
	if (x == 0)
		return -1;

	if (FT_New_Memory_Face(ftl, (FT_Byte*) x, n, 0, &face))
		return error("cannot load font '%s'", argv[1]);

	if (FT_Set_Pixel_Sizes(face, 0, atoi(argv[2])))
		return error("cannot set font size to %s\n", argv[2]);

	for (n = FIRST; n < LAST; n++) {
		if (FT_Load_Char(face, n, FT_LOAD_NO_BITMAP)) {
			error("missing glyph for %d\n", n);
			continue;
		}
		get_bbox(&face->glyph->outline, &bb);
		list[n - FIRST].c = n;
		list[n - FIRST].h = bb.yMax - bb.yMin;
		
	}
	std::sort(list, list + LAST - FIRST, compare_glyphs);

	max = 128;
retry:
	memset(data, 0, sizeof(data));
	memset(info, 0, sizeof(info));

	bm.rows = max;
	bm.width = max;
	bm.pitch = max;
	bm.buffer = data;
	bm.num_grays = 256;
	bm.pixel_mode = FT_PIXEL_MODE_GRAY;
	bm.palette_mode = 0;
	bm.palette = NULL;

	nx = ny = 1;
	rh = 0;

	printf("%dx%d texture...\n", max, max);
	for (i = 0; i < (LAST - FIRST); i++) {
		n = list[i].c;
		if (FT_Load_Char(face, n, FT_LOAD_NO_BITMAP)) {
			error("missing glyph for %d\n", n);
			continue;
		}
		get_bbox(&face->glyph->outline, &bb);
		w = bb.xMax - bb.xMin + 1;
		h = bb.yMax - bb.yMin + 1;
		tx = -bb.xMin;
		ty = -bb.yMin;
		if ((nx + w) >= max) {
			nx = 1;
			ny += rh + 1;
			rh = 0;
		}
		if ((ny + h) >= max) {
			max *= 2;
			if (max > MAXDIM) {
				fprintf(stderr,"oops! out of space\n");
				return -1;
			}
			goto retry;
		}
		info[n].x = nx;
		info[n].y = max - ny - h + 1;
		info[n].w = w - 1;
		info[n].h = h - 1;
		info[n].dx = bb.xMin;
		info[n].dy = bb.yMin;
		info[n].advance = face->glyph->advance.x >> 6;
		FT_Outline_Translate(&face->glyph->outline, nx * 64 + tx * 64, ny * 64 + ty * 64);
		FT_Outline_Get_Bitmap(ftl, &face->glyph->outline, &bm);
		nx += w;
		if (h > rh)
			rh = h;
	}
	bitmap = &bm;

	sprintf(tmp, "%s.png", argv[3]);
	save_png_gray(tmp, bitmap->buffer, bitmap->width, bitmap->rows);
	FontInfo header;
	header.magic = TEXTUREFONT_MAGIC;
	header.first = FIRST;
	header.count = LAST - FIRST;
	header.unused = 0;

	sprintf(tmp, "%s.dat", argv[3]);
	FILE *fp = fopen(tmp, "wb");
	if (!fp)
		return error("cannot write font data");
	if (fwrite(&header, sizeof(header), 1, fp) != 1)
		return error("failed to write font header");
	if (fwrite(info + header.first, sizeof(FontInfo), header.count, fp) != header.count)
		return error("failed to write character info");
	fclose(fp);
	return 0;
}
