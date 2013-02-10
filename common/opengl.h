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

#ifndef _OPEN_GL_H_
#define _OPEN_GL_H_

#include "glcorearb.h"

#ifndef GLUE_DEFINE_EXTENSIONS
#define GLXTN extern
#else
#define GLXTN
#endif

GLXTN PFNGLACTIVETEXTUREPROC glActiveTexture;
GLXTN PFNGLATTACHSHADERPROC glAttachShader;
GLXTN PFNGLBINDBUFFERPROC glBindBuffer;
GLXTN PFNGLBINDBUFFERBASEPROC glBindBufferBase;
GLXTN PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
GLXTN PFNGLBUFFERDATAPROC glBufferData;
GLXTN PFNGLCOMPILESHADERPROC glCompileShader;
GLXTN PFNGLCREATEPROGRAMPROC glCreateProgram;
GLXTN PFNGLCREATESHADERPROC glCreateShader;
GLXTN PFNGLDELETEBUFFERSPROC glDeleteBuffers;
GLXTN PFNGLDELETEPROGRAMPROC glDeleteProgram;
GLXTN PFNGLDELETESHADERPROC glDeleteShader;
GLXTN PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
GLXTN PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
GLXTN PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced;
GLXTN PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced;
GLXTN PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
GLXTN PFNGLGENBUFFERSPROC glGenBuffers;
GLXTN PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
GLXTN PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
GLXTN PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
GLXTN PFNGLGETPROGRAMIVPROC glGetProgramiv;
GLXTN PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
GLXTN PFNGLGETSHADERIVPROC glGetShaderiv;
GLXTN PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
GLXTN PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
GLXTN PFNGLLINKPROGRAMPROC glLinkProgram;
GLXTN PFNGLSHADERSOURCEPROC glShaderSource;
GLXTN PFNGLUNIFORM1IPROC glUniform1i;
GLXTN PFNGLUNIFORM4FVPROC glUniform4fv;
GLXTN PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
GLXTN PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding;
GLXTN PFNGLUSEPROGRAMPROC glUseProgram;
GLXTN PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor;
GLXTN PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
GLXTN PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer;

#ifdef GLUE_DEFINE_EXTENSIONS
#define EFUNC(n) { (void**) &n, #n }
struct {
	void **func;
	const char *name;
} fntb[] = {
	EFUNC(glActiveTexture), EFUNC(glAttachShader), EFUNC(glBindBuffer),
	EFUNC(glBindBufferBase), EFUNC(glBindVertexArray), EFUNC(glBufferData),
	EFUNC(glCompileShader), EFUNC(glCreateProgram), EFUNC(glCreateShader),
	EFUNC(glDeleteBuffers), EFUNC(glDeleteProgram), EFUNC(glDeleteShader),
	EFUNC(glDeleteVertexArrays), EFUNC(glDisableVertexAttribArray),
	EFUNC(glDrawArraysInstanced), EFUNC(glDrawElementsInstanced),
	EFUNC(glEnableVertexAttribArray), EFUNC(glGenBuffers), EFUNC(glGenerateMipmap),
	EFUNC(glGenVertexArrays), EFUNC(glGetAttribLocation), EFUNC(glGetProgramiv),
	EFUNC(glGetProgramInfoLog), EFUNC(glGetShaderiv), EFUNC(glGetShaderInfoLog),
	EFUNC(glGetUniformLocation), EFUNC(glLinkProgram), EFUNC(glShaderSource),
	EFUNC(glUniform1i), EFUNC(glUniform4fv), EFUNC(glUniformMatrix4fv), 
	EFUNC(glUniformBlockBinding), EFUNC(glUseProgram), EFUNC(glVertexAttribDivisor),
	EFUNC(glVertexAttribPointer), EFUNC(glVertexAttribIPointer),
};
#endif
#endif

