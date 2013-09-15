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
#include "util.h"

#include "texturefont.h"

#include "Model.h"
#include "Effect.h"
#include "Renderable.h"

class TestApp : public App {
public:
	TestApp();
	int init(void);
	void render(void);
	void onKeyUp(unsigned code);
private:
	float r;

	Model *m;
	Effect *e;

	Renderable *grid;
	Effect *grid_effect;

	UniformBuffer obj, mat, scn;
	mat4 proj;

	TextureFont font;

	Renderable *fullscreen;
	Effect *copy;

	Effect *vblur;
	Effect *hblur;
	FrameBuffer fb0;
	FrameBuffer fb1;

	int postproc;
};

TestApp::TestApp() : App(), r(0.0) { }

int TestApp::init(void) {
	/* resources for scene */
	if (!(m = Model::load("unitcubeoid")))
		return error("cannot load cube object");

	if (!(e = Effect::load("simple+SPECULAR")))
		return error("could not load simple effect");

	proj.setPerspective(D2R(90.0), width / (float) height, 0.1f, 250.0f);

	font.init("orbitron-bold-72");

	/* resources for post-processing */
	hblur = Effect::load("rectangle+HBLUR");
	vblur = Effect::load("rectangle+VBLUR");
	copy = Effect::load("rectangle+COPY+SKIPLINES");

	fb0.init(width, height);
	fb1.init(width, height);

	fullscreen = Renderable::createFullscreenQuad();
	grid = Renderable::createXZgrid(10.0, 20);
	grid_effect = Effect::load("flat");

	postproc = 0;
	return 0;
}

void TestApp::onKeyUp(unsigned code) {
	if (code == SDL_SCANCODE_SPACE)
		postproc = !postproc;
}

void TestApp::render(void) {
	struct ubScene scene;
	struct ubObject object;
	struct ubMaterial material;
	mat4 model, view, tmp;

	/* if we're applying post-processing, we render to an offscreen buffer */
	if (postproc)
		fb0.use();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	r += 1.0;
	if (r > 360.0) r = 0.0;

	view.camera(vec3(0, 1.0, 3.0), vec3(0, 1.0, 0.0), vec3(0, 1, 0));

	model.identity();
	object.mvp = model * view * proj;
	object.mv = model * view;
	obj.load(&object, sizeof(object));
	obj.use(U_OBJECT);
	grid_effect->apply();
	grid->render();

	scene.Ortho.setOrtho(0, width, height, 0, -1.0, 1.0);
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
	font.render();

	if (postproc) {
		glDisable(GL_DEPTH_TEST);

		fb1.use();
		glClear(GL_COLOR_BUFFER_BIT);
		vblur->apply();
		fb0.useTexture(0);
		fullscreen->render();

		fb0.use();
		glClear(GL_COLOR_BUFFER_BIT);
		hblur->apply();
		fb1.useTexture(0);
		fullscreen->render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		copy->apply();
		fb0.useTexture(0);
		fullscreen->render();
	}
}

App *createApp(void) {
	return new TestApp();
}
