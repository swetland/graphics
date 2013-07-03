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

#ifndef _MATRIX_MATH_H_
#define _MATRIX_MATH_H_

#include <string.h>

#define D2R(d) (((d) * M_PI) / 180.0)

/* low level operations */
void __mat4_mul_mat4(float m[16], const float a[16], const float b[16]);
void __mat4_mul_vec4(float o[4], const float m[16], const float v[4]);

void __mat4_set_identity(float m[16]);
void __mat4_set_translate(float m[16], float x, float y, float z);
void __mat4_set_rotate_x(float m[16], float rad);
void __mat4_set_rotate_y(float m[16], float rad);
void __mat4_set_rotate_z(float m[16], float rad);
void __mat4_set_scale(float m[16], float x, float y, float z);

void __mat4_set_perspective(float m[16], float fov, float aspect,
	float znear, float zfar);
void __mat4_set_ortho(float m[16], float left, float right,
	float bottom, float top, float znear, float zfar);

class mat4;

class vec4 {
	float v[4];
public:
	vec4() { };
	vec4(const vec4 &x) {
		v[0] = x[0]; v[1] = x[1]; v[2] = x[2]; v[3] = x[3];
	};
	vec4(const float *raw) {
		memcpy(v, raw, sizeof(float[4]));
	}
	vec4(float a, float b, float c, float d) {
		v[0] = a; v[1] = b; v[2] = c; v[3] = d;
	};
	vec4(float x, float y, float z) {
		v[0] = x; v[1] = y; v[2] = z; v[3] = 1.0;
	};

	/* raw accessor suitable for glSomething4fv() */
	operator const float*() { return v; };

	vec4& operator*=(float n) {
		v[0]*=n; v[1]*=n; v[2]*=n; v[3]*=n;
		return *this;
	}

	/* linear accessors */
	float operator[] (const int n) const { return v[n]; };
	float& operator[] (const int n) { return v[n]; };
	float operator() (const int n) const { return v[n]; };
	float& operator() (const int n) { return v[n]; };

	friend vec4 operator*(const vec4& a, const float b);
	friend vec4 operator/(const vec4& a, const float b);
	friend vec4 operator+(const vec4& a, const vec4& b);
	friend vec4 operator-(const vec4& a, const vec4& b);

	friend vec4 operator*(const mat4& a, const vec4& b);
};

inline vec4 operator*(const vec4& a, const float n) {
	return vec4(a.v[0]*n,a.v[1]*n,a.v[2]*n,a.v[3]*n);
}
inline vec4 operator/(const vec4& a, const float n) {
	return vec4(a.v[0]/n,a.v[1]/n,a.v[2]/n,a.v[3]/n);
}
inline vec4 operator+(const vec4& a, const vec4& b) {
	return vec4(a[0]+b[0],a[1]+b[1],a[2]+b[2],a[3]+b[3]);
}
inline vec4 operator-(const vec4& a, const vec4& b) {
	return vec4(a[0]-b[0],a[1]-b[1],a[2]-b[2],a[3]-b[3]);
}

class mat4 {
	float m[16];
public:
	mat4() { __mat4_set_identity(m); };
	mat4(const float raw[16]) { memcpy(m, raw, sizeof(float[16])); };

	/* raw column-major matrix, suitable for glSomething4fv() */
	operator const float*() { return m; };
	const float* data() { return m; };

	mat4& operator*=(const mat4& b) {
		__mat4_mul_mat4(m, m, b.m);
		return *this;
	}

	friend mat4 operator*(const mat4& a, const mat4& b) {
		mat4 out;
		__mat4_mul_mat4(out.m, a.m, b.m);
		return out;
	}

	/* linear and grid accessors */
	float operator[] (const int n) const { return m[n]; }
	float& operator[] (const int n) { return m[n]; }
	float operator() (const int col, const int row) const { return m[col * 4 + row]; };
	float& operator() (const int col, const int row) { return m[col * 4 + row]; };

	mat4& identity(void) {
		__mat4_set_identity(m);
		return *this;
	};

	mat4& rotateX(float rad) {
		float r[16];
		__mat4_set_rotate_x(r, rad);
		__mat4_mul_mat4(m, m, r);
		return *this;
	};
	mat4& rotateY(float rad) {
		float r[16];
		__mat4_set_rotate_y(r, rad);
		__mat4_mul_mat4(m, m, r);
		return *this;
	};
	mat4& rotateZ(float rad) {
		float r[16];
		__mat4_set_rotate_z(r, rad);
		__mat4_mul_mat4(m, m, r);
		return *this;
	};
	mat4& translate(float x, float y, float z) {
		float t[16];
		__mat4_set_translate(t, x, y, z);
		__mat4_mul_mat4(m, m, t);
		return *this;
	};
	mat4& scale(float x, float y, float z) {
		float t[16];
		__mat4_set_scale(t, x, y, z);
		__mat4_mul_mat4(m, m, t);
		return *this;
	}
	mat4& setOrtho(float left, float right, float bottom, float top, float znear, float zfar) {
		__mat4_set_ortho(m, left, right, bottom, top, znear, zfar);
		return *this;
	};
	mat4& setPerspective(float fov, float aspect, float znear, float zfar) {
		__mat4_set_perspective(m, fov, aspect, znear, zfar);
		return *this;
	};

	friend vec4 operator*(const mat4& a, const vec4& b);

	mat4& mul(mat4& left, mat4& right) {
		__mat4_mul_mat4(m, left.m, right.m);
		return *this;
	}
};

inline vec4 operator*(const mat4& a, const vec4& b) {
	vec4 out;
	__mat4_mul_vec4(out.v, a.m, b.v);
	return out;
}

#endif
