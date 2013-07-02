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

#include "app.h"
#include "util.h"

static void dump_compile_error(unsigned id) {
	int len;
	char *buf;
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
	buf = (char*) malloc(len + 1);
	if (buf != 0) {
		memset(buf, 0, len);
		glGetShaderInfoLog(id, len, &len, buf);
		buf[len] = 0;
		fprintf(stderr,"-- shader compiler error --\n%s\n", buf);
		free(buf);
	}
}

static void dump_link_error(unsigned id) {
	int len;
	char *buf;
	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &len);
	buf = (char*) malloc(len + 1);
	if (buf != 0) {
		memset(buf, 0, len);
		glGetProgramInfoLog(id, len, &len, buf);
		buf[len] = 0;
		fprintf(stderr,"-- shader link error --\n%s\n", buf);
		free(buf);
	}
}

int Program::link(VertexShader *vs, PixelShader *ps) {
	return link(vs, NULL, ps);
}

int Program::link(VertexShader *vs, GeometryShader *gs, PixelShader *ps) {
	unsigned n;
	int r;
	n = glCreateProgram();
	glAttachShader(n, vs->id);
	if (gs)
		glAttachShader(n, gs->id);
	glAttachShader(n, ps->id);
	glLinkProgram(n);
	glGetProgramiv(n, GL_LINK_STATUS, &r);
	if (!r) {
		dump_link_error(n);
		glDeleteProgram(n);
		return error("shader program link error");
	}
	if (id)
		glDeleteProgram(id);
	id = n;
	return 0;
}

int Program::load(const char *vsfn, const char *gsfn, const char *psfn) {
	VertexShader vs;
	GeometryShader gs;
	PixelShader ps;
	if (vs.load(vsfn))
		return -1;
	if (gs.load(gsfn))
		return -1;
	if (ps.load(psfn))
		return -1;
	return link(&vs, &gs, &ps);
}

int Program::load(const char *vsfn, const char *psfn) {
	VertexShader vs;
	PixelShader ps;
	if (vs.load(vsfn))
		return -1;
	if (ps.load(psfn))
		return -1;
	return link(&vs, &ps);
}

struct section {
	section *next;
	unsigned lineno;
	const char *name;
	const char *str;
	unsigned len;
};

struct source {
	source *next;
	const char *name;
	section *sections;
	section common;
	const char *str;
	unsigned len;
};

static source *source_cache = NULL;

static struct source *load_shader_source(const char *fn) {
	source *src;
	section *part;
	unsigned lineno = 1;
	unsigned len;
	char *x, *end, *tmp;
	char buf[1024];

	x = (char*) strchr(fn, '.');
	if (x) {
		memcpy(buf, fn, x-fn);
		strcpy(buf + (x - fn), ".glsl");
	} else {
		sprintf(buf, "%s.glsl", fn);
	}

	for (src = source_cache; src; src = src->next)
		if (!strcmp(buf, src->name))
			return src;

	src = new source;
	src->sections = NULL;
	part = &src->common;
	part->name = "$common$";

	x = (char*) load_file(buf, &len);
	if (!x) {
		delete src;
		return NULL;
	}
	src->str = x;
	src->len = len;

	part->str = x;
	part->lineno = lineno;

	end = x + len;
	while (x < end) {
		if (*x++ != '\n')
			continue;
		lineno++;
		if ((end - x) < 3)
			continue;
		if ((x[0] != '-') || (x[1] != '-'))
			continue;
		part->len = x - part->str;
		part->next = src->sections;
		src->sections = part;
		*x = 0;
		x += 2;

		part = new section;
		part->next = NULL;
		part->name = "";

		tmp = x;
		while (x < end) {
			if (*x == '\n') {
				*x = 0;
				while ((tmp < x) && (*tmp <= ' '))
					tmp++;
				part->name = tmp;
				while (tmp < x) {
					if (*tmp <= ' ')
						*tmp = 0;
					tmp++;
				}
				x++;
				break;
			}
			x++;
		}

		lineno++;
		part->str = x;
		part->lineno = lineno;
	}

	part->len = x - part->str;
	part->next = src->sections;
	src->sections = part;

#if 0
	printx("[ source '%s' %d lines ]\n", buf, lineno);
	for (part = src->sections; part; part = part->next)
		printx("  [ section '%s' %d bytes @ %d]\n",
			part->name, part->len, part->lineno);
#endif

	src->name = strdup(buf);
	src->next = source_cache;
	source_cache = src;
	return src;
}

static int compile_shader_source(source *src, const char *name, unsigned id) {
	char misc[128];
	const char *data[3];
	int size[3];
	section *part;

	for (part = src->sections; part; part = part->next) {
		if (!strcmp(name, part->name)) {
			printx("Loaded shader section '%s' from '%s'\n",
				part->name, src->name);
			sprintf(misc, "#line %d\n", part->lineno - 1);
			data[0] = src->common.str;
			size[0] = src->common.len;
			data[1] = misc;
			size[1] = strlen(misc);
			data[2] = part->str;
			size[2] = part->len;
#if 0
			printf("----------------\n%s%s%s\n-------------\n",
				data[0], data[1], data[2]);
#endif
			glShaderSource(id, 3, data, size);
			return 0;
		}
	}
	return error("cannot find section '%s'", name);
}

static int _load_shader(unsigned *out, const char *fn, unsigned type) {
	unsigned id;
	source *src;
	const char *x;
	int r;

	x = strchr(fn, '.');
	if (x == 0)
		return error("shader source '%s' has no section", fn);
	x++;

	src = load_shader_source(fn);
	if (src == NULL)
		return error("cannot load shader source '%s'", fn);

	id = glCreateShader(type);
	if (compile_shader_source(src, x, id)) {
		glDeleteShader(id);
		return -1;
	}
	glCompileShader(id);
	glGetShaderiv(id, GL_COMPILE_STATUS, &r);
	if (!r) {
		dump_compile_error(id);
		glDeleteShader(id);
		return error("shader '%s' compile error", fn);
	}
	if (*out)
		glDeleteShader(*out);
	*out = id;
	return 0;
}

int VertexShader::load(const char *fn) {
	return _load_shader(&id, fn, GL_VERTEX_SHADER);
}

int PixelShader::load(const char *fn) {
	return _load_shader(&id, fn, GL_FRAGMENT_SHADER);
}

int GeometryShader::load(const char *fn) {
	return _load_shader(&id, fn, GL_GEOMETRY_SHADER);
}

