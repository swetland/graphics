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

#include "stringutils.h"

#include "Effect.h"

void Effect::apply(void) {
	pgm.use();
}

int Effect::init(const char *name) {
	const char *x = strchr(name, '+');
	if (x) {
		int len = x - name;
		string128 sname(stringptr(name, len));
		string1024 defines;
		x++;
		while (*x) {
			const char *n = x;
			x = strchr(n, '+');
			defines.append("#define ");
			if (x) {
				defines.append(n, x-n);
				defines.append("\n");
				x++;
			} else {
				defines.append(n);
				defines.append("\n");
				break;
			}
		}
		sname.append(".vertex");
		if (vs.load(sname, defines))
			return -1;
		sname.trim(len).append(".fragment");
		if (ps.load(sname, defines))
			return -1;
	} else {
		string128 sname(name);
		int len = sname.length();
		sname.append(".vertex");
		if (vs.load(sname))
			return -1;
		sname.trim(len).append(".fragment");
		if (ps.load(sname))
			return -1;
	}
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
