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

#ifndef _TEXTGRID_H_
#define _TEXTGRID_H_

class TextGrid {
public:
	int init(App *a, int w, int h);
	void render(App *a);
	void clear(void);
	void printf(int x, int y, const char *fmt, ...);

private:
	int width;
	int height;
	int dirty;

	UniformBuffer ubuf;
	VertexBuffer vtx;
	VertexBuffer cbuf;
	PixelShader ps;
	VertexShader vs;
	Texture2D texture;
	InputConfiguration cfg;

	unsigned char *grid;
};

#endif
