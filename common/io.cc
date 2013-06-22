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
#include <string.h>
#include <sys/stat.h>

#include "util.h"

static char base_path[1024 + 8] = "";

static const char *defaultsearch[] = {
#ifdef _WIN32
	"assets\\",
#else
	"assets/",
	"../common/assets/",
#endif
	NULL,
};

static const char *nosearch[] = {
	"",
	NULL,
};

static const char **search = defaultsearch;

void io_ignore_asset_paths(void) {
	base_path[0] = 0;
	search = nosearch;
}
FILE *fopen_asset(const char *fn, const char *kind) {
	char path[2048 + 64];
	FILE *fp;
	int i;
	if (strlen(fn) > 1024)
		return NULL;
	for (i = 0; search[i]; i++) {
		sprintf(path, "%s%s%s", base_path, search[i], fn);
		fp = fopen(path, "rb");
		if (fp != NULL) {
			printx("Loading %s from '%s'...\n", kind, path);
			return fp;
		}
	}
	printx("Cannot find %s '%s'.\n", kind, fn);
	return NULL;
}

#ifdef _WIN32
#include <windows.h>
void init_io(void) {
	char *x;
	GetModuleFileName(NULL, base_path, 1024);
	base_path[1023] = 0;
	x = strrchr(base_path, '\\');
	if (x)
		x[1] = 0;
	else
		strcpy(base_path,".\\");
}
#else
#include <unistd.h>
void init_io(void) {
	char *x;
	int r;
	r = readlink("/proc/self/exe", base_path, 1024);
	if (r < 0)
		return;
	x = strrchr(base_path, '/');
	if (x)
		x[1] = 0;
	else
		strcpy(base_path,"./");
}
#endif

int file_get_mtime(const char *fn) {
	struct stat s;
	char buf[2048 + 64];
	snprintf(buf, 1024, "%s%s%s", base_path, search[0], fn);
	if (stat(buf, &s))
		return -1;
	return s.st_mtime;
}


void vprintx(const char *fmt, va_list ap) {
	char buf[128];
	vsnprintf(buf, sizeof(buf), fmt, ap);
	buf[127] = 0;
#if defined(_WIN32) && defined(_DEBUG)
	OutputDebugString(buf);
#else
	fwrite(buf, strlen(buf), 1, stderr);
#endif
}

void printx(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vprintx(fmt, ap);
	va_end(ap);
}

void die(const char *fmt, ...) {
	va_list ap;
	printx("ERROR: ");
	va_start(ap, fmt);
	vprintx(fmt, ap);
	va_end(ap);
	printx("\n");
	exit(-1);
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
