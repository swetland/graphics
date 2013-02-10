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

#define SL "hlsl"

enum {
	FMT_32x4_FLOAT = DXGI_FORMAT_R32G32B32A32_FLOAT,
	FMT_32x3_FLOAT = DXGI_FORMAT_R32G32B32_FLOAT,
	FMT_32x2_FLOAT = DXGI_FORMAT_R32G32_FLOAT,
	FMT_32x1_FLOAT = DXGI_FORMAT_R32_FLOAT,

	FMT_8x4_SNORM = DXGI_FORMAT_R8G8B8A8_SNORM,

	FMT_8x4_UNORM = DXGI_FORMAT_R8G8B8A8_UNORM,
	FMT_8x2_UNORM = DXGI_FORMAT_R8G8_UNORM,
	FMT_8x1_UNORM = DXGI_FORMAT_R8_UNORM,

	FMT_8x4_UINT = DXGI_FORMAT_R8G8B8A8_UINT,
	FMT_8x2_UINT = DXGI_FORMAT_R8G8_UINT,
	FMT_8x1_UINT = DXGI_FORMAT_R8_UINT,
};

enum {
	VERTEX_DATA = D3D10_INPUT_PER_VERTEX_DATA,
	INSTANCE_DATA = D3D10_INPUT_PER_INSTANCE_DATA,
};

struct AttribInfo {
	const char *name;
	unsigned nidx;
	unsigned format;
	unsigned slot;
	unsigned offset;
	unsigned type;
	unsigned divisor;
};

struct PixelShader {
	ID3D10PixelShader *ps;
	PixelShader() : ps(NULL) {};
	~PixelShader() { if (ps) ps->Release(); };
};

struct VertexShader {
	ID3D10VertexShader *vs;
	ID3D10InputLayout *layout;
	D3D10_INPUT_ELEMENT_DESC *desc;
	unsigned dcount;
	VertexShader() : vs(NULL), layout(NULL), desc(NULL), dcount(0) {};
	~VertexShader() { if (vs) { vs->Release(); layout->Release(); } };
};

struct Texture2D {
	ID3D10Texture2D *tex;
	ID3D10ShaderResourceView *srv;
	Texture2D() : tex(NULL), srv(NULL) {};
	~Texture2D() { if (tex) { tex->Release(); srv->Release(); } };
};

struct UniformBuffer {
	ID3D10Buffer *buf;
	UniformBuffer() : buf(NULL) {};
	~UniformBuffer() { if (buf) buf->Release(); };
};

struct VertexBuffer {
	ID3D10Buffer *buf;
	VertexBuffer() : buf(NULL) {};
	~VertexBuffer() { if (buf) buf->Release(); };
};

struct IndexBuffer {
	ID3D10Buffer *buf;
	IndexBuffer() : buf(NULL) {};
	~IndexBuffer() { if (buf) buf->Release(); };
};

struct InputConfiguration {
	VertexShader *vs;
	PixelShader *ps;

	IndexBuffer *ibuf;
	VertexBuffer *vbuf[16];
	unsigned vstride[16];
	unsigned voffset[16];
	UniformBuffer *ubuf[16];
	Texture2D *tex[16];

	int vcount;
	int ucount;
	int tcount;

	InputConfiguration() : vs(NULL), ps(NULL), vcount(0), ucount(0), tcount(0), ibuf(NULL) {
		memset(vbuf, 0, sizeof(vbuf));
		memset(ubuf, 0, sizeof(ubuf));
	}
};

class App {
public:
	App();
	virtual ~App();

	virtual int init(void) = 0;
	virtual void render(void) = 0;
	virtual void release(void) {};

	/* glue - do not use */
	int start(HINSTANCE hInstance, int nCmdShow);
	void eventloop(void);
	void stop(void);
	int reconfigure(int init);
	void setActive(int a) { active = a; };
	void setMouseXY(int x, int y) { mouseWX = x; mouseWY = y; };

	// TODO: move away from D3D10_INPUT...
	int compileShader(VertexShader *vs, const char *fn,
		void *data, unsigned len, int raw,
		AttribInfo *layout, unsigned lcount);
	int compileShader(PixelShader *ps, const char *fn,
		void *data, unsigned len, int raw);
	int loadShader(VertexShader *vs, const char *fn,
		AttribInfo *layout, unsigned lcount);
	int loadShader(PixelShader *ps, const char *fn);

	int loadTextureRGBA(Texture2D *tex, const char *fn, int genmips);
	int createTextureRGBA(Texture2D *tex, void *data, unsigned w, unsigned h, int genmips);

	int initConfig(InputConfiguration *ic, VertexShader *vs, PixelShader *ps);
	int initBuffer(VertexBuffer *vb, void *data, int sz);
	int initBuffer(IndexBuffer *ib, void *data, int sz);
	int initBuffer(UniformBuffer *ub, void *data, int sz);

	void updateBuffer(VertexBuffer *vb, void *data) {
		device->UpdateSubresource(vb->buf, 0, NULL, data, 0, 0);
	}
	void updateBuffer(IndexBuffer *ib, void *data) {
		device->UpdateSubresource(ib->buf, 0, NULL, data, 0, 0);
	}
	void updateBuffer(UniformBuffer *ub, void *data) {
		device->UpdateSubresource(ub->buf, 0, NULL, data, 0, 0);
	}

	void useConfig(InputConfiguration *ic);
	void useBuffer(VertexBuffer *vb, int slot, UINT stride, UINT offset);
	void useBuffer(IndexBuffer *ib);
	void useBuffer(UniformBuffer *ub, int slot);
	void useTexture(Texture2D *tex, int slot);
	void drawIndexedInstanced(unsigned numindices, unsigned numinstances) {
		device->DrawIndexedInstanced(numindices, numinstances, 0, 0, 0);
	}
	void drawInstanced(unsigned numvertices, unsigned numinstances) {
		device->DrawInstanced(numvertices, numinstances, 0, 0);
	}
	void drawIndexed(unsigned numindices) {
		device->DrawIndexed(numindices, 0, 0);
	};
	void setBlend(int enabled);
protected:
	int width;
	int height;

	/* mouse motion since last frame */
	int mouseDX, mouseDY, mouseDZ;
	/* mouse button state */
	int mouseBTN;
	/* mouse position in window coordinates */
	int mouseWX, mouseWY;
	/* keys down */
	unsigned char keystate[256];

	ID3D10Device *device;
	IDXGISwapChain *swapchain;
	ID3D10RenderTargetView *targetView;
	ID3D10Texture2D *depthBuffer;
	ID3D10DepthStencilView *depthView;
	ID3D10RasterizerState *rsDefault;
	ID3D10DepthStencilState *dsDepthEnabled;
	ID3D10DepthStencilState *dsDepthDisabled;
	ID3D10SamplerState *defaultSamplerState;
	ID3D10BlendState *bsAlpha;
	ID3D10BlendState *bsOpaque;
private:
	InputConfiguration *ic;
	LPDIRECTINPUT8 dinput;
	LPDIRECTINPUTDEVICE8 dkeyboard;
	LPDIRECTINPUTDEVICE8 dmouse;
	int initD3D(void);
	int initDirectInput(void);
	HWND hwnd;
	HINSTANCE hinstance;
	int active;
};

//int compileShader(const char *fn, const char *profile, ID3D10Blob **shader);

App *createApp(void);

#endif
