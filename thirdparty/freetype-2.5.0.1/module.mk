M_PATH := $(modulepath)

M_NAME := freetype
M_CFLAGS := -DFT2_BUILD_LIBRARY

# base library
M_OBJS := base/ftsystem.o
M_OBJS += base/ftinit.o
M_OBJS += base/ftdebug.o
M_OBJS += base/ftbase.o

# suggested / dependencies
M_OBJS += base/ftbbox.o
M_OBJS += base/ftglyph.o
M_OBJS += base/ftbitmap.o

# font drivers: truetype
M_OBJS += truetype/truetype.o
M_OBJS += sfnt/sfnt.o
M_OBJS += psnames/psnames.o

# rasterizer
M_OBJS += smooth/smooth.o

include build/lib.mk


