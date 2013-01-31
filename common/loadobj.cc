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

#include <vector>
using std::vector;

#include "util.h"

struct v3 {
	float x;
	float y;
	float z;
};

struct v2 {
	float u;
	float v;
};

struct i3 {
	unsigned v;
	unsigned vt;
	unsigned vn;
};

struct obj {
	vector<v3> vertices;
	vector<v3> normals;
	vector<v2> texcoords;
	vector<i3> triangles;
};

struct obj *load_obj(const char *fn) {
	char buf[128];
	FILE *fp;
	struct obj *o = 0;

	if (!(fp = fopen(fn, "r")))
		goto exit;

	o = new(obj);
	if (!o)
		goto close_and_exit;

	while (fgets(buf, sizeof(buf), fp) != 0) {
		if (!strncmp(buf, "v ", 2)) {
			v3 t;
			sscanf(buf + 2, "%f %f %f", &t.x, &t.y, &t.z);
			o->vertices.push_back(t);
		} else if (!strncmp(buf, "vn ", 3)) {
			v3 t;
			sscanf(buf + 3, "%f %f %f", &t.x, &t.y, &t.z);
			o->normals.push_back(t);
		} else if (!strncmp(buf, "vt ", 3)) {
			v2 t;
			sscanf(buf + 3, "%f %f", &t.u, &t.v);
			o->texcoords.push_back(t);
		} else if (!strncmp(buf, "f ", 2)) {
			i3 t;
			char *tmp = buf + 2;
			/* XXX: handle non-triangles */
			while (sscanf(tmp, "%d/%d/%d", &t.v, &t.vt, &t.vn) == 3) {
				t.v--;
				t.vt--;
				t.vn--;
				o->triangles.push_back(t);
				tmp = strstr(tmp, " ");
				if (!tmp) break;
				tmp++;
			}
		} else {
//			fprintf(stderr,"ignoring: %s", buf);
		}
	}

close_and_exit:
	fclose(fp);
exit:
	return o;	
}

static struct model *obj_to_model(struct obj *o) {
	int i, j, n;
	struct model *m;
	int count = o->triangles.size();

	if(!(m = (model*) malloc(sizeof(struct model))))
		return 0;
	memset(m, 0, sizeof(struct model));

	if (!(m->vdata = (float*) malloc(sizeof(float) * 8 * count))) {
		free(m);
		return 0;
	}
	if (!(m->idx = (unsigned short *) malloc(sizeof(short) * count))) {
		free(m->vdata);
		free(m);
		return 0;
	}

	for (n = 0, i = 0; i < count; i++) {
		i3 &tri = o->triangles[i];
		float data[8];
		v3 &vrt = o->vertices[tri.v];
		v3 &nrm = o->normals[tri.vn];
		v2 &txc = o->texcoords[tri.vt];
		data[0] = vrt.x;
		data[1] = vrt.y;
		data[2] = vrt.z;
		data[3] = nrm.x;
		data[4] = nrm.y;
		data[5] = nrm.z;
		data[6] = txc.u;
		data[7] = txc.v;
		for (j = 0; j < n; j++)
			if (!memcmp(data, &m->vdata[8 * j], sizeof(float) * 8))
				goto found_it;
		memcpy(&m->vdata[8 * j], data, sizeof(float) * 8);
		n++;
found_it:
		m->idx[i] = j;
	}

	m->vcount = n;
	m->icount = count;

	return m;
}

#if 0
void model_dump(struct model *m, FILE *fp) {
	int i;
	fprintf(fp, "struct model M = {\n");
	fprintf(fp, "  .vdata = {\n");
	for (i = 0; i < m->vcount; i++)
		fprintf(fp, "    %f, %f, %f,\n    %f, %f, %f, %f, %f,\n",
			m->vdata[i*8+0], m->vdata[i*8+1],
			m->vdata[i*8+2], m->vdata[i*8+3], 
			m->vdata[i*8+4], m->vdata[i*8+5],
			m->vdata[i*8+6], m->vdata[i*8+7]);
	fprintf(fp, "  },\n  .idx[] = {\n");
	for (i = 0; i < m->icount; i += 3) 
		fprintf(fp, "    %d, %d, %d,\n",
			m->idx[i+0], m->idx[i+1], m->idx[i+2]);
	fprintf(fp, "  },\n  .vcount = %d,\n  .icount = %d,\n};\n",
		m->vcount, m->icount);
}
#endif

struct model *load_wavefront_obj(const char *fn) {
	struct obj *o;
	struct model *m;
	o = load_obj(fn);
	if (!o)
		return 0;
	m = obj_to_model(o);
	delete o;
	return m;
} 

