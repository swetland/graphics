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

M_NAME := $(strip $(M_NAME))

# sanity check
ifeq "$(M_NAME)" ""
$(error No module name specified)
endif

M_OBJS := $(addprefix $(OUT_OBJ)/$(M_NAME)/,$(M_OBJS))
DEPS += $(M_OBJS:%o=%d)

ASSETS := $(wildcard $(M_NAME)/assets/*) 
ASSETS += $(patsubst common/%,$(M_NAME)/%,$(wildcard common/assets/*))
ASSETS := $(addprefix $(OUT)/,$(ASSETS))

$(OUT)/$(M_NAME)/assets/%: $(M_NAME)/assets/%
	@$(MKDIR)
	cp $< $@ 

$(OUT)/$(M_NAME)/assets/%: common/assets/%
	@$(MKDIR)
	cp $< $@ 

M_LIBS := $(addprefix $(OUT_LIB)/,$(M_LIBS))
M_LIBS := $(addsuffix .a,$(M_LIBS))

ALL += $(OUT)/$(M_NAME)/$(M_NAME)

$(OUT_OBJ)/$(M_NAME)/%.o: $(M_NAME)/%.cc
	@$(MKDIR)
	@echo compile $<
	$(QUIET)g++ $(HOST_CFLAGS) -c $< -o $@ -MD -MT $@ -MF $(@:%o=%d)

$(OUT)/$(M_NAME)/$(M_NAME): _OBJS := $(M_OBJS)
$(OUT)/$(M_NAME)/$(M_NAME): _LIBS := $(M_LIBS)
$(OUT)/$(M_NAME)/$(M_NAME): $(M_OBJS) $(M_LIBS) $(ASSETS)
	@$(MKDIR)
	@echo link $@
	$(QUIET)g++ $(HOST_CFLAGS) -o $@ $(_OBJS) $(_LIBS) $(HOST_LIBS)

$(info module $(M_NAME))

M_LIBS :=
M_OBJS :=
M_NAME :=

