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
#include "textgrid.h"
#include "Renderable.h"

struct MenuItem {
	MenuItem *next;
	const char *text;
	void *cookie;
	unsigned x, y;
	unsigned width;
	unsigned t, b, l, r;
};

class BigMenu {
public:
	void addItem(const char *text, void *cookie);
	void layout(unsigned width, unsigned height);
	void update(unsigned x, unsigned y);
	void render(void) { text->render(); }
	static BigMenu *create(TextureFont *font);
private:
	DISALLOW_COPY_AND_ASSIGN(BigMenu);
	BigMenu(TextureFont *_font);
	TextureFont *font;
	Text *text;
	MenuItem *first, *last;
	unsigned count;
};

BigMenu* BigMenu::create(TextureFont *font) {
	return new BigMenu(font);
}

BigMenu::BigMenu(TextureFont *_font)
	: font(_font), first(nullptr), last(nullptr), count(0) {
	text = Text::create(_font);
}

void BigMenu::addItem(const char *text, void *cookie) {
	MenuItem *itm = new MenuItem;
	itm->next = nullptr;
	itm->text = text;
	itm->cookie = cookie;
	itm->width = 0;
	itm->x = 0;
	itm->y = 0;
	if (last)
		last->next = itm;
	else
		first = itm;
	last = itm;
	count++;
}

void BigMenu::layout(unsigned width, unsigned height) {
	unsigned y, skip;
	skip = (font->getMaxHeight() * 5) / 3;
	y = (height / 2) - ((skip * count) / 2) + font->getMaxAscent();
	for (MenuItem *itm = first; itm; itm = itm->next) {
		unsigned iw, ih;
		font->measure(itm->text, &iw, &ih);
		itm->y = y;
		itm->x = (width / 2) - (iw / 2);
		printf("iw %d\n", iw);
		itm->width = iw;
		text->puts(itm->x, itm->y, itm->text);
		y += skip;

		itm->t = itm->y - font->getMaxAscent();
		itm->b = itm->t + font->getMaxHeight();
		itm->l = itm->x;
		itm->r = itm->x + itm->width;
	}
}

void BigMenu::update(unsigned x, unsigned y) {
	text->clear();
	for (MenuItem *itm = first; itm; itm = itm->next) {
		if ((x > itm->l) && (x < itm->r) && (y > itm->t) && (y < itm->b))
			text->setColor(RGBA(0,255,0,255));
		else
			text->setColor(RGBA(255,255,255,255));
		text->puts(itm->x, itm->y, itm->text);
	}
}

class TestApp : public App {
public:
	TestApp();
	int init(void);
	void render(void);

private:
	UniformBuffer obj, mat, scn;
	mat4 proj;

	TextureFont *font;
	BigMenu *menu;
};

TestApp::TestApp() : App() { }

int TestApp::init(void) {
	if (!(font = TextureFont::load("orbitron-bold-72")))
		return error("cannot load font");
	menu = BigMenu::create(font);
	menu->addItem("Start", nullptr);
	menu->addItem("Options", nullptr);
	menu->addItem("Quit", nullptr);
	menu->layout(width, height);
	return 0;
}

void TestApp::render(void) {
	struct ubScene scene;
	struct ubObject object;
	struct ubMaterial material;
	mat4 model, view, tmp;

	scene.Ortho.setOrtho(0, width, height, 0, -1.0, 1.0);
	scene.OrthoSize.set(width, height, 0, 0);
	scene.TextGrid.set(16, 16, 64, 64);
	scn.load(&scene, sizeof(scene));
	scn.use(U_SCENE);

	int mx, my;
	SDL_GetMouseState(&mx,&my);
	menu->update(mx, my);
	menu->render();
}

App *createApp(void) {
	return new TestApp();
}
