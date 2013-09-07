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

#include <string>

#include "Model.h"

#include "util.h"

// idx, src, dst, count, offset, stride, divisor
static VertexAttrDesc layout[] = {
	{ 0, SRC_FLOAT, DST_FLOAT, 3,  0, 32, 0 },
	{ 1, SRC_FLOAT, DST_FLOAT, 3, 12, 32, 0 },
	{ 2, SRC_FLOAT, DST_FLOAT, 2, 24, 32, 0 },
};

void Model::render(void) {
	attr.use();
	texture.use(0);
	glDrawElements(GL_TRIANGLES, icount, GL_UNSIGNED_SHORT, NULL);
}

int Model::init(const char *name) {
	struct model *m;
	VertexBuffer *data[] = {
		&vbuf, &vbuf, &vbuf,
	};

	std::string mname(name);
	std::string tname(name);
	mname.append(".obj");
	tname.append(".png");

	if (!(m = load_wavefront_obj(mname.c_str())))
		return error("cannot load model");

	printx("Object '%s' loaded. %d vertices, %d indices.\n",
		name, m->vcount, m->icount);

	texture.load(tname.c_str(), OPT_TEX2D_GEN_MIPMAP);

	vbuf.load(m->vdata, 32 * m->vcount);
	ibuf.load(m->idx, 2 * m->icount);

	icount = m->icount;

	attr.init(layout, data, sizeof(layout)/sizeof(layout[0]));

	/* this will persist because it is part of the VAO state */
	ibuf.use();

	delete_wavefront_obj(m);

	return 0;
}

Model *Model::load(const char *name) {
	Model *m = new Model();
	if (m->init(name)) {
		delete m;
		return nullptr;
	} else {
		return m;
	}
}

