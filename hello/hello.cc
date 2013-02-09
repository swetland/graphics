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
#include "textgrid.h"

#include <stdio.h>
#include <stdlib.h>

static AttribInfo obj_layout[] = {
	{ "POSITION", 0, FMT_32x3_FLOAT, 0,  0, VERTEX_DATA, 0 },
	{ "NORMAL",   0, FMT_32x3_FLOAT, 0, 12, VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, FMT_32x2_FLOAT, 0, 24, VERTEX_DATA, 0 },
	{ "LOCATION", 0, FMT_8x4_SNORM,  1,  0, INSTANCE_DATA, 1 },
};

static float locationx[] = {
	0, 0, 0,
	-2, 0, 0,
	-4, 0, 0,
	2, 0, 0,
	4, 0, 0,
};

#define SZ 32
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
	void build(void);

private:
	float t;

	PixelShader ps;
	VertexShader vs;
	IndexBuffer ibuf;
	VertexBuffer vbuf;
	UniformBuffer ubuf;
	VertexBuffer lbuf;
	InputConfiguration cfg;

	TextGrid text;
	float nx,ny;

	mat4 proj;
	float rx, ry, zoom;
	struct model *m;
};

TestApp::TestApp() : App(), t(0.0f), zoom(0), rx(0), ry(0), nx(0), ny(0) {
	width = 1280;
	height = 1024;
}

void TestApp::release(void) {
}

void TestApp::build(void) {
	float scale = SZh;
	int x, y, z, n = 0;
	for (z = -SZh; z < SZh; z++) {
		for (x = -SZh; x < SZh; x++) {
			for (y = -SZh; y < SZh; y++) {
				float fx = x/scale * 0.50f + nx;
				float fy = y/scale * 0.50f;
				float fz = z/scale * 0.50f + ny;
#if 0
				if (snoise(fx,fy,fz) > 0.1)
					continue;
#else
				float sn =
					snoise(fx,fz) +
					snoise(fx*2.0f,fz*2.0f) / 4.0f +
					snoise(fx*4.0f,fz*4.0f) / 8.0f;
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

	initBuffer(&lbuf, location, lcount * 4);
}

int TestApp::init(void) {
	if (loadShader(&ps, "SimplePS."SL ))
		return -1;
	if (loadShader(&vs, "SimpleVS."SL, obj_layout, sizeof(obj_layout) / sizeof(obj_layout[0])))
		return -1;

	if (!(m = load_wavefront_obj("unitcubeoid.obj")))
		return error("cannot load model");
	printx("Object Loaded. %d vertices, %d indices.\n", m->vcount, m->icount);

	if (initBuffer(&vbuf, m->vdata, 32 * m->vcount))
		return -1;
	if (initBuffer(&ibuf, m->idx, 2 * m->icount))
		return -1;
	if (initBuffer(&ubuf, NULL, 32 * 4))
		return -1;

	if (initConfig(&cfg, &vs, &ps))
		return -1;
	proj.setPerspective(D2R(90.0), width / (float) height, 0.1f, 250.0f);

	build();
	zoom = SZ;

	useConfig(&cfg);
	useBuffer(&ubuf, 0);
	useBuffer(&vbuf, 0, 32, 0);
	useBuffer(&lbuf, 1, 4, 0);
	useBuffer(&ibuf);
	
	if (text.init(this, 64, 64))
		return -1;

	return 0;
}

static float rate = 90.0;

void TestApp::render(void) {
	unsigned stride, offset;
	int update = 0;

	if (mouseBTN & 1) {
		float dx = ((float) mouseDX) / 400.0f;
		float dy = ((float) mouseDY) / 400.0f;
		ry += dx * rate;
		rx += dy * rate;
		if (rx < 0.0) rx += 360.0;
		else if (rx > 360.0) rx -= 360.0;
		if (ry < 0.0) ry += 360.0;
		else if (ry > 360.0) ry -= 360.0;
	}
	if (mouseBTN & 2) {
		float dy = ((float) mouseDY) / 400.0f;
		zoom += dy * rate;
		if (zoom < 5.0) zoom = 5.0;
		if (zoom > 100.0) zoom = 100.0;
	}

#if _WIN32
	if (keystate[DIK_A]) { nx -= 0.01; update = 1; }
	if (keystate[DIK_D]) { nx += 0.01; update = 1; }
	if (keystate[DIK_W]) { ny -= 0.01; update = 1; }
	if (keystate[DIK_S]) { ny += 0.01; update = 1; }
	if (keystate[DIK_P]) {
		loadShader(&ps, "SimplePS."SL);
		loadShader(&vs, "SimpleVS."SL, obj_layout,
			sizeof(obj_layout) / sizeof(obj_layout[0]));
	}
oops:
	if (update)
		build();
#endif

	useConfig(&cfg);

	struct {
		mat4 mvp;
		mat4 mv;
	} cb0;
	mat4 world, view, tmp;

	view.identity().rotateX(D2R(10)).rotateY(t).translate(0, 0, -10);
	view.identity().rotateY(D2R(ry)).rotateX(D2R(rx)).translate(0, 0, -zoom);
//	world.identity().translate(0.5, 0.5, 0.5).rotateY(D2R(ry)).rotateX(D2R(rx));
	world.identity().translate(0.5, 0.5, 0.5);
	cb0.mvp = world * view * proj;
	cb0.mv = world * view;
	updateBuffer(&ubuf, &cb0);
	drawIndexedInstanced(m->icount, lcount);

	text.clear();
	text.printf(0, 0, "rx: %8.4f", rx);
	text.printf(0, 1, "ry: %8.4f", ry);
	text.printf(0, 2, "zm: %8.4f", zoom);
	text.printf(0, -1, "hello.cc");

	setBlend(1);
	text.render(this);
	setBlend(0);
}

App *createApp(void) {
	return new TestApp();
}
