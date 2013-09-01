## Copyright 2013 Brian Swetland <swetland@frotz.net>
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.

what_to_build:: all

-include local.mk

SDLCFG := /work/sdl2/bin/sdl2-config

SDLFLAGS := $(shell $(SDLCFG) --cflags)
#SDLLIBS := $(shell $(SDLCFG) --static-libs)
SDLLIBS := /work/sdl2/lib/libSDL2.a -lpthread -lrt

HOST_CFLAGS := $(SDLFLAGS) -Wall -g -O2
HOST_CFLAGS += -std=c++0x
HOST_CFLAGS += -Icommon
HOST_CFLAGS += -Ifreetype-2.5.0.1/include
#HOST_CFLAGS += -ffunction-sections -fdata-sections

HOST_LFLAGS := -static-libstdc++
#HOST_LFLAGS += -Wl,--verbose
#HOST_LFLAGS += -Wl,-gc-sections

#GLLIB := /usr/lib/nvidia-experimental-310/libGL.so.310.14
GLLIB := -L/usr/lib -lGL
HOST_LIBS := $(SDLLIBS) $(GLLIB) -lm -lpng

include $(wildcard arch/*/config.mk)

OUT := out
OUT_OBJ := $(OUT)/_obj
OUT_LIB := $(OUT)/_lib

ALL :=
DEPS :=

MKDIR = if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi

QUIET ?= @

modulepath = $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

# additional modules
include $(wildcard */module.mk)

clean::
	@echo clean
	@rm -rf $(OUT)

# we generate .d as a side-effect of compiling. override generic rule:
%.d:
-include $(DEPS)

all:: $(ALL)

