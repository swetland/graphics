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

#ifndef _SHARED_H_
#define _SHARED_H_

/* Constants that are shared with shader programs. */
/* Ensure that the textual version below stays in sync. */

#define A_POSITION	0
#define A_NORMAL	1
#define A_TEXCOORD	2

#define U_SCENE		0
#define U_OBJECT	1
#define U_MATERIAL	2

struct ubScene {
	mat4 Ortho;		/* 1:1 2D mapping */
	vec4 OrthoSize;		/* x,y = width,height */
	vec4 LightColor;
	vec4 LightPosition;
};

struct ubObject {
	mat4 mvp;
	mat4 mv;
};

struct ubMaterial {
	vec4 Ambient;
	vec4 Diffuse;
	vec4 Specular;
	float Shininess;
};

#if INCLUDE_SHADER_GLOBALS
static const char *shader_globals = 
"#define A_POSITION 0\n"
"#define A_NORMAL 1\n"
"#define A_TEXCOORD 2\n"
"layout(std140) uniform block0 {\n"
"	mat4 uOrtho;\n"
"	vec4 uOrthoSize;\n"
"	vec4 uLightColor;\n"
"	vec4 uLightPosition;\n"
"};\n"
"layout(std140) uniform block1 {\n"
"	mat4 uMVP;\n"
"	mat4 uMV;\n"
"};\n"
"layout(std140) uniform block2 {\n"
"	vec4 uAmbient;\n"
"	vec4 uDiffuse;\n"
"	vec4 uSpecular;\n"
"	float uShininess;\n"
"};\n"
;
#endif

#endif
