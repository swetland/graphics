/* WARNING - MACHINE GENERATED FILE - DO NOT EDIT */
#ifndef _OPEN_GL_H_
#define _OPEN_GL_H_
#include "glcorearb.h"
#ifndef GLXTN
#define GLXTN extern
#define __NO_GLXTN_TABLE__
#endif
GLXTN PFNGLACTIVETEXTUREPROC glActiveTexture;
GLXTN PFNGLATTACHSHADERPROC glAttachShader;
GLXTN PFNGLBINDBUFFERPROC glBindBuffer;
GLXTN PFNGLBINDBUFFERBASEPROC glBindBufferBase;
GLXTN PFNGLBINDTEXTUREPROC glBindTexture;
GLXTN PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
GLXTN PFNGLBLENDFUNCPROC glBlendFunc;
GLXTN PFNGLBUFFERDATAPROC glBufferData;
GLXTN PFNGLCLEARPROC glClear;
GLXTN PFNGLCOMPILESHADERPROC glCompileShader;
GLXTN PFNGLCREATEPROGRAMPROC glCreateProgram;
GLXTN PFNGLCREATESHADERPROC glCreateShader;
GLXTN PFNGLDELETEBUFFERSPROC glDeleteBuffers;
GLXTN PFNGLDELETEPROGRAMPROC glDeleteProgram;
GLXTN PFNGLDELETESHADERPROC glDeleteShader;
GLXTN PFNGLDELETETEXTURESPROC glDeleteTextures;
GLXTN PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
GLXTN PFNGLDISABLEPROC glDisable;
GLXTN PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
GLXTN PFNGLDRAWARRAYSPROC glDrawArrays;
GLXTN PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced;
GLXTN PFNGLDRAWELEMENTSPROC glDrawElements;
GLXTN PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced;
GLXTN PFNGLENABLEPROC glEnable;
GLXTN PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
GLXTN PFNGLFLUSHPROC glFlush;
GLXTN PFNGLGENBUFFERSPROC glGenBuffers;
GLXTN PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
GLXTN PFNGLGENTEXTURESPROC glGenTextures;
GLXTN PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
GLXTN PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
GLXTN PFNGLGETINTEGERVPROC glGetIntegerv;
GLXTN PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
GLXTN PFNGLGETPROGRAMIVPROC glGetProgramiv;
GLXTN PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
GLXTN PFNGLGETSHADERIVPROC glGetShaderiv;
GLXTN PFNGLGETSTRINGPROC glGetString;
GLXTN PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex;
GLXTN PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
GLXTN PFNGLLINKPROGRAMPROC glLinkProgram;
GLXTN PFNGLSHADERSOURCEPROC glShaderSource;
GLXTN PFNGLTEXBUFFERPROC glTexBuffer;
GLXTN PFNGLTEXIMAGE2DPROC glTexImage2D;
GLXTN PFNGLTEXPARAMETERIPROC glTexParameteri;
GLXTN PFNGLUNIFORM1IPROC glUniform1i;
GLXTN PFNGLUNIFORM4FVPROC glUniform4fv;
GLXTN PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding;
GLXTN PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
GLXTN PFNGLUSEPROGRAMPROC glUseProgram;
GLXTN PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor;
GLXTN PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer;
GLXTN PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
#ifndef __NO_GLXTN_TABLE__
#define __GLFN__(n) { (void**) &n, #n }
struct {
	void **func;
	const char *name;
} fntb[] = {
	__GLFN__(glActiveTexture),
	__GLFN__(glAttachShader),
	__GLFN__(glBindBuffer),
	__GLFN__(glBindBufferBase),
	__GLFN__(glBindTexture),
	__GLFN__(glBindVertexArray),
	__GLFN__(glBlendFunc),
	__GLFN__(glBufferData),
	__GLFN__(glClear),
	__GLFN__(glCompileShader),
	__GLFN__(glCreateProgram),
	__GLFN__(glCreateShader),
	__GLFN__(glDeleteBuffers),
	__GLFN__(glDeleteProgram),
	__GLFN__(glDeleteShader),
	__GLFN__(glDeleteTextures),
	__GLFN__(glDeleteVertexArrays),
	__GLFN__(glDisable),
	__GLFN__(glDisableVertexAttribArray),
	__GLFN__(glDrawArrays),
	__GLFN__(glDrawArraysInstanced),
	__GLFN__(glDrawElements),
	__GLFN__(glDrawElementsInstanced),
	__GLFN__(glEnable),
	__GLFN__(glEnableVertexAttribArray),
	__GLFN__(glFlush),
	__GLFN__(glGenBuffers),
	__GLFN__(glGenerateMipmap),
	__GLFN__(glGenTextures),
	__GLFN__(glGenVertexArrays),
	__GLFN__(glGetAttribLocation),
	__GLFN__(glGetIntegerv),
	__GLFN__(glGetProgramInfoLog),
	__GLFN__(glGetProgramiv),
	__GLFN__(glGetShaderInfoLog),
	__GLFN__(glGetShaderiv),
	__GLFN__(glGetString),
	__GLFN__(glGetUniformBlockIndex),
	__GLFN__(glGetUniformLocation),
	__GLFN__(glLinkProgram),
	__GLFN__(glShaderSource),
	__GLFN__(glTexBuffer),
	__GLFN__(glTexImage2D),
	__GLFN__(glTexParameteri),
	__GLFN__(glUniform1i),
	__GLFN__(glUniform4fv),
	__GLFN__(glUniformBlockBinding),
	__GLFN__(glUniformMatrix4fv),
	__GLFN__(glUseProgram),
	__GLFN__(glVertexAttribDivisor),
	__GLFN__(glVertexAttribIPointer),
	__GLFN__(glVertexAttribPointer),
};
#endif
#endif
