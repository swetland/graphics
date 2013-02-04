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
#include <string.h>
#include <math.h>

#include "matrix.h"

/* m = a * b   m and b must not point to the same matrix */
void __mat4_mul_mat4(float m[16], const float a[16], const float b[16]) {
	for (int i = 0; i < 16; i += 4) {
		float ai0 = a[i+0], ai1 = a[i+1], ai2 = a[i+2], ai3 = a[i+3];
		m[i+0] = ai0*b[0] + ai1*b[4] + ai2*b[8]  + ai3*b[12];
		m[i+1] = ai0*b[1] + ai1*b[5] + ai2*b[9]  + ai3*b[13];
		m[i+2] = ai0*b[2] + ai1*b[6] + ai2*b[10] + ai3*b[14];
		m[i+3] = ai0*b[3] + ai1*b[7] + ai2*b[11] + ai3*b[15];
	}
}

void __mat4_mul_vec4(float o[4], const float m[16], const float v[4]) {
	float v0=v[0], v1=v[1], v2=v[2], v3=v[3];
	o[0] = m[0]*v0 + m[4]*v1 + m[8]*v2  + m[12]*v3;
	o[1] = m[1]*v0 + m[5]*v1 + m[9]*v2  + m[13]*v3;
	o[2] = m[2]*v0 + m[6]*v1 + m[10]*v2 + m[14]*v3;
	o[3] = m[3]*v0 + m[7]*v1 + m[11]*v2 + m[15]*v3;
}

void __mat4_set_identity(float m[16]) {
	memset(m, 0, sizeof(float[16]));
	m[0] = m[5] = m[10] = m[15] = 1.0f;
}

void __mat4_set_translate(float m[16], float x, float y, float z) {
	memset(m, 0, sizeof(float[16]));
	m[0] = 1.0;
	m[5] = 1.0;
	m[10] = 1.0;
	m[12] = x;
	m[13] = y;
	m[14] = z;
	m[15] = 1.0;
}

void __mat4_set_scale(float m[16], float x, float y, float z) {
	memset(m, 0, sizeof(float[16]));
	m[0] = x;
	m[5] = y;
	m[10] = z;
	m[15] = 1.0;
}

void __mat4_set_rotate_x(float m[16], float rad) {
	float s = sinf(rad);
	float c = cosf(rad);
	memset(m, 0, sizeof(float[16]));
	m[0] = 1.0;
	m[5] = c;
	m[6] = s;
	m[9] = -s;
	m[10] = c;
	m[15] = 1.0;
}

void __mat4_set_rotate_y(float m[16], float rad) {
	float s = sinf(rad);
	float c = cosf(rad);
	memset(m, 0, sizeof(float[16]));
	m[0] = c;
	m[2] = -s;
	m[5] = 1.0;
	m[8] = s;
	m[10] = c;
	m[15] = 1.0;
}

void __mat4_set_rotate_z(float m[16], float rad) {
	float s = sinf(rad);
	float c = cosf(rad);
	memset(m, 0, sizeof(float[16]));
	m[0] = c;
	m[1] = s;
	m[4] = -s;
	m[5] = c;
	m[10] = 1.0;
	m[15] = 1.0;
}

/* D3D RH Style */
void __mat4_set_perspective(float m[16], float fov, float a, float zn, float zf) {
	memset(m, 0, sizeof(float[16]));
	float ys = 1.0 / tanf(fov / 2.0);
	float xs = ys / a;
	m[0] = xs;
	m[5] = ys;
	m[10] = zf / (zn - zf);
	m[11] = -1.0;
	m[14] = (zn * zf) / (zn - zf);
}

void __mat4_set_ortho(float m[16], float l, float r, float b, float t, float n, float f) {
	memset(m, 0, sizeof(float[16]));
	m[0] = 2.0 / (r - l);
	m[5] = 2.0 / (t - b);
	m[10] = 1.0 / (n - f);
	m[12] = (l + r) / (l - r);
	m[13] = (t + b) / (b - t);
	m[14] = n / (n - f);
	m[15] = 1.0;
}

