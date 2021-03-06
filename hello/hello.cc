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
#include <stdlib.h>

#include "app.h"
#include "matrix.h"
#include "util.h"
#include "textgrid.h"
#include "shared.h"
#include "Effect.h"

// idx, src, dst, count, offset, stride, divisor
static VertexAttrDesc layout[] = {
        { 0, SRC_FLOAT, DST_FLOAT,      3,  0, 32, 0 },
        { 1, SRC_FLOAT, DST_FLOAT,      3, 12, 32, 0 },
        { 2, SRC_FLOAT, DST_FLOAT,      2, 24, 32, 0 },
	{ 3, SRC_INT8,  DST_NORMALIZED, 4,  0,  4, 1 },
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
	void onKeyUp(unsigned code);
	void onResize(void);

private:
	float t;

	Effect *e;

	IndexBuffer ibuf;
	VertexBuffer vbuf;
	VertexBuffer lbuf;
	VertexAttributes attr;

	UniformBuffer scn;
	UniformBuffer mat;
	UniformBuffer obj;

	TextGrid text;

	mat4 proj;
	float zoom, rx, ry;
	float nx,ny;
	struct model *m;
};

TestApp::TestApp() : App(), t(0.0f), zoom(0), rx(0), ry(0), nx(0), ny(0) {
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

	lbuf.load(location, lcount * 4);
}

int TestApp::init(void) {
	VertexBuffer *data[] = {
		&vbuf, &vbuf, &vbuf, &lbuf,
	};

	if (!(e = Effect::load("simple")))
		return -1;
	if (!(m = load_wavefront_obj("unitcubeoid.obj")))
		return error("cannot load model");
	printx("Object Loaded. %d vertices, %d indices.\n", m->vcount, m->icount);

	vbuf.load(m->vdata, 32 * m->vcount);
	ibuf.load(m->idx, 2 * m->icount);

	build();
	zoom = SZ;

	attr.init(layout, data, sizeof(layout) / sizeof(layout[0]));
	ibuf.use();
	
	if (text.init(16, 16, width/16, height/16))
		return -1;

	onResize();

	return 0;
}

void TestApp::onResize(void) {
	proj.setPerspective(D2R(50.0), width / (float) height, 0.1f, 250.0f);
	text.resize(width/16, height/16);
}

static float rate = 90.0;

void TestApp::onKeyUp(unsigned code) {
	switch (code) {
	case SDL_SCANCODE_P: {
		Effect *e2 = Effect::load("simple");
		if (e2) {
			delete e;
			e = e2;
		}
		break;
	}
	case SDL_SCANCODE_RETURN:
		fullscreen(!isFullscreen());
		break;
	}
}

void TestApp::render(void) {
	int update = 0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

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

	if (keydown(SDL_SCANCODE_A)) { nx -= 0.01; update = 1; }
	if (keydown(SDL_SCANCODE_D)) { nx += 0.01; update = 1; }
	if (keydown(SDL_SCANCODE_W)) { ny -= 0.01; update = 1; }
	if (keydown(SDL_SCANCODE_S)) { ny += 0.01; update = 1; }
	if (update)
		build();


	struct ubScene scene;
	struct ubObject object;
	struct ubMaterial material;
	
	mat4 model, view, tmp;

	view.identity().rotateX(D2R(10)).rotateY(t).translate(0, 0, -10);
	view.identity().rotateY(D2R(ry)).rotateX(D2R(rx)).translate(0, 0, -zoom);
	model.identity().translate(0.5, 0.5, 0.5);
	object.mvp = model * view * proj;
	object.mv = model * view;

	scene.Ortho.setOrtho(0, width, height, 0, -1.0, 1.0);
	scene.OrthoSize.set(width, height, 0, 0);
	scene.TextGrid.set(16, 16, width / 16, height / 16);
	scene.LightColor.set(1.0, 1.0, 1.0);
	scene.LightPosition = view * vec4(0, 1, 0, 0);
	scene.LightPosition.w = 0;

	material.Ambient.set(0.325,0.325,0.325,1.0);
	material.Diffuse.set(1.0,1.0,1.0,1.0);
	material.Specular.set(1.0,1.0,1.0,1.0);
	material.Color.set(0,1,0,1);
	material.Shininess = 250.0f;

	scn.load(&scene, sizeof(scene));
	mat.load(&material, sizeof(material));
	obj.load(&object, sizeof(object));
	scn.use(U_SCENE);
	mat.use(U_MATERIAL);
	obj.use(U_OBJECT);

	e->apply();
	attr.use();
	glDrawElementsInstanced(GL_TRIANGLES, m->icount, GL_UNSIGNED_SHORT, NULL, lcount);

	text.clear();
	text.printf(0, 0, "rx: %8.4f", rx);
	text.printf(0, 1, "ry: %8.4f", ry);
	text.printf(0, 2, "zm: %8.4f", zoom);
	text.printf(0, -1, "%d fps", fps);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	text.render();
	glDisable(GL_BLEND);
}

App *createApp(void) {
	return new TestApp();
}
