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

#include "Effect.h"

void Effect::apply(void) {
	pgm.use();
}

int Effect::init(const char *name) {
	std::string vname(name);
	std::string fname(name);
	vname.append(".vertex");
	fname.append(".fragment");
	if (vs.load(vname.c_str()))
		return -1;
	if (ps.load(fname.c_str()))
		return -1;
	if (pgm.link(&vs, &ps))
		return -1;
	return 0;
}

Effect *Effect::load(const char *name) {
	Effect *e = new Effect();
	if (e->init(name)) {
		delete e;
		return nullptr;
	} else {
		return e;
	}
}
