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
#include <d3dx10.h>

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

	int start(HINSTANCE hInstance, int nCmdShow);

protected:
	int width;
	int height;
	ID3D10Device *device;
	IDXGISwapChain *swap;
	ID3D10RenderTargetView *target;
	ID3D10Texture2D *depthStencil;
	ID3D10DepthStencilView *depthStencilView;
	ID3D10RasterizerState *rasterizerState;

private:
	int initD3D(void);
	HWND hwnd;
	HINSTANCE hinstance;
};

App *createApp(void);

void printx(const char *fmt, ...);
void printmtx(D3DXMATRIX *m, const char *name);

#endif