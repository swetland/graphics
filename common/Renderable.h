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

#ifndef _RENDERABLE_H_
#define _RENDERABLE_H_

#include "core.h"

class Renderable {
public:
	Renderable(void) {};
	virtual void render(void) = 0;

	static Renderable* createXZgrid(float size, unsigned steps);
	static Renderable* createFullscreenQuad(void);

private:
	DISALLOW_COPY_AND_ASSIGN(Renderable);
};

#endif
