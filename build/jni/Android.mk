LOCAL_PATH := $(call my-dir)

CORE_DIR := $(LOCAL_PATH)/../..

INCLUDES    :=
SOURCES_C   :=
SOURCES_CXX :=

include $(CORE_DIR)/build/Makefile.common
SOURCES_C := $(filter-out %/version.c, $(SOURCES_C))

COREFLAGS := -D__LIBRETRO__ $(RETRODEFS) $(INCLUDES)

GIT_VERSION := " $(shell git rev-parse --short HEAD || echo unknown)"
ifneq ($(GIT_VERSION)," unknown")
  COREFLAGS += -DGIT_VERSION=\"$(GIT_VERSION)\"
endif

include $(CLEAR_VARS)
LOCAL_MODULE    := retro
LOCAL_SRC_FILES := $(SOURCES_C) $(SOURCES_CXX)
LOCAL_CPPFLAGS  := -std=c++11 $(COREFLAGS)
LOCAL_CFLAGS    := $(COREFLAGS)
LOCAL_LDFLAGS   := -Wl,-version-script=$(CORE_DIR)/build/link.T
include $(BUILD_SHARED_LIBRARY)
