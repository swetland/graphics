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
	~TestApp();
	int init(void);
	void render(void);

private:
	float t;
	DWORD timeStart;

	ID3D10Effect *effect;
	ID3D10EffectTechnique *technique;
	ID3D10InputLayout *layout;
	ID3D10Buffer *vtxbuf;
	ID3D10Buffer *idxbuf;

	ID3D10EffectMatrixVariable *uMVP;
	ID3D10EffectShaderResourceVariable *uShader0;

	ID3D10ShaderResourceView *rvShader0;

	mat4 proj;

	struct model *m;
	void *texdata;
	unsigned tw, th;
};

TestApp::TestApp() : App(), t(0.0f), timeStart(0), vtxbuf(NULL), idxbuf(NULL), layout(NULL), effect(NULL) {
}

TestApp::~TestApp() {
	if (rvShader0) rvShader0->Release();
	if (vtxbuf) vtxbuf->Release();
	if (idxbuf) idxbuf->Release();
	if (layout) layout->Release();
	if (effect) effect->Release();
}

int TestApp::init(void) {
	HRESULT hr;

	if (!(texdata = load_png_rgba("cube-texture.png", &tw, &th, 0)))
		return error("cannot load texture");
	if (!(m = load_wavefront_obj("cube.obj")))
		return error("cannot load model");

	hr = D3DX10CreateEffectFromFile("hello.fx", NULL, NULL, "fx_4_0",
		D3D10_SHADER_ENABLE_STRICTNESS | SHADER_DEBUG_FLAGS,
		0, device, NULL, NULL, &effect, NULL, NULL );

	if (FAILED(hr))
		return error("cannot load effect '%s'", "hello.fx");

	technique = effect->GetTechniqueByName("Render");
	uMVP = effect->GetVariableByName("MVP")->AsMatrix();
	uShader0 = effect->GetVariableByName("Texture0")->AsShaderResource();

	D3D10_PASS_DESC pass;
	technique->GetPassByIndex(0)->GetDesc(&pass);
	hr = device->CreateInputLayout(obj_layout, sizeof(obj_layout)/sizeof(obj_layout[0]),
		pass.pIAInputSignature, pass.IAInputSignatureSize, &layout);
	if (FAILED(hr))
		return error("create input layout failed 0x%08x", hr);

	if (createVtxBuffer(m->vdata, 32 * m->vcount, &vtxbuf))
		return -1;
	if (createIdxBuffer(m->idx, sizeof(short) * m->icount, &idxbuf))
		return -1;
	if (createTextureRGBA(texdata, tw, th, 1, &rvShader0))
		return -1;

	uShader0->SetResource(rvShader0);

	proj.setPerspective(D3DX_PI * 0.5, width / (float) height, 0.1f, 100.0f);

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
	D3D10_TECHNIQUE_DESC td;
	technique->GetDesc(&td);

	mat4 world, view, mvp;

	view.identity().rotateX(D2R(25)).translate(0, 0, -10);

	world.identity().rotateY(t);
	mvp = world * view * proj;
	uMVP->SetMatrix((float*) mvp.data());
	for (unsigned p = 0; p < td.Passes; ++p) {
		technique->GetPassByIndex(p)->Apply(0);
		device->DrawIndexed(m->icount, 0, 0 );
	}

	world.identity().rotateY(t).translate(-5,0,0).rotateY(t);
	mvp = world * view * proj;
	uMVP->SetMatrix((float*) mvp.data());
	for (unsigned p = 0; p < td.Passes; ++p) {
		technique->GetPassByIndex(p)->Apply(0);
		device->DrawIndexed(m->icount, 0, 0 );
	}

	swap->Present(0, 0);
}

App *createApp(void) {
	return new TestApp();
}