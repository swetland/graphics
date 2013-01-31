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

struct CubeVertex {
	D3DXVECTOR3 Pos;
	D3DXVECTOR4 Color;
};

static D3D10_INPUT_ELEMENT_DESC cube_layout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
};

#if 0 // original
static CubeVertex cube_vertices[] = {
	{ D3DXVECTOR3( -1.0f, 1.0f, -1.0f ), D3DXVECTOR4( 0.0f, 0.0f, 1.0f, 1.0f ) },
	{ D3DXVECTOR3( 1.0f, 1.0f, -1.0f ), D3DXVECTOR4( 0.0f, 1.0f, 0.0f, 1.0f ) },
	{ D3DXVECTOR3( 1.0f, 1.0f, 1.0f ), D3DXVECTOR4( 0.0f, 1.0f, 1.0f, 1.0f ) },
	{ D3DXVECTOR3( -1.0f, 1.0f, 1.0f ), D3DXVECTOR4( 1.0f, 0.0f, 0.0f, 1.0f ) },
	{ D3DXVECTOR3( -1.0f, -1.0f, -1.0f ), D3DXVECTOR4( 1.0f, 0.0f, 1.0f, 1.0f ) },
	{ D3DXVECTOR3( 1.0f, -1.0f, -1.0f ), D3DXVECTOR4( 1.0f, 1.0f, 0.0f, 1.0f ) },
	{ D3DXVECTOR3( 1.0f, -1.0f, 1.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
	{ D3DXVECTOR3( -1.0f, -1.0f, 1.0f ), D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f ) },
};
#else // flip z
static CubeVertex cube_vertices[] = {
	{ D3DXVECTOR3( -1.0f, 1.0f, 1.0f ), D3DXVECTOR4( 0.0f, 0.0f, 1.0f, 1.0f ) },
	{ D3DXVECTOR3( 1.0f, 1.0f, 1.0f ), D3DXVECTOR4( 0.0f, 1.0f, 0.0f, 1.0f ) },
	{ D3DXVECTOR3( 1.0f, 1.0f, -1.0f ), D3DXVECTOR4( 0.0f, 1.0f, 1.0f, 1.0f ) },
	{ D3DXVECTOR3( -1.0f, 1.0f, -1.0f ), D3DXVECTOR4( 1.0f, 0.0f, 0.0f, 1.0f ) },
	{ D3DXVECTOR3( -1.0f, -1.0f, 1.0f ), D3DXVECTOR4( 1.0f, 0.0f, 1.0f, 1.0f ) },
	{ D3DXVECTOR3( 1.0f, -1.0f, 1.0f ), D3DXVECTOR4( 1.0f, 1.0f, 0.0f, 1.0f ) },
	{ D3DXVECTOR3( 1.0f, -1.0f, -1.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
	{ D3DXVECTOR3( -1.0f, -1.0f, -1.0f ), D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f ) },
};
#endif

#if 0
static DWORD cube_indices[] = { // CW, but CCW on Z faces if RH
	3,1,0, 2,1,3,
	0,5,4, 1,5,0,
	3,4,7, 0,4,3,
	1,6,5, 2,6,1,
	2,7,6, 3,7,2,
	6,4,5, 7,4,6,
};
#else
static DWORD cube_indices[] = { // CCW
	0,1,3, 3,1,2,
	4,5,0, 0,5,1,
	7,4,3, 3,4,0,
	5,6,1, 1,6,2,
	6,7,2, 2,7,3,
	5,4,6, 6,4,7,
};
#endif

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
//	ID3D10EffectMatrixVariable *uView;
//	ID3D10EffectMatrixVariable *uProjection;

	D3DXMATRIX world;
	D3DXMATRIX view;
	D3DXMATRIX projection;

	mat4 proj;
	mat4 mvp;
};

TestApp::TestApp() : App(), t(0.0f), timeStart(0), vtxbuf(NULL), idxbuf(NULL), layout(NULL), effect(NULL) {
}

TestApp::~TestApp() {
	if (vtxbuf) vtxbuf->Release();
	if (idxbuf) idxbuf->Release();
	if (layout) layout->Release();
	if (effect) effect->Release();
}

int TestApp::init(void) {
	HRESULT hr;

	hr = D3DX10CreateEffectFromFile("hello.fx", NULL, NULL, "fx_4_0",
		D3D10_SHADER_ENABLE_STRICTNESS | SHADER_DEBUG_FLAGS,
		0, device, NULL, NULL, &effect, NULL, NULL );

	if (FAILED(hr)) {
		MessageBox(NULL, "Cannot open hello.fx", "Error", MB_OK);
		return -1;
	}

	technique = effect->GetTechniqueByName("Render");
//	uWorld = effect->GetVariableByName("World")->AsMatrix();
//	uView = effect->GetVariableByName("View")->AsMatrix();
	uMVP = effect->GetVariableByName("MVP")->AsMatrix();

	D3D10_PASS_DESC pass;
	technique->GetPassByIndex(0)->GetDesc(&pass);
	hr = device->CreateInputLayout(cube_layout, sizeof(cube_layout)/sizeof(cube_layout[0]),
		pass.pIAInputSignature, pass.IAInputSignatureSize, &layout);
	if (FAILED(hr))
		return -1;

	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CubeVertex) * sizeof(cube_vertices)/sizeof(cube_vertices[0]); //XXX
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = cube_vertices;
	hr = device->CreateBuffer(&bd, &InitData, &vtxbuf);
	if (FAILED(hr))
		return -1;

	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(DWORD) * sizeof(cube_indices) / sizeof(cube_indices[0]);
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	InitData.pSysMem = cube_indices;
	hr = device->CreateBuffer(&bd, &InitData, &idxbuf);
	if (FAILED(hr))
		return -1;

	D3DXMatrixIdentity(&world);

	D3DXVECTOR3 eye(0.0f, 0.0f, 5.0f);
	D3DXVECTOR3 at(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&view, &eye, &at, &up);

	D3DXMatrixIdentity(&view);
	D3DXMatrixTranslation(&view, 0, 0, 10);

	D3DXMatrixPerspectiveFovRH(&projection, (float) D3DX_PI * 0.5f, width / (float) height, 0.1f, 100.0f);
//	printmtx(&projection, "proj1");
	mat4 tmp;
	tmp.setPerspective(D3DX_PI * 0.5, width / (float) height, 0.1f, 100.0f);
	memcpy(&projection,&tmp,sizeof(float[16]));
//	printmtx(&projection, "proj2");
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
#if 0
	printmtx(&world,"world");
	printmtx(&view,"view");
	printmtx(&projection,"projection");
#endif


//	D3DXMATRIX xlat, MVP, rotn, tmp;

	UINT stride = sizeof(CubeVertex);
	UINT offset = 0;
	device->IASetInputLayout(layout);
	device->IASetVertexBuffers(0, 1, &vtxbuf, &stride, &offset);
	device->IASetIndexBuffer(idxbuf, DXGI_FORMAT_R32_UINT, 0);
	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D10_TECHNIQUE_DESC td;
	technique->GetDesc(&td);

	mat4 world, view, proj, tmp, mvp;

	proj.setPerspective(M_PI * 0.5, 1024.0/768.0, 1.0, 100.0);
	view.identity().rotateX(D2R(25)).translate(0, 0, -10);
	world.identity().rotateY(t);
	mvp = world * view * proj;
	uMVP->SetMatrix((float*) mvp.data());
	for (unsigned p = 0; p < td.Passes; ++p) {
		technique->GetPassByIndex(p)->Apply(0);
		device->DrawIndexed(36, 0, 0 );
	}

	world.identity().rotateY(t).translate(-5,0,0).rotateY(t);
	mvp = world * view * proj;
	uMVP->SetMatrix((float*) mvp.data());
	for (unsigned p = 0; p < td.Passes; ++p) {
		technique->GetPassByIndex(p)->Apply(0);
		device->DrawIndexed(36, 0, 0 );
	}

#if 0
	D3DXMatrixIdentity(&view);
//	D3DXMatrixRotationX(&tmp,D2R(-25));
//	D3DXMatrixMultiply(&view,&view,&tmp);
	D3DXMatrixTranslation(&tmp, 0, 0, -10);
	D3DXMatrixMultiply(&view,&view,&tmp);

	D3D10_TECHNIQUE_DESC td;
	technique->GetDesc(&td);
	D3DXMatrixRotationY(&world, t);
	MVP = world * view * projection;
	uMVP->SetMatrix((float*) &MVP);
	//uMVP->SetMatrixTranspose((float*) &MVP);
	for (unsigned p = 0; p < td.Passes; ++p) {
		technique->GetPassByIndex(p)->Apply(0);
		device->DrawIndexed(36, 0, 0 );
	}

	D3DXMatrixRotationY(&world, t);
	D3DXMatrixTranslation(&xlat, -5.0, 0.0, 0.0);
	D3DXMatrixRotationY(&rotn, t);
	D3DXMatrixMultiply(&world, &world, &xlat);
	//printmtx(&world, "v1");
	D3DXMatrixMultiply(&world, &world, &rotn);
	MVP = world * view * projection;
	//printmtx(&MVP, "mvp1");
	uMVP->SetMatrix((float*) &MVP);
	//uMVP->SetMatrixTranspose((float*) &MVP);
#if 0
	mat4 XMVP, xworld, xxlat, xrotn, xview, xproj;
	memcpy(&xview, &view, sizeof(float[16]));
	memcpy(&xproj, &projection, sizeof(float[16]));
	memcpy(&xworld, &rotn, sizeof(float[16]));
	memcpy(&xxlat, &xlat, sizeof(float[16]));
	memcpy(&xrotn, &rotn, sizeof(float[16]));
	xworld.mul(xworld, xxlat);
	memcpy(&world, &xworld, sizeof(float[16]));
	//printmtx(&world, "v2");
	xworld.mul(xworld, xrotn);
	XMVP = xworld * xview * xproj;
	memcpy(&MVP, &XMVP, sizeof(float[16]));
	//printmtx(&MVP, "mvp2");
	//uMVP->SetMatrixTranspose((float*) &MVP);
	uMVP->SetMatrix((float*)XMVP.data());
#endif
	for (unsigned p = 0; p < td.Passes; ++p) {
		technique->GetPassByIndex(p)->Apply(0);
		device->DrawIndexed(36, 0, 0 );
	}
#endif
	swap->Present(0, 0);
}

App *createApp(void) {
	return new TestApp();
}