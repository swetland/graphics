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

// idx, src, dst, count, offset, stride, divisor
static VertexAttrDesc layout[] = {
	{ 0, SRC_FLOAT, DST_FLOAT, 3,  0, 32, 0 },
	{ 1, SRC_FLOAT, DST_FLOAT, 3, 12, 32, 0 },
	{ 2, SRC_FLOAT, DST_FLOAT, 2, 24, 32, 0 },
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
	Program pgm;
	IndexBuffer ibuf;
	VertexBuffer vbuf;
	UniformBuffer ubuf;
	VertexAttributes attr;

	mat4 proj;
	struct model *m;
};

TestApp::TestApp() : App(), r(0.0) {
}

void TestApp::release(void) {
}

int TestApp::init(void) {
	VertexBuffer *data[] = {
		&vbuf, &vbuf, &vbuf,
	};

	if (!(m = load_wavefront_obj("unitcubeoid.obj")))
		return error("cannot load model");
	printx("Object Loaded. %d vertices, %d indices.\n", m->vcount, m->icount);

	proj.setPerspective(D2R(90.0), width / (float) height, 0.1f, 250.0f);

	ps.load("simple.fragment");
	vs.load("simple.vertex");
	pgm.link(&vs, &ps);

	vbuf.load(m->vdata, 32 * m->vcount);
	ibuf.load(m->idx, 2 * m->icount);
	ubuf.load(NULL, 32 * 4);

	attr.init(layout, data, sizeof(layout)/sizeof(layout[0]));

	/* this will persist because it is part of the VAO state */
	ibuf.use();

	return 0;
}

void TestApp::render(void) {
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

	ubuf.load(&cb0, 32 * 4);

	pgm.use();
	ubuf.use(0);
	attr.use();
	glDrawElements(GL_TRIANGLES, m->icount, GL_UNSIGNED_SHORT, NULL);
}

App *createApp(void) {
	return new TestApp();
}
