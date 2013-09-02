M_PATH := $(modulepath)

M_NAME := common
M_OBJS := glapp.o
M_OBJS += shaders.o
M_OBJS += buffers.o
M_OBJS += io.o
M_OBJS += loadfile.o
M_OBJS += loadpng.o
M_OBJS += loadobj.o
M_OBJS += matrix.o
M_OBJS += savepng.o
M_OBJS += simplexnoise.o
M_OBJS += textgrid.o
M_OBJS += texturefont.o
M_OBJS += Model.o
M_OBJS += Effect.o
M_LIBS := common

include build/lib.mk
