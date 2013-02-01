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
};

class TestApp : public App {
public:
	TestApp();
	int init(void);
	void render(void);
	void release(void);

private:
	float t;
	DWORD timeStart;

	ID3D10InputLayout *layout;
	ID3D10Buffer *vtxbuf;
	ID3D10Buffer *idxbuf;
	ID3D10Buffer *cbuf;

	ID3D10PixelShader *PS;
	ID3D10VertexShader *VS;
	ID3D10Blob *PSbc;
	ID3D10Blob *VSbc;

	ID3D10ShaderResourceView *rvShader0;

	mat4 proj;

	struct model *m;
};

TestApp::TestApp() : App(), t(0.0f), timeStart(0),
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

	if (!(m = load_wavefront_obj("cube.obj")))
		return error("cannot load model");
	if (createVtxBuffer(m->vdata, 32 * m->vcount, &vtxbuf))
		return -1;
	if (createIdxBuffer(m->idx, sizeof(short) * m->icount, &idxbuf))
		return -1;

	if (!(data = load_png_rgba("cube-texture.png", &dw, &dh, 0)))
		return error("cannot load texture");
	if (createTextureRGBA(data, dw, dh, 1, &rvShader0))
		return -1;
	free(data);

	if (createConstantBuffer(16 * 4, &cbuf))
		return -1;
	device->VSSetConstantBuffers(0, 1, &cbuf);
	device->PSSetShaderResources(0, 1, &rvShader0);

	proj.setPerspective(M_PI * 0.5, width / (float) height, 0.1f, 100.0f);

	return 0;
}

void TestApp::render(void) {
	static float t = 0.0f;
        static DWORD timeStart = 0;
	DWORD timeCur = GetTickCount();
	if (timeStart == 0)
		timeStart = timeStart;
	t = (timeCur - timeStart) / 1000.0f;

	float rgba[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; 
	device->ClearRenderTargetView(target, rgba);
	device->ClearDepthStencilView(depthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0 );

	UINT stride = 32;
	UINT offset = 0;
	device->IASetInputLayout(layout);
	device->IASetVertexBuffers(0, 1, &vtxbuf, &stride, &offset);
	device->IASetIndexBuffer(idxbuf, DXGI_FORMAT_R16_UINT, 0);
	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mat4 world, view, mvp, tmp;

	view.identity().rotateX(D2R(25)).translate(0, 0, -10);
	world.identity().rotateY(t);
	mvp = world * view * proj;
	device->UpdateSubresource(cbuf, 0, NULL, mvp.data(), 0, 0);
	device->DrawIndexed(m->icount, 0, 0);

	world.identity().rotateY(t).translate(-5,0,0).rotateY(t);
	mvp = world * view * proj;
	device->UpdateSubresource(cbuf, 0, NULL, mvp.data(), 0, 0);
	device->DrawIndexed(m->icount, 0, 0);

	swap->Present(0, 0);
}

App *createApp(void) {
	return new TestApp();
}