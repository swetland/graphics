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

#include "Model.h"
#include "Effect.h"

class TestApp : public App {
public:
	TestApp();
	int init(void);
	void render(void);
	void release(void);
	void build(void);

private:
	float r;

	Model *m;
	Effect *e;

	UniformBuffer ubuf;
	mat4 proj;
};

TestApp::TestApp() : App(), r(0.0) {
}

void TestApp::release(void) {
}

int TestApp::init(void) {
	if (!(m = Model::load("cube")))
		return error("cannot load cube object");

	if (!(e = Effect::load("simple")))
		return error("could not load simple effect");
		
	proj.setPerspective(D2R(90.0), width / (float) height, 0.1f, 250.0f);
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
	ubuf.use(0);

	e->apply();
	m->render();
}

App *createApp(void) {
	return new TestApp();
}
