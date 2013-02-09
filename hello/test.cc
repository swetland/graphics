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
};

class TestApp : public App {
public:
	TestApp();
	int init(void);
	void render(void);
	void release(void);
	void build(void);

private:
	float r;

	PixelShader ps;
	VertexShader vs;
	IndexBuffer ibuf;
	VertexBuffer vbuf;
	UniformBuffer ubuf;
	VertexBuffer lbuf;
	InputConfiguration cfg;

	mat4 proj;
	struct model *m;
};

TestApp::TestApp() : App(), r(0.0) {
}

void TestApp::release(void) {
}

int TestApp::init(void) {
	if (loadShader(&ps, "TestPS."SL))
		return -1;
	if (loadShader(&vs, "TestVS."SL, obj_layout, sizeof(obj_layout) / sizeof(obj_layout[0])))
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

	useConfig(&cfg);
	useBuffer(&ubuf, 0);
	useBuffer(&vbuf, 0, 32, 0);
	useBuffer(&lbuf, 1, 4, 0);
	useBuffer(&ibuf);

	return 0;
}

void TestApp::render(void) {
	unsigned stride, offset;

	useConfig(&cfg);
	struct {
		mat4 mvp;
		mat4 mv;
	} cb0;
	mat4 world, view, tmp;

	r += 0.010;
	if (r > 360.0) r = 0.0;

	view.identity().rotateX(D2R(10)).rotateY(r).translate(0,0,-2);
	world.identity();

	cb0.mvp = world * view * proj;
	cb0.mv = world * view;

	updateBuffer(&ubuf, &cb0);
	drawIndexed(m->icount);
}

App *createApp(void) {
	return new TestApp();
}
