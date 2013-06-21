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
M_PATH := $(strip $(M_PATH))

# sanity check
ifeq "$(M_NAME)" ""
$(error No module name specified)
endif
ifeq "$(M_PATH)" ""
$(error No module path specified)
endif

M_OBJS := $(addprefix $(OUT_OBJ)/$(M_PATH)/,$(M_OBJS))
DEPS += $(M_OBJS:%o=%d)

ALL += $(OUT_LIB)/$(M_NAME).a

$(OUT_OBJ)/$(M_PATH)/%.o: _CFLAGS := $(M_CFLAGS)

$(OUT_OBJ)/$(M_PATH)/%.o: $(M_PATH)/%.cc
	@$(MKDIR)
	@echo compile $<
	$(QUIET)g++ $(HOST_CFLAGS) $(_CFLAGS) -c $< -o $@ -MD -MT $@ -MF $(@:%o=%d)

$(OUT_OBJ)/$(M_PATH)/%.o: $(M_PATH)/%.c
	@$(MKDIR)
	@echo compile $<
	$(QUIET)g++ $(HOST_CFLAGS) $(_CFLAGS) -c $< -o $@ -MD -MT $@ -MF $(@:%o=%d)

$(OUT_LIB)/$(M_NAME).a: _OBJS := $(M_OBJS)
$(OUT_LIB)/$(M_NAME).a: $(M_OBJS)
	@$(MKDIR)
	@echo archive $@
	@rm -f $@
	$(QUIET)ar cr $@ $(_OBJS)

$(info module $(M_NAME))

M_LIBS :=
M_OBJS :=
M_NAME :=
M_CLFAGS :=
