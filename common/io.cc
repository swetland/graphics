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
#include <stdarg.h>
#include <string.h>

#include "util.h"

extern const char *load_file_base_path;

static char base_path[1024];

#ifdef _WIN32
#include <windows.h>
void init_io(void) {
	char *x;
	GetModuleFileName(NULL, base_path, 1024);
	base_path[1023] = 0;
	x = strrchr(base_path, '\\');
	if (x) {
		x[1] = 0;
		strcat(base_path,"assets\\");
		load_file_base_path = base_path;
	}
}
#else
void init_io(void) {
}
#endif

void die(const char *fmt, ...) {
	printx("ERROR: %s\n", fmt);
	exit(-1);
}

void printx(const char *fmt, ...) {
	char buf[128];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	buf[127] = 0;
	va_end(ap);
#if defined(_WIN32) && defined(_DEBUG)
	OutputDebugString(buf);
#else
	fwrite(buf, strlen(buf), 1, stderr);
#endif
}

void printmtx(float *m, const char *name) {
	printx("| %8.4f %8.4f %8.4f %8.4f | \"%s\"\n", m[0], m[1], m[2], m[3], name);
	printx("| %8.4f %8.4f %8.4f %8.4f |\n", m[4], m[5], m[6], m[7]);
	printx("| %8.4f %8.4f %8.4f %8.4f |\n", m[8], m[9], m[10], m[11]);
	printx("| %8.4f %8.4f %8.4f %8.4f |\n", m[12], m[13], m[14], m[15]);
}

int error(const char *fmt, ...) {
	char buf[128];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	buf[127] = 0;
#if defined(_WIN32) && !defined(_DEBUG)
	MessageBox(NULL, buf, "Error", MB_OK);
#else
	printx("ERROR: %s\n", buf);
#endif
	return -1;
}
