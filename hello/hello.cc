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

#include "app.h"
#include "matrix.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>

static D3D10_INPUT_ELEMENT_DESC obj_layout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "LOCATION", 0, DXGI_FORMAT_R8G8B8A8_SNORM, 1,  0, D3D10_INPUT_PER_INSTANCE_DATA, 1 },
};

static float locationx[] = {
	0, 0, 0,
	-2, 0, 0,
	-4, 0, 0,
	2, 0, 0,
	4, 0, 0,
};

#define SZ 64
#define SZh (SZ / 2)
#define SZe (SZ * SZ * SZ)
#define SZb (SZe * 4)

static char location[SZb];
static int lcount = 0;

class TestApp : public App {
public:
	TestApp();
	int init(void);
	void render(void);
	void release(void);
	void mouse(int x, int y, int b);
private:
	float t;
	DWORD timeStart;

	ID3D10InputLayout *layout;
	ID3D10Buffer *vtxbuf;
	ID3D10Buffer *idxbuf;
	ID3D10Buffer *ibuf;
	ID3D10Buffer *cbuf;

	ID3D10PixelShader *PS;
	ID3D10VertexShader *VS;
	ID3D10Blob *PSbc;
	ID3D10Blob *VSbc;

	ID3D10ShaderResourceView *rvShader0;

	mat4 proj;
	int lx, ly, lb;
	float rx, ry;
	struct model *m;
};

TestApp::TestApp() : App(), t(0.0f), timeStart(0), lb(0), rx(0), ry(0),
	layout(NULL), vtxbuf(NULL), idxbuf(NULL), cbuf(NULL),
	PS(NULL), VS(NULL), PSbc(NULL), VSbc(NULL), rvShader0(NULL) {
}

void TestApp::release(void) {
	if (PS) PS->Release();
	if (VS) VS->Release();
	if (rvShader0) rvShader0->Release();
	if (layout) layout->Release();
	if (vtxbuf) vtxbuf->Release();
	if (idxbuf) idxbuf->Release();
	if (cbuf) cbuf->Release();
}

int TestApp::init(void) {
	HRESULT hr;
	void *data;
	unsigned dsz, dw, dh;

	if (compilePixelShader("SimplePS.hlsl", &PS, &PSbc))
		return -1;
	if (compileVertexShader("SimpleVS.hlsl", &VS, &VSbc))
		return -1;

	hr = device->CreateInputLayout(obj_layout,
		sizeof(obj_layout) / sizeof(obj_layout[0]),
		VSbc->GetBufferPointer(), VSbc->GetBufferSize(), &layout);
	if (FAILED(hr))
		return error("create input layout failed 0x%08x", hr);

	device->VSSetShader(VS);
	device->PSSetShader(PS);

	float scale = SZh;
	int x, y, z, n = 0;
	for (z = -SZh; z < SZh; z++) {
		for (x = -SZh; x < SZh; x++) {
			for (y = -SZh; y < SZh; y++) {
				float fx = x/scale * 0.50;
				float fy = y/scale * 0.50;
				float fz = z/scale * 0.50;
#if 0
				if (snoise(fx,fy,fz) > 0.1)
					continue;
#else
				float sn =
					snoise(fx,fz) +
					snoise(fx*2.0,fz*2.0) / 4.0 +
					snoise(fx*4.0,fz*4.0) / 8.0;
				if (sn < y/scale)
					continue;
#endif
				location[n+0] = x;
				location[n+1] = y;
				location[n+2] = z;
				location[n+3] = 1;
				n += 4;
			}
		}
	}
	lcount = n / 4;
	printx("Wrote %d locations\n", lcount);
		
	if (!(m = load_wavefront_obj("unitcubeoid.obj")))
		return error("cannot load model");
	printx("Object Loaded. %d vertices, %d indices.\n", m->vcount, m->icount);

	if (createVtxBuffer(m->vdata, 32 * m->vcount, &vtxbuf))
		return -1;
	if (createIdxBuffer(m->idx, sizeof(short) * m->icount, &idxbuf))
		return -1;
	if (createVtxBuffer(location, lcount*4, &ibuf))
		return -1;

#if 0
	if (!(data = load_png_rgba("cube-texture.png", &dw, &dh, 0)))
		return error("cannot load texture");
	if (createTextureRGBA(data, dw, dh, 1, &rvShader0))
		return -1;
	free(data);
#endif
	if (createConstantBuffer(32 * 4, &cbuf))
		return -1;
	device->VSSetConstantBuffers(0, 1, &cbuf);
	device->PSSetShaderResources(0, 1, &rvShader0);

	proj.setPerspective(D2R(90.0), width / (float) height, 0.1f, 250.0f);

	return 0;
}

static float rate = 90.0;

void TestApp::mouse(int x, int y, int b) {
	if (b & lb & 1) {
		float dx = ((float) (x - lx)) / ((float) width);
		float dy = ((float) (y - ly)) / ((float) height);
		ry += dx * rate;
		rx += dy * rate;
		if (rx < 0.0) rx += 360.0;
		else if (rx > 360.0) rx -= 360.0;
		if (ry < 0.0) ry += 360.0;
		else if (ry > 360.0) ry -= 360.0;
	}
	lb = b;
	lx = x;
	ly = y;
}

void TestApp::render(void) {
	static float t = 0.0f;
        static DWORD timeStart = 0;
	DWORD timeCur = GetTickCount();
	if (timeStart == 0)
		timeStart = timeStart;
	t = (timeCur - timeStart) / 1000.0f;

	float rgba[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; 
	device->ClearRenderTargetView(targetView, rgba);
	device->ClearDepthStencilView(depthView, D3D10_CLEAR_DEPTH, 1.0f, 0 );

	UINT stride = 32;
	UINT offset = 0;
	device->IASetInputLayout(layout);
	device->IASetVertexBuffers(0, 1, &vtxbuf, &stride, &offset);
	stride = 4;
	offset = 0;
	device->IASetVertexBuffers(1, 1, &ibuf, &stride, &offset);
	device->IASetIndexBuffer(idxbuf, DXGI_FORMAT_R16_UINT, 0);
	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	struct {
		mat4 mvp;
		mat4 mv;
	} cb0;
	mat4 world, view, tmp;

	view.identity().rotateX(D2R(10)).rotateY(t).translate(0, 0, -10);
	view.identity().translate(0, 0, -SZ);
	world.identity().translate(0.5, 0.5, 0.5).rotateY(D2R(ry)).rotateX(D2R(rx));
	cb0.mvp = world * view * proj;
	cb0.mv = world * view;
	device->UpdateSubresource(cbuf, 0, NULL, &cb0, 0, 0);
	device->DrawIndexedInstanced(m->icount, lcount, 0, 0, 0);

	swapchain->Present(1, 0);
}

App *createApp(void) {
	return new TestApp();
}