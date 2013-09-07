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

#ifndef _APP_H_
#define _APP_H_

#include "core.h"

class App {
public:
	App();
	virtual ~App();

	virtual int init(void) = 0;
	virtual void render(void) = 0;
	virtual void release(void) {};
	virtual void onKeyDown(unsigned code) {};
	virtual void onKeyUp(unsigned code) {};

	int getWidth(void) { return width; }
	int getHeight(void) { return height; }

	int keydown(unsigned code) {
		return keystate[code >> 5] & (1 << (code & 0x1f));
	}

	static int __main(int argc, char **argv);

protected:
	int width;
	int height;

	/* mouse motion since last frame */
	int mouseDX, mouseDY, mouseDZ;
	/* mouse button state */
	int mouseBTN;
	/* mouse position in window coordinates */
	int mouseWX, mouseWY;

	/* keys down */
	unsigned keystate[16];
	int fps;

private:
	int start(void);
	void handleEvents(void);
	void setOptions(int argc, char **argv);

	int _vsync;
	int _fullscreen;
	SDL_Window *win;
	SDL_GLContext glcontext;
};

App *createApp(void);

void printx(const char *fmt, ...);
void printmtx(float *m, const char *name);
int error(const char *fmt, ...);

#endif
