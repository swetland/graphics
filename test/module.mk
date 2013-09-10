M_PATH := $(modulepath)

M_NAME := object
M_OBJS := object.o
M_LIBS := common

include build/app.mk

M_NAME := menu
M_OBJS := menu.o
M_LIBS := common

include build/app.mk
