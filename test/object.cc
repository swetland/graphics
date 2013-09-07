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
#include "shared.h"

#include "texturefont.h"

#include "Model.h"
#include "Effect.h"

struct Grid {
	Grid(float size, unsigned steps) {
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
	void render(void) {
		attr.use();
		glDrawArrays(GL_LINES, 0, count);
	}
	VertexBuffer buf;
	VertexAttributes attr;
	unsigned count;
};
		
class TestApp : public App {
public:
	TestApp();
	int init(void);
	void render(void);

private:
	float r;

	Model *m;
	Effect *e;

	Grid *g;
	Effect *ge;

	UniformBuffer obj, mat, scn;
	mat4 proj;

	TextureFont font;
};

TestApp::TestApp() : App(), r(0.0) { }

int TestApp::init(void) {
	if (!(m = Model::load("unitcubeoid")))
		return error("cannot load cube object");

	if (!(e = Effect::load("simple+SPECULAR")))
		return error("could not load simple effect");

	g = new Grid(10.0, 20);
	ge = Effect::load("flat");
		
	proj.setPerspective(D2R(90.0), width / (float) height, 0.1f, 250.0f);

	font.init(this, "orbitron-bold-72");
	return 0;
}

int frame = 0;
void TestApp::render(void) {
	struct ubScene scene;
	struct ubObject object;
	struct ubMaterial material;
	mat4 model, view, tmp;

	frame ++;

	r += 1.0;
	if (r > 360.0) r = 0.0;

	view.camera(vec3(0, 1.0, 3.0), vec3(0, 1.0, 0.0), vec3(0, 1, 0));

	model.identity();
	object.mvp = model * view * proj;
	object.mv = model * view;
	obj.load(&object, sizeof(object));
	obj.use(U_OBJECT);
	ge->apply();
	g->render();

	scene.LightColor.set(1.0, 1.0, 1.0);
	scene.LightPosition = view * vec4(0, 1, 0, 0);
	scene.LightPosition.w = 0;
	scn.load(&scene, sizeof(scene));
	scn.use(U_SCENE);

	material.Ambient.set(0.325,0.325,0.325,1.0);
	material.Diffuse.set(1.0,1.0,1.0,1.0);
	material.Specular.set(0, 0, 0, 0);
	material.Shininess = 50.0f;
	material.Color.set(1, 0, 0, 1);
	mat.load(&material, sizeof(material));
	mat.use(U_MATERIAL);

	model.identity().rotateY(D2R(r)).translate(0, 0.5, 0.0);
	object.mvp = model * view * proj;
	object.mv = model * view;
	obj.load(&object, sizeof(object));
	e->apply();
	m->render();

	model.identity().translate(-1.5, 0.5, 0.0);
	object.mvp = model * view * proj;
	object.mv = model * view;
	obj.load(&object, sizeof(object));
	e->apply();
	m->render();
	
	material.Specular.set(1,1,1,1);
	mat.load(&material, sizeof(material));
	model.identity().translate(1.5, 0.5, 0.0);
	object.mvp = model * view * proj;
	object.mv = model * view;
	obj.load(&object, sizeof(object));
	e->apply();
	m->render();

	font.clear();
	font.puts(100, 100, "Hello World!");
	font.render(this);
}

App *createApp(void) {
	return new TestApp();
}
