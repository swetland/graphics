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
#include <string.h>

#include <png.h>

#include "util.h"

void *_load_png(const char *fn, unsigned *_width, unsigned *_height, int ch, int inverty) {
	png_structp png;
	png_infop info;
	png_uint_32 w, h;
	int depth, ctype, itype, i;
	png_byte *data = 0;
	FILE *fp;

	if ((fp = fopen(fn, "rb")) == NULL)
		goto exit;

	if (!(png = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0)))
		goto close_and_exit;

	if (!(info = png_create_info_struct(png))) {
		png_destroy_read_struct(&png, NULL, NULL);
		goto destroy_and_exit;
	}

	if (setjmp(png->jmpbuf)) {
		if (data) {
			free(data);
			data = 0;
		}
		goto destroy_and_exit;
	}

	png_init_io(png, fp);

	png_read_info(png, info);

	png_get_IHDR(png, info, &w, &h, &depth, &ctype, &itype, NULL, NULL);

	if (depth < 8)
		png_set_packing(png);

	if (depth > 8)
		png_set_strip_16(png);

	if (ctype == PNG_COLOR_TYPE_PALETTE)
		png_set_expand(png);

	if ((ctype == PNG_COLOR_TYPE_GRAY) && (depth < 8))
		png_set_expand(png);

	if (png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_expand(png);

	if (ch == 4) {
		if ((ctype == PNG_COLOR_TYPE_RGB) ||
			(ctype == PNG_COLOR_TYPE_GRAY))
			png_set_add_alpha(png, 0xFF, PNG_FILLER_AFTER);

		if ((ctype == PNG_COLOR_TYPE_GRAY) ||
			(ctype == PNG_COLOR_TYPE_GRAY_ALPHA))
			png_set_gray_to_rgb(png);
	} else if (ch == 1) {
		if ((ctype == PNG_COLOR_TYPE_RGB) ||
			(ctype == PNG_COLOR_TYPE_RGB_ALPHA))
			png_set_rgb_to_gray_fixed(png, 1, -1, -1);

		png_set_strip_alpha(png);
	} else {
		png_error(png, "unsupported channel count");
	}

	if (!(data = (png_byte*) malloc(w * h * ch)))
		png_error(png, "cannot allocate image buffer");

	if (inverty)
		for (i = h-1; i >= 0; i--)
			png_read_row(png, data + (i * w * ch), NULL);
	else
		for (i = 0; i < h; i++)
			png_read_row(png, data + (i * w * ch), NULL);

	*_width = w;
	*_height = h;

destroy_and_exit:
	png_destroy_read_struct(&png, &info, NULL);

close_and_exit:
	fclose(fp);
exit:
	if (!data)
		fprintf(stderr,"failed to load '%s'\n", fn);
	return data;
}

void *load_png_rgba(const char *fn, unsigned *_width, unsigned *_height, int texture) {
	return _load_png(fn, _width, _height, 4, texture);
}

void *load_png_gray(const char *fn, unsigned *_width, unsigned *_height, int texture) {
	return _load_png(fn, _width, _height, 1, texture);
}

