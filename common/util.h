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

#ifndef _UTIL_H_
#define _UTIL_H_

/* basic datatypes */
typedef unsigned int u32;
typedef int s32;
typedef unsigned short u16;
typedef short s16;
typedef unsigned char u8;
typedef signed char s8;

/* file io helpers */
#define OPT_PNG_INVERTY 0x0001
#define OPT_PNG_GRAY    0x0002

void *load_png_rgba(const char *fn, unsigned *width, unsigned *height, int options);
void *load_png_gray(const char *fn, unsigned *width, unsigned *height, int options);
void *load_file(const char *fn, unsigned *sz);
int file_get_mtime(const char *fn);

int save_png_rgba(const char *fn, void *data, unsigned width, unsigned height);
int save_png_gray(const char *fn, void *data, unsigned width, unsigned height);

/* tread filenames as normal filenames, don't use asset search process */
void io_ignore_asset_paths(void);

/* model helpers */

struct model {
	float *vdata; /* { vertex[3], normal[3], texcoord[2] } * vcount */
	unsigned short *idx;
	unsigned vcount;
	unsigned icount;
};

struct model *load_wavefront_obj(const char *fn);

/* simplex noise */
float snoise(float x);
float snoise(float x, float y);
float snoise(float x, float y, float z);
float snoise(float x, float y, float z, float w);

#ifdef _WIN32
#define snprintf _snprintf
#endif

void printx(const char *fmt, ...);
void printmtx(float *m, const char *name);
int error(const char *fmt, ...);
void die(const char *fmt, ...);

FILE *fopen_asset(const char *fn, const char *kind);
#endif

