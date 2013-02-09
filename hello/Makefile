
SDLCFG := sdl-config

SDLFLAGS := $(shell $(SDLCFG) --cflags)
SDLLIBS := $(shell $(SDLCFG) --libs)

CFLAGS := $(SDLFLAGS) -DWITH_SDL2=0 -Wall -g -O2
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -std=c++0x
CFLAGS += -I../common

LFLAGS := -static-libstdc++
LFLAGS += -Wl,-gc-sections

CXXFLAGS := $(CFLAGS)

GLLIB := /usr/lib/nvidia-experimental-310/libGL.so.310.14
#LIBS := $(SDLLIBS) -lGL -lm -lpng
LIBS := $(SDLLIBS) $(GLLIB) -lm -lpng

LIBOBJS := ../common/loadfile.o
LIBOBJS += ../common/loadobj.o
LIBOBJS += ../common/loadpng.o
LIBOBJS += ../common/savepng.o
LIBOBJS += ../common/simplexnoise.o
LIBOBJS += ../common/matrix.o
LIBOBJS += ../common/textgrid.o
LIBOBJS += ../common/glapp.o

OBJS := hello.o

all: hello test

hello: $(OBJS) $(LIBOBJS)
	$(CXX) $(LFLAGS) -o hello $(OBJS) $(LIBOBJS) $(LIBS)

test: test.o $(LIBOBJS)
	$(CXX) $(LFLAGS) -o test test.o $(LIBOBJS) $(LIBS)

clean:
	rm -f *.o ../common/*.o hello test
