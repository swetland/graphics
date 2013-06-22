M_PATH := $(modulepath)

M_NAME := mkfont
M_OBJS := mkfont.o
M_LIBS := freetype common

include build/app.mk
