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

#define BUILTIN 1

#if BUILTIN
#include "TextVS.h"
#include "TextPS.h"
#endif

static D3D10_INPUT_ELEMENT_DESC text_layout_desc[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "CHARACTER", 0, DXGI_FORMAT_R8_UINT, 1, 0, D3D10_INPUT_PER_INSTANCE_DATA, 1 },
};

static float unit_box_2d[] = {
	0, 1,  0, 0,
	0, 0,  0, 1,
	1, 0,  1, 1,
	1, 0,  1, 1,
	1, 1,  1, 0,
	0, 1,  0, 0,
};

int TextGrid::init(App *a, ID3D10Device *device, int w, int h) {
	void *data;
	unsigned int dw, dh;
	HRESULT hr;

	struct {
		mat4 proj;
		unsigned cw;
		unsigned ch;
		unsigned pad0;
		unsigned pad1;
	} cb;

	width = w;
	height = h;
	dirty = 0;

	grid = (unsigned char*) malloc(w * h);
	if (!grid)
		return -1;
	clear();

	if (a->loadTextureRGBA(&texture, "font-vincent-8x8.png", 0))
		return -1;
	if (a->initBuffer(&vtx, unit_box_2d, sizeof(unit_box_2d)))
		return -1;
	if (a->initBuffer(&cbuf, grid, width * height))
		return -1;
	if (a->initBuffer(&ubuf, NULL, sizeof(cb)))
		return -1;
#if BUILTIN
	if (a->compileShader(&ps, "TextPS.hlsl",
		(void*) codeTextPS, sizeof(codeTextPS), 1))
		return -1;
	if (a->compileShader(&vs, "TextVS.hlsl",
		(void*) codeTextVS, sizeof(codeTextVS), 1,
		text_layout_desc, sizeof(text_layout_desc)/sizeof(text_layout_desc[0])))
		return -1;
#else
	if (a->loadShader(&ps, "../common/TextPS.hlsl"))
		return -1;
	if (a->loadShader(&vs, "../common/TextVS.hlsl",
		text_layout_desc, sizeof(text_layout_desc)/sizeof(text_layout_desc[0])))
		return -1;
#endif

	cb.proj.setOrtho(0, w, 0, h, -1, 1);	
	cb.cw = width;
	cb.ch = height;
	a->updateBuffer(&ubuf, &cb);

	return 0;
}

void TextGrid::render(App *a, ID3D10Device *device) {
	if (dirty) {
		dirty = 0;
		a->updateBuffer(&cbuf, grid);
	}
	a->useShaders(&ps, &vs);
	a->useBuffer(&ubuf, 0);
	a->useTexture(&texture, 0);
	a->useBuffer(&vtx, 0, 16, 0);
	a->useBuffer(&cbuf, 1, 1, 0);
	device->DrawInstanced(6, width * height, 0, 0);
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