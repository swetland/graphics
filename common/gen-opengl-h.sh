#!/bin/bash

echo '/* WARNING - MACHINE GENERATED FILE - DO NOT EDIT */'
echo '#ifndef _OPEN_GL_H_'
echo '#define _OPEN_GL_H_'
echo '#include "glcorearb.h"'
echo '#ifndef GLXTN'
echo '#define GLXTN extern'
echo '#define __NO_GLXTN_TABLE__'
echo '#endif'
awk '{ print "GLXTN PFN" toupper($1) "PROC " $1 ";" }' < gl-api.txt
echo '#ifndef __NO_GLXTN_TABLE__'
echo '#define __GLFN__(n) { (void**) &n, #n }'
echo 'struct {'
echo '	void **func;'
echo '	const char *name;'
echo '} fntb[] = {'
awk '{ print "\t__GLFN__(" $1 ")," }' < gl-api.txt
echo '};'
echo '#endif'
echo '#endif'
