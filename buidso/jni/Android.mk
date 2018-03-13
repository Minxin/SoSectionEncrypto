LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := encrypto
LOCAL_SRC_FILES := encrypto.cpp
LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
