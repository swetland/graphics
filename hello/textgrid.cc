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

static D3D10_INPUT_ELEMENT_DESC text_layout_desc[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "CHARACTER", 0, DXGI_FORMAT_R8_UINT, 1, 0, D3D10_INPUT_PER_INSTANCE_DATA, 1 },
};

static float unit_box_2d[] = {
	0, 1,  0, 1,
	0, 0,  0, 0,
	1, 0,  1, 0,
	1, 0,  1, 0,
	1, 1,  1, 1,
	0, 1,  0, 1,
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

	ID3D10Blob *blob;
	if (!(data = load_png_rgba("font-vincent-8x8.png", &dw, &dh, 1)))
		return error("cannot load texture");
	if (a->createTextureRGBA(data, dw, dh, 0, &texture))
		return -1;
	free(data);

	if (a->createVtxBuffer(unit_box_2d, sizeof(unit_box_2d), &vtxbuf))
		return -1;
	if (a->createVtxBuffer(grid, width * height, &charbuf))
		return -1;
	if (a->compilePixelShader("TextPS.hlsl", &PS, NULL))
		return -1;
	if (a->compileVertexShader("TextVS.hlsl", &VS, &blob))
		return -1;
	if (a->createConstantBuffer(sizeof(cb), &cbuf))
		return -1;

	cb.proj.setOrtho(0, w, 0, h, -1, 1);	
	cb.cw = width;
	cb.ch = height;
	a->updateBuffer(cbuf, &cb);

	hr = device->CreateInputLayout(text_layout_desc,
		sizeof(text_layout_desc) / sizeof(text_layout_desc[0]),
		blob->GetBufferPointer(), blob->GetBufferSize(), &layout);
	blob->Release();
	if (FAILED(hr))
		return -1;
	stride[0] = 16;
	stride[1] = 1;
	offset[0] = 0;
	offset[1] = 0;
	buffer[0] = vtxbuf;
	buffer[1] = charbuf;
	return 0;
}

void TextGrid::render(App *a, ID3D10Device *device) {
	if (dirty) {
		dirty = 0;
		a->updateBuffer(charbuf, grid);
	}
	device->VSSetShader(VS);
	device->PSSetShader(PS);
	device->VSSetConstantBuffers(0, 1, &cbuf);
	device->PSSetShaderResources(0, 1, &texture);
	device->IASetInputLayout(layout);
	device->IASetVertexBuffers(0, 2, buffer, stride, offset);
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