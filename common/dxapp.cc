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

#include "util.h"
#include "app.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

App::App() : width(800), height(600), active(0),
	mouseDX(0), mouseDY(0), mouseDZ(0), mouseBTN(0),
	device(NULL), targetView(NULL), depthView(NULL),
	swapchain(NULL), rsDefault(NULL), dsDepthEnabled(NULL), dsDepthDisabled(NULL) {
}

App::~App() {
}

void App::stop(void) {
	if (swapchain) {
		// teardown during fullscreen will crash
		// ensure we're not fullscreen first...
		swapchain->SetFullscreenState(false, NULL);
	}
	if (device) device->Release();

	release();

	if (rsDefault) rsDefault->Release();
	if (dsDepthEnabled) dsDepthEnabled->Release();
	if (dsDepthDisabled) dsDepthDisabled->Release();
	if (depthView) depthView->Release();
	if (depthBuffer) depthBuffer->Release();
	if (targetView) targetView->Release();
	if (swapchain) swapchain->Release();
	if (device) device->ClearState();

	printx("-- goodbye --\n");
}

static App *app;
static int moving = 0;
static int frame = 0;

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
	case WM_SIZE:
#if 0
		if (wParam == SIZE_MINIMIZED) printx("win: minimized\n");
		else if (wParam == SIZE_MAXIMIZED) printx("win: maximized\n");
		else if (wParam == SIZE_RESTORED) printx("win: restored\n");
		else printx("win: sized %d??\n", wParam);
#endif
		if (!moving)
			app->reconfigure(0);
		break;
	case WM_ENTERSIZEMOVE:
		moving = 1;
		break;
	case WM_EXITSIZEMOVE:
		moving = 0;
		app->reconfigure(0);
		break;
	case WM_MOUSEMOVE:
		app->setMouseXY((short) LOWORD(lParam), (short) HIWORD(lParam));
		break;
	case WM_ACTIVATEAPP:
		printx("win: activate: %d\n", wParam);
		app->setActive(wParam);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	app = createApp();
	if (app->start(hInstance, nCmdShow))
		return 0;
	app->eventloop();
	app->stop();
	delete app;
	return 0;
}

void App::eventloop(void) {
	HRESULT hr;
	MSG msg = {0};
	while (msg.message != WM_QUIT) {
		if (1) {
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				continue;
			}
		} else {
			if (GetMessage(&msg, 0, 0, 0) == -1)
				return;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

		hr = dkeyboard->GetDeviceState(sizeof(keystate), (void*) keystate);
		if (FAILED(hr)) {
			// We can lose the keyboard if we lose focus
			// Try to reacquire it
			dkeyboard->Acquire();
		} else {
			if (keystate[DIK_ESCAPE] & 0x80)
				break;
		}

		DIMOUSESTATE2 state;
		hr = dmouse->GetDeviceState(sizeof(DIMOUSESTATE2), &state);
		if (FAILED(hr)) {
			dmouse->Acquire();
			mouseDX = mouseDY = mouseDZ = 0;
		} else {
			mouseDX = state.lX;
			mouseDY = state.lY;
			mouseDZ = state.lZ;
			mouseBTN =
				((state.rgbButtons[0] >> 7) & 1) |
				((state.rgbButtons[1] >> 6) & 2) |
				((state.rgbButtons[2] >> 5) & 4);
		}

		float rgba[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; 
		device->ClearRenderTargetView(targetView, rgba);
		device->ClearDepthStencilView(depthView, D3D10_CLEAR_DEPTH, 1.0f, 0 );

		app->render();

		swapchain->Present(1, 0);
		frame++;
	}
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
	if (initDirectInput())
		return -1;
	if (init())
		return -1;
	return 0;
}

int App::initDirectInput(void) {
	HRESULT hr;
	hr = DirectInput8Create(hinstance,
		DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**) &dinput, NULL);
	if (FAILED(hr))
		return -1;

	hr = dinput->CreateDevice(GUID_SysKeyboard, &dkeyboard, NULL);
	if (FAILED(hr))
		return -1;
	hr = dkeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
		return -1;
	hr = dkeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))
		return -1;

	hr = dinput->CreateDevice(GUID_SysMouse, &dmouse, NULL);
	if (FAILED(hr))
		return -1;
	hr = dmouse->SetDataFormat(&c_dfDIMouse2);
	if (FAILED(hr))
		return -1;
	hr = dmouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))
		return -1;

	return 0;
}

int App::reconfigure(int init) {
	HRESULT hr;
	int w, h;

	if (init) {
		printx("reconfigure: init\n");
	} else {
		RECT r;
		GetClientRect(hwnd, &r);
		w = r.right - r.left;
		h = r.bottom - r.top;

		// TODO: handle minimized state
		if (w < 320) w = 320;
		if (h < 240) h = 240;

		if ((width == w) && (height == h))
			return 0;

		width = w;
		height = h;
		if (targetView) {
			targetView->Release();
			targetView = NULL;
		}
		if (depthView) {
			depthView->Release();
			depthView = NULL;
		}
		if (depthBuffer) {
			depthBuffer->Release();
			depthBuffer = NULL;
		}

		hr = swapchain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		if (FAILED(hr))
			return -1;
	}

	ID3D10Texture2D *buffer;
	hr = swapchain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*) &buffer);
	if (FAILED(hr)) printx("OOPS\n");
	hr = device->CreateRenderTargetView(buffer, NULL, &targetView);
	buffer->Release();
	if (FAILED(hr))
		return -1;

	D3D10_TEXTURE2D_DESC td;
	td.Width = width;
	td.Height = height;
	td.MipLevels = 1;
	td.ArraySize = 1;
//	td.Format = DXGI_FORMAT_D32_FLOAT;
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // NVIDIA suggested
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D10_USAGE_DEFAULT;
	td.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;
	hr = device->CreateTexture2D(&td, NULL, &depthBuffer);
	if (FAILED(hr))
		return -1;

	D3D10_DEPTH_STENCIL_VIEW_DESC dsvd;
	dsvd.Format = td.Format;
	dsvd.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(depthBuffer, &dsvd, &depthView);
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

	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	device->OMSetRenderTargets(1, &targetView, depthView);

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
		D3D10_SDK_VERSION, &sc, &swapchain, &device);
	if (FAILED(hr))
		return -1;

	if (reconfigure(1))
		return -1;

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
	hr = device->CreateRasterizerState(&rd, &rsDefault);
	if (FAILED(hr))
		return -1;

	D3D10_DEPTH_STENCIL_DESC dsd;
	memset(&dsd, 0, sizeof(dsd));
	dsd.DepthEnable = true;
	dsd.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D10_COMPARISON_LESS;
	dsd.StencilEnable = false;
	dsd.StencilReadMask = D3D10_DEFAULT_STENCIL_READ_MASK;
	dsd.StencilWriteMask = D3D10_DEFAULT_STENCIL_WRITE_MASK;
	hr = device->CreateDepthStencilState(&dsd, &dsDepthEnabled);
	if (FAILED(hr))
		return -1;
	hr = device->CreateDepthStencilState(&dsd, &dsDepthDisabled);
	if (FAILED(hr))
		return -1;

	device->RSSetState(rsDefault);
	return S_OK;
}

// ----

static int _compile_shader(const char *fn, void *data, unsigned sz,
	const char *profile, ID3D10Blob **shader) {
	HRESULT hr;
	ID3D10Blob *errors = NULL;

	hr = D3D10CompileShader((char*) data, sz, fn,
		NULL, NULL, "main", profile,
		D3D10_SHADER_ENABLE_STRICTNESS,
		shader, &errors);

	if (errors) {
		printx("--- error compiling '%s' ---\n", fn);
		OutputDebugString((char*) errors->GetBufferPointer());
		errors->Release();
		return -1;
	}
	if (FAILED(hr))
		return error("cannot compile shader 0x%08x", hr);

	printx("Compiled '%s' to %d bytes\n", fn, (*shader)->GetBufferSize());
	return 0;
}

int App::compileShader(VertexShader *_vs, const char *fn,
	void *data, unsigned len, int raw,
	AttribInfo *desc, unsigned dcount) {
	ID3D10VertexShader *vs = NULL;
	ID3D10InputLayout *layout = NULL;
	ID3D10Blob *bin = NULL;
	HRESULT hr;

	if (raw) {
		hr = device->CreateVertexShader(data, len, &vs);
		if (FAILED(hr))
			return error("failed to create shader '%s' 0x%08x", fn, hr);
		hr = device->CreateInputLayout((D3D10_INPUT_ELEMENT_DESC*) desc,
			dcount, data, len, &layout);
	} else {
		if (_compile_shader(fn, data, len, "vs_4_0", &bin))
			return -1;
		hr = device->CreateVertexShader(bin->GetBufferPointer(), bin->GetBufferSize(), &vs);
		if (FAILED(hr)) {
			bin->Release();
			return error("failed to create shader '%s' 0x%08x", fn, hr);
		}
		hr = device->CreateInputLayout((D3D10_INPUT_ELEMENT_DESC*) desc, dcount,
			bin->GetBufferPointer(), bin->GetBufferSize(), &layout);
		bin->Release();
	}
	if (FAILED(hr))
		return error("failed to bind layout for '%s' 0x%08x", fn, hr);
	if (_vs->vs)
		_vs->vs->Release();
	if (_vs->layout)
		_vs->layout->Release();
	_vs->vs = vs;
	_vs->layout = layout;
	_vs->desc = (D3D10_INPUT_ELEMENT_DESC*) desc;
	_vs->dcount = dcount;
	return 0;
}

int App::compileShader(PixelShader *_ps, const char *fn,
	void *data, unsigned len, int raw) {
	ID3D10PixelShader *ps;
	HRESULT hr;
	if (raw) {
		hr = device->CreatePixelShader(data, len, &ps);
	} else {
		ID3D10Blob *bin = NULL;
		if (_compile_shader(fn, data, len, "ps_4_0", &bin))
			return -1;
		hr = device->CreatePixelShader(bin->GetBufferPointer(), bin->GetBufferSize(), &ps);
		bin->Release();
	}
	if (FAILED(hr))
		return error("failed to create shader '%s' 0x%08x", fn, hr);
	if (_ps->ps)
		_ps->ps->Release();
	_ps->ps = ps;
	return 0;
}

// TODO choose raw based on content or extension
int App::loadShader(VertexShader *vs, const char *fn,
	AttribInfo *layout, unsigned lcount) {
	void *data;
	unsigned sz;
	if (!(data = load_file(fn, &sz)))
		return -1;
	int r = compileShader(vs, fn, data, sz, 0, layout, lcount);
	free(data);
	return r;
}

int App::loadShader(PixelShader *ps, const char *fn) {
	void *data;
	unsigned sz;
	if (!(data = load_file(fn, &sz)))
		return -1;
	int r = compileShader(ps, fn, data, sz, 0);
	free(data);
	return r;
}

int App::loadTextureRGBA(Texture2D *tex, const char *fn, int genmips) {
	void *data;
	unsigned dw, dh;
	int r;
	if (!(data = load_png_rgba(fn, &dw, &dh, 0)))
		return -1;
	r = createTextureRGBA(tex, data, dw, dh, genmips);
	free(data);
	return r;
}

int App::createTextureRGBA(Texture2D *tex, void *data, unsigned tw, unsigned th, int genmips) {
	int stride = tw * 4;
	int size = stride * th;
	HRESULT hr;
	ID3D10Texture2D *texture;

	D3D10_TEXTURE2D_DESC txd;
	txd.Width = tw;
	txd.Height = th;
	txd.MipLevels = genmips ? 0 : 1;
	txd.ArraySize = 1;
	txd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	txd.SampleDesc.Count = 1;
	txd.SampleDesc.Quality = 0;
	txd.Usage = D3D10_USAGE_DEFAULT;
	txd.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	if (genmips)
		txd.BindFlags |= D3D10_BIND_RENDER_TARGET;
	txd.CPUAccessFlags = 0; // no cpu access
	txd.MiscFlags = genmips ? D3D10_RESOURCE_MISC_GENERATE_MIPS : 0;

	D3D10_SUBRESOURCE_DATA idata;
	idata.pSysMem = data;
	idata.SysMemPitch = stride;
	idata.SysMemSlicePitch = size;

	hr = device->CreateTexture2D(&txd, genmips ? NULL : &idata, &texture);
	if (FAILED(hr))
		return error("failed to create texture 0x%08x",hr);

	D3D10_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvd.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = genmips ? -1 : 1;
	srvd.Texture2D.MostDetailedMip = 0;
	hr = device->CreateShaderResourceView(texture, &srvd, &tex->srv);
	if (FAILED(hr)) {
		tex->tex->Release();
		tex->tex = NULL;
		return error("failed to create shader resource view 0x%08x",hr);
	}

	if (genmips) {
		device->UpdateSubresource(texture, 0, NULL, data, stride, size);
		device->GenerateMips(tex->srv);
	}

	return 0;
}

int _create_buffer(ID3D10Device *device, D3D10_BIND_FLAG flag,
	void *data, int sz, ID3D10Buffer **buf) {
	HRESULT hr;
	ID3D10Buffer *old = *buf;

	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sz;
	bd.BindFlags = flag;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA idata;
	idata.pSysMem = data;
	hr = device->CreateBuffer(&bd, data ? &idata : NULL, buf);
	if (FAILED(hr))
		return error("create buffer failed 0x%08x", hr);
	if (old != NULL)
		old->Release();
	return 0;
}

int App::initBuffer(VertexBuffer *vb, void *data, int sz) {
	return _create_buffer(device, D3D10_BIND_VERTEX_BUFFER, data, sz, &vb->buf);
}
int App::initBuffer(IndexBuffer *ib, void *data, int sz) {
	return _create_buffer(device, D3D10_BIND_INDEX_BUFFER, data, sz, &ib->buf);
}
int App::initBuffer(UniformBuffer *ub, void *data, int sz) {
	return _create_buffer(device, D3D10_BIND_CONSTANT_BUFFER, data, sz, &ub->buf);
}

// ----

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

void printmtx(float *m, const char *name) {
#if DEBUG || _DEBUG
	printx("| %8.4f %8.4f %8.4f %8.4f | \"%s\"\n", m[0], m[1], m[2], m[3], name);
	printx("| %8.4f %8.4f %8.4f %8.4f |\n", m[4], m[5], m[6], m[7]);
	printx("| %8.4f %8.4f %8.4f %8.4f |\n", m[8], m[9], m[10], m[11]);
	printx("| %8.4f %8.4f %8.4f %8.4f |\n", m[12], m[13], m[14], m[15]);
#endif
}

int error(const char *fmt, ...) {
	char buf[128];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	buf[127] = 0;
#if _DEBUG
	printx("ERROR: %s\n", buf);
#else
	MessageBox(NULL, buf, "Error", MB_OK);
#endif
	return -1;
}
