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

#ifndef _MODEL_H_
#define _MODEL_H_

#include "Renderable.h"

class Model : public Renderable {
public:
	virtual void render(void);
	static Model *load(const char *name);
private:
	int init(const char *name);
	Model() : Renderable() {};
	//DISALLOW_COPY_AND_ASSIGN(Model);

	IndexBuffer ibuf;
	VertexBuffer vbuf;
	VertexAttributes attr;
	Texture2D texture;
	int icount;
};

#endif
