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

#include "Renderable.h"

class XZGrid : public Renderable {
public:
	XZGrid(float size, unsigned steps);
	virtual void render(void);
private:
	VertexBuffer buf;
	VertexAttributes attr;
	unsigned count;
};

XZGrid::XZGrid(float size, unsigned steps) : Renderable() {
	static VertexAttrDesc layout[] = {
		{ 0, SRC_FLOAT, DST_FLOAT, 3, 0, 12, 0 },
	};
	count = (steps + 1) * 4;
	VertexBuffer *vb = &buf;
	float x, z;
 	float *varr = (float*) malloc(sizeof(float) * 3 * count);
	float *vtx = varr;
	float min = -(size / 2.0);
	float max = (size / 2.0) + 0.0001;
	float step = size / ((float) steps);
	for (x = min; x < max; x += step) {
		vtx[0] = x;
		vtx[1] = 0.0;
		vtx[2] = min;
		vtx[3] = x;
		vtx[4] = 0.0;
		vtx[5] = max;
		vtx += 6;
	}
	for (z = min; z < max; z += step) {
		vtx[0] = min;
		vtx[1] = 0.0;
		vtx[2] = z;
		vtx[3] = max;
		vtx[4] = 0.0;
		vtx[5] = z;
		vtx += 6;
	}
	buf.load(varr, count * 3 * sizeof(float));
	attr.init(layout, &vb, 1);
}

void XZGrid::render(void) {
	attr.use();
	glDrawArrays(GL_LINES, 0, count);
}

Renderable* Renderable::createXZgrid(float size, unsigned steps) {
	return new XZGrid(size, steps);
}

static VertexAttrDesc fsq_layout[] = {
	{ 0, SRC_FLOAT, DST_FLOAT,  2, 0, 16, 0 },
	{ 1, SRC_FLOAT, DST_FLOAT,  2, 8, 16, 0 },
};

static float fsq_vertices[] = {     //  1,4    6
	-1.0f,  1.0f,  0.0f,  1.0f, //   o  |  o
	-1.0f, -1.0f,  0.0f,  0.0f, //      |
	 1.0f, -1.0f,  1.0f,  0.0f, // -----|-----
	-1.0f,  1.0f,  0.0f,  1.0f, //      |
	 1.0f, -1.0f,  1.0f,  0.0f, //   o  |  o 
	 1.0f,  1.0f,  1.0f,  1.0f, //   2    3,5
};

class FSQuad : public Renderable {
public:
	FSQuad();
	virtual void render(void);
private:
	VertexBuffer vbuf;
	VertexAttributes attr;
};

FSQuad::FSQuad() : Renderable() {
	VertexBuffer *data[] = { &vbuf, &vbuf };
	vbuf.load(fsq_vertices, sizeof(fsq_vertices));
	attr.init(fsq_layout, data, 2);
}

void FSQuad::render(void) {
	attr.use();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

Renderable* Renderable::createFullscreenQuad(void) {
	return new FSQuad();
}
