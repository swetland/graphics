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
GLXTN PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
GLXTN PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
GLXTN PFNGLBINDTEXTUREPROC glBindTexture;
GLXTN PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
GLXTN PFNGLBLENDFUNCPROC glBlendFunc;
GLXTN PFNGLBUFFERDATAPROC glBufferData;
GLXTN PFNGLCLEARPROC glClear;
GLXTN PFNGLCOMPILESHADERPROC glCompileShader;
GLXTN PFNGLCREATEPROGRAMPROC glCreateProgram;
GLXTN PFNGLCREATESHADERPROC glCreateShader;
GLXTN PFNGLDELETEBUFFERSPROC glDeleteBuffers;
GLXTN PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
GLXTN PFNGLDELETEPROGRAMPROC glDeleteProgram;
GLXTN PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;
GLXTN PFNGLDELETESHADERPROC glDeleteShader;
GLXTN PFNGLDELETETEXTURESPROC glDeleteTextures;
GLXTN PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
GLXTN PFNGLDEPTHMASKPROC glDepthMask;
GLXTN PFNGLDISABLEPROC glDisable;
GLXTN PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
GLXTN PFNGLDRAWARRAYSPROC glDrawArrays;
GLXTN PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced;
GLXTN PFNGLDRAWELEMENTSPROC glDrawElements;
GLXTN PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced;
GLXTN PFNGLENABLEPROC glEnable;
GLXTN PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
GLXTN PFNGLFINISHPROC glFinish;
GLXTN PFNGLFLUSHPROC glFlush;
GLXTN PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
GLXTN PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
GLXTN PFNGLGENBUFFERSPROC glGenBuffers;
GLXTN PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
GLXTN PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
GLXTN PFNGLGENQUERIESPROC glGenQueries;
GLXTN PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
GLXTN PFNGLGENTEXTURESPROC glGenTextures;
GLXTN PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
GLXTN PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
GLXTN PFNGLGETINTEGERVPROC glGetIntegerv;
GLXTN PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
GLXTN PFNGLGETPROGRAMIVPROC glGetProgramiv;
GLXTN PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv;
GLXTN PFNGLGETQUERYOBJECTI64VPROC glGetQueryObjecti64v;
GLXTN PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv;
GLXTN PFNGLGETQUERYOBJECTUI64VPROC glGetQueryObjectui64v;
GLXTN PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
GLXTN PFNGLGETSHADERIVPROC glGetShaderiv;
GLXTN PFNGLGETSTRINGPROC glGetString;
GLXTN PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex;
GLXTN PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
GLXTN PFNGLLINKPROGRAMPROC glLinkProgram;
GLXTN PFNGLQUERYCOUNTERPROC glQueryCounter;
GLXTN PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
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
GLXTN PFNGLVIEWPORTPROC glViewport;
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
	__GLFN__(glBindFramebuffer),
	__GLFN__(glBindRenderbuffer),
	__GLFN__(glBindTexture),
	__GLFN__(glBindVertexArray),
	__GLFN__(glBlendFunc),
	__GLFN__(glBufferData),
	__GLFN__(glClear),
	__GLFN__(glCompileShader),
	__GLFN__(glCreateProgram),
	__GLFN__(glCreateShader),
	__GLFN__(glDeleteBuffers),
	__GLFN__(glDeleteFramebuffers),
	__GLFN__(glDeleteProgram),
	__GLFN__(glDeleteRenderbuffers),
	__GLFN__(glDeleteShader),
	__GLFN__(glDeleteTextures),
	__GLFN__(glDeleteVertexArrays),
	__GLFN__(glDepthMask),
	__GLFN__(glDisable),
	__GLFN__(glDisableVertexAttribArray),
	__GLFN__(glDrawArrays),
	__GLFN__(glDrawArraysInstanced),
	__GLFN__(glDrawElements),
	__GLFN__(glDrawElementsInstanced),
	__GLFN__(glEnable),
	__GLFN__(glEnableVertexAttribArray),
	__GLFN__(glFinish),
	__GLFN__(glFlush),
	__GLFN__(glFramebufferRenderbuffer),
	__GLFN__(glFramebufferTexture2D),
	__GLFN__(glGenBuffers),
	__GLFN__(glGenerateMipmap),
	__GLFN__(glGenFramebuffers),
	__GLFN__(glGenQueries),
	__GLFN__(glGenRenderbuffers),
	__GLFN__(glGenTextures),
	__GLFN__(glGenVertexArrays),
	__GLFN__(glGetAttribLocation),
	__GLFN__(glGetIntegerv),
	__GLFN__(glGetProgramInfoLog),
	__GLFN__(glGetProgramiv),
	__GLFN__(glGetQueryObjectiv),
	__GLFN__(glGetQueryObjecti64v),
	__GLFN__(glGetQueryObjectuiv),
	__GLFN__(glGetQueryObjectui64v),
	__GLFN__(glGetShaderInfoLog),
	__GLFN__(glGetShaderiv),
	__GLFN__(glGetString),
	__GLFN__(glGetUniformBlockIndex),
	__GLFN__(glGetUniformLocation),
	__GLFN__(glLinkProgram),
	__GLFN__(glQueryCounter),
	__GLFN__(glRenderbufferStorage),
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
	__GLFN__(glViewport),
};
#endif
#endif
