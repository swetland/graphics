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

#include <math.h>
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
	union {
		float v[4];
		struct {
			float x, y, z, w;
		};
	};
public:
	vec4() { };
	vec4(const vec4 &v) {
		x = v.x; y = v.y; z = v.z; w = v.z;
	};
	vec4(const float *raw) {
		memcpy(v, raw, sizeof(float[4]));
	}
	vec4(float a, float b, float c, float d) {
		x = a; y = b; z = c; w = d;
	};
	vec4(float a, float b, float c) {
		x = a; y = b; z = c; w = 1.0f;
	};
	vec4& set(float a, float b, float c, float d) {
		x = a; y = b; z = c; w = d;
		return *this;
	}
	vec4& set(float a, float b, float c) {
		x = a; y = b; z = c; w = 1.0f;
		return *this;
	}

	/* raw accessor suitable for glSomething4fv() */
	operator const float*() { return v; };

	vec4& operator*=(float n) {
		x *= n; y *= n; z *= n; w *= n;
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

	float length(void) {
		return sqrtf(x*x + y*y + z*z + w*w);
	}
	float dot(const vec4& v) {
		return x*v.x + y*v.y + z*v.z + w*v.w;
	}
	vec4& normalize(void) {
		float n = 1.0f / length();
		x *= n; y *= n; z *= n; w *= n;
		return *this;
	}
};

inline vec4 operator*(const vec4& v, const float n) {
	return vec4(v.x * n, v.y * n, v.z * n, v.w * n);
}
inline vec4 operator/(const vec4& v, const float n) {
	return vec4(v.x / n, v.y / n, v.z / n, v.w / n);
}
inline vec4 operator+(const vec4& a, const vec4& b) {
	return vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}
inline vec4 operator-(const vec4& a, const vec4& b) {
	return vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

class vec3 {
	union {
		float v[3];
		struct {
			float x, y, z;
		};
	};
public:
	vec3() { };
	vec3(const vec3 &v) {
		x = v.x; y = v.y; z = v.z;
	};
	vec3(const float *raw) {
		memcpy(v, raw, sizeof(float[3]));
	}
	vec3(float a, float b, float c) {
		x = a; y = b; z = c;
	};
	void set(float a, float b, float c) {
		x = a; y = b; z = c;
	}

	/* raw accessor suitable for glSomething4fv() */
	operator const float*() { return v; };

	vec3& operator*=(float n) {
		x *= n; y *= n; z *= n;
		return *this;
	}

	/* linear accessors */
	float operator[] (const int n) const { return v[n]; };
	float& operator[] (const int n) { return v[n]; };
	float operator() (const int n) const { return v[n]; };
	float& operator() (const int n) { return v[n]; };

	friend vec3 operator*(const vec3& a, const float b);
	friend vec3 operator/(const vec3& a, const float b);
	friend vec3 operator+(const vec3& a, const vec3& b);
	friend vec3 operator-(const vec3& a, const vec3& b);

	float length(void) {
		return sqrtf(x*x + y*y + z*z);
	}
	float dot(const vec4& v) {
		return x*v.x + y*v.y + z*v.z;
	}
	vec3& normalize(void) {
		float n = 1.0f / length();
		x *= n; y *= n; z *= n;
		return *this;
	}
};

inline vec3 operator*(const vec3& v, const float n) {
	return vec3(v.x * n, v.y * n, v.z * n);
}
inline vec3 operator/(const vec3& v, const float n) {
	return vec3(v.x / n, v.y / n, v.z / n);
}
inline vec3 operator+(const vec3& a, const vec3& b) {
	return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}
inline vec3 operator-(const vec3& a, const vec3& b) {
	return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}
inline vec3 cross(const vec3& p, const vec3& q) {
	return vec3(p.y*q.z-p.z*q.y, p.z*q.x-p.x*q.z, p.x*q.y-p.y*q.x);
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
	mat4& lookAt(const vec3& eye, const vec3& center, const vec3& up); 
	mat4& camera(const vec3& eye, const vec3& center, const vec3& up) {
		lookAt(eye, center, up);
		m[12] = -eye.x;
		m[13] = -eye.y;
		m[14] = -eye.z;
		return *this;
	}
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
