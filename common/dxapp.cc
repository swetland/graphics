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
#include <stdarg.h>

#include "app.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

App::App() : width(800), height(600), device(NULL), target(NULL), swap(NULL) {
}

App::~App() {
	if (device) device->ClearState();
	if (rasterizerState) rasterizerState->Release();
	if (depthStencilView) depthStencilView->Release();
	if (depthStencil) depthStencil->Release();
	if (target) target->Release();
	if (swap) swap->Release();
	if (device) device->Release();
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hdc;
	switch( message ) {
	case WM_PAINT:
		hdc = BeginPaint( hWnd, &ps );
		EndPaint( hWnd, &ps );
		break;
	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	App *app = createApp();

	if (app->start(hInstance, nCmdShow))
		return 0;

	MSG msg = {0};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			app->render();
		}
	}

	delete app;
	return (int) msg.wParam;
}

int App::start(HINSTANCE hInstance, int nCmdShow) {
	hinstance = hInstance;

	WNDCLASSEX wc;
	memset(&wc, 0, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszClassName = "MyWindowClass";

	if (!RegisterClassEx(&wc))
		return -1;

	RECT rc = { 0, 0, width, height };

	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);
	hwnd = CreateWindow("MyWindowCLass", "Hello", WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, width, height,
			NULL, NULL, hInstance, NULL);
	if (!hwnd)
		return -1;

	ShowWindow(hwnd, nCmdShow);

	if (FAILED(initD3D()))
		return -1;
	if (init())
		return -1;
	return 0;
}

int App::initD3D(void) {
	HRESULT hr;
	RECT rc;
	GetClientRect(hwnd, &rc);

	unsigned width = rc.right - rc.left;
	unsigned height = rc.bottom - rc.top;

	DXGI_SWAP_CHAIN_DESC sc;
	ZeroMemory(&sc, sizeof(sc));
	sc.BufferCount = 1;
	sc.BufferDesc.Width = width;
	sc.BufferDesc.Height = height;
	sc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sc.BufferDesc.RefreshRate.Numerator = 60;
	sc.BufferDesc.RefreshRate.Denominator = 1;
	sc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sc.OutputWindow = hwnd;
	sc.SampleDesc.Count = 1;
	sc.SampleDesc.Quality = 0;
	sc.Windowed = TRUE;

	hr = D3D10CreateDeviceAndSwapChain(NULL,
		D3D10_DRIVER_TYPE_HARDWARE, NULL, DEVICE_DEBUG_FLAGS,
		D3D10_SDK_VERSION, &sc, &swap, &device);
	if (FAILED(hr))
		return -1;

	ID3D10Texture2D *buffer;
	hr = swap->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*) &buffer);
	if (FAILED(hr))
		return hr;
	hr = device->CreateRenderTargetView(buffer, NULL, &target);
	buffer->Release();
	if (FAILED(hr))
		return hr;

	D3D10_TEXTURE2D_DESC td;
	td.Width = width;
	td.Height = height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_D32_FLOAT;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D10_USAGE_DEFAULT;
	td.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;
	hr = device->CreateTexture2D(&td, NULL, &depthStencil);
	if (FAILED(hr))
		return -1;

	D3D10_DEPTH_STENCIL_VIEW_DESC dsvd;
	dsvd.Format = td.Format;
	dsvd.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(depthStencil, &dsvd, &depthStencilView);
	if (FAILED(hr))
		return -1;

	D3D10_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	device->RSSetViewports(1, &vp);

	D3D10_RASTERIZER_DESC rd;
	rd.FillMode = D3D10_FILL_SOLID;
	rd.CullMode = D3D10_CULL_BACK;
	rd.FrontCounterClockwise = true;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0.0f;
	rd.SlopeScaledDepthBias =0.0f;
	rd.DepthClipEnable = true;
	rd.ScissorEnable = false;
	rd.MultisampleEnable = false;
	rd.AntialiasedLineEnable = false;
	hr = device->CreateRasterizerState(&rd, &rasterizerState);
	if (FAILED(hr))
		return -1;
	device->RSSetState(rasterizerState);

	device->OMSetRenderTargets(1, &target, depthStencilView);
	
	return S_OK;
}

void printx(const char *fmt, ...) {
#if DEBUG || _DEBUG
	char buf[128];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	buf[127] = 0;
	va_end(ap);
	OutputDebugString(buf);
#endif
}

void printmtx(D3DXMATRIX *m, const char *name) {
#if DEBUG || _DEBUG
	printx("| %8.4f %8.4f %8.4f %8.4f | \"%s\"\n",
		m->_11, m->_12, m->_13, m->_14, name);
	printx("| %8.4f %8.4f %8.4f %8.4f |\n",
		m->_21, m->_22, m->_23, m->_24);
	printx("| %8.4f %8.4f %8.4f %8.4f |\n",
		m->_31, m->_32, m->_33, m->_34);
	printx("| %8.4f %8.4f %8.4f %8.4f |\n",
		m->_41, m->_42, m->_43, m->_44);
#endif
}
