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

#ifndef _DX_APP_H_
#define _DX_APP_H_

#include <windows.h>

/* TODO: figure out wtf is up with dxgitype.h macro redefinitions... */
#pragma warning ( disable : 4005 )

#include <d3d10.h>
#include <dinput.h>

#ifdef _DEBUG
#define DEVICE_DEBUG_FLAGS D3D10_CREATE_DEVICE_DEBUG
#define SHADER_DEBUG_FLAGS D3D10_SHADER_DEBUG
#else
#define DEVICE_DEBUG_FLAGS 0
#define SHADER_DEBUG_FLAGS 0
#endif

class App {
public:
	App();
	virtual ~App();

	virtual int init(void) = 0;
	virtual void render(void) = 0;
	virtual void release(void) {};
	virtual void mouse(int x, int y, int buttons) {};

	/* glue - do not use */
	int start(HINSTANCE hInstance, int nCmdShow);
	void eventloop(void);
	void stop(void);
	int reconfigure(int init);
	void setActive(int a) { active = a; };
protected:
	int width;
	int height;

	ID3D10Device *device;
	IDXGISwapChain *swapchain;
	ID3D10RenderTargetView *targetView;
	ID3D10Texture2D *depthBuffer;
	ID3D10DepthStencilView *depthView;
	ID3D10RasterizerState *rasterizerState;

	int createTextureRGBA(void *data, int tw, int th,
		int genmips, ID3D10ShaderResourceView **srv);
	int createBuffer(D3D10_BIND_FLAG flag,
		void *data, int sz, ID3D10Buffer **buf);
	int createVtxBuffer(void *data, int sz, ID3D10Buffer **buf) {
		return createBuffer(D3D10_BIND_VERTEX_BUFFER, data, sz, buf);
	}
	int createIdxBuffer(void *data, int sz, ID3D10Buffer **buf) {
		return createBuffer(D3D10_BIND_INDEX_BUFFER, data, sz, buf);
	}
	int createConstantBuffer(int sz, ID3D10Buffer **buf) {
		return createBuffer(D3D10_BIND_CONSTANT_BUFFER, NULL, sz, buf);
	}
	int compileVertexShader(const char *fn, ID3D10VertexShader **vs, ID3D10Blob **data);
	int compilePixelShader(const char *fn, ID3D10PixelShader **ps, ID3D10Blob **data);

private:
	LPDIRECTINPUT8 dinput;
	LPDIRECTINPUTDEVICE8 dkeyboard;
	unsigned char keystate[256];
	int initD3D(void);
	int initDirectInput(void);
	HWND hwnd;
	HINSTANCE hinstance;
	int active;
};

int compileShader(const char *fn, const char *profile, ID3D10Blob **shader);

App *createApp(void);

void printx(const char *fmt, ...);
void printmtx(float *m, const char *name);
int error(const char *fmt, ...);

#endif