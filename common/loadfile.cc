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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

void *load_file(const char *fn, unsigned *_sz) {
	void *data = 0;
	long sz;
	FILE *fp;

	if(!(fp = fopen(fn, "rb")))
		goto exit;

	if (fseek(fp, 0, SEEK_END))
		goto close_and_exit;

	if ((sz = ftell(fp)) < 0)
		goto close_and_exit;

	if (fseek(fp, 0, SEEK_SET))
		goto close_and_exit;

	if (!(data = malloc(sz + 1)))
		goto close_and_exit;

	if (fread(data, sz, 1, fp) != 1) {
		free(data);
		data = 0;
	} else {
		((char*) data)[sz] = 0;
		if (_sz)
			*_sz = sz;
	}

close_and_exit:
	fclose(fp);
exit:
	if (!data)
		fprintf(stderr, "failed to load '%s'\n", fn);
	return data;
}
