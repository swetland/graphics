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
	int init(App *a, ID3D10Device *device, int w, int h);
	void render(App *a, ID3D10Device *device);
	void clear(void);
	void printf(int x, int y, const char *fmt, ...);

private:
	int width;
	int height;
	int dirty;

	ID3D10InputLayout *layout;
	ID3D10Buffer *vtxbuf;
	ID3D10Buffer *charbuf;
	ID3D10PixelShader *PS;
	ID3D10VertexShader *VS;
	ID3D10ShaderResourceView *texture;
	ID3D10Buffer *cbuf;

	unsigned char *grid;

	UINT stride[2];
	UINT offset[2];
	ID3D10Buffer *buffer[2];
};

#endif