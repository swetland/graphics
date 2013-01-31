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

int _save_png(const char *fn, png_byte *data, unsigned w, unsigned h, int ch) {
	png_structp png;
	png_infop info;
	FILE *fp;
	int i, status = -1;

	if ((fp = fopen(fn, "wb")) == NULL)
		goto exit;

	if (!(png = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0)))
		goto close_and_exit;

	if (!(info = png_create_info_struct(png))) {
		png_destroy_write_struct(&png, NULL);
		goto destroy_and_exit;
	}

	if (setjmp(png->jmpbuf)) {
		goto destroy_and_exit;
	}

	png_init_io(png, fp);

	if (ch == 4) {
		png_set_IHDR(png, info, w, h, 8,
			PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	} else if (ch == 1) {
		png_set_IHDR(png, info, w, h, 8,
			PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	} else {
		png_error(png, "unsupported channel count");
	}

	png_write_info(png, info);

	for (i = 0; i < h; i++)
		png_write_row(png, data + (i * w * ch));

	png_write_end(png, info);

	status = 0;

destroy_and_exit:
	png_destroy_write_struct(&png, &info);

close_and_exit:
	fclose(fp);
exit:
	if (status)
		fprintf(stderr,"failed to load '%s'\n", fn);
	return status;
}

int save_png_rgba(const char *fn, void *data, unsigned w, unsigned h) {
	return _save_png(fn, (png_byte*) data, w, h, 4);
}

int save_png_gray(const char *fn, void *data, unsigned w, unsigned h) {
	return _save_png(fn, (png_byte*) data, w, h, 1);
}

