LOCAL_PATH:= $(call my-dir)


########################
# prepare Eco.System1
# The system library inside the Eco.System1 devkit is named by ABI
# (53595331/53595332/53595333), not by the component CID.
ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
    ECO_SYS_LIB := 00000000000000000000000053595333
else ifeq ($(TARGET_ARCH_ABI),x86_64)
    ECO_SYS_LIB := 00000000000000000000000053595333
else ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    ECO_SYS_LIB := 00000000000000000000000053595332
else ifeq ($(TARGET_ARCH_ABI),x86)
    ECO_SYS_LIB := 00000000000000000000000053595332
else ifeq ($(TARGET_ARCH_ABI),mips64)
    ECO_SYS_LIB := 00000000000000000000000053595332
else ifeq ($(TARGET_ARCH_ABI),armeabi)
    ECO_SYS_LIB := 00000000000000000000000053595331
else ifeq ($(TARGET_ARCH_ABI),mips)
    ECO_SYS_LIB := 00000000000000000000000053595331
else ifeq ($(TARGET_ARCH_ABI),)
    # Parsed outside ndk-build (TARGET_ARCH_ABI is set before this file is
    # included for every configured ABI); leave the selection to the caller.
    ECO_SYS_LIB :=
else
    $(error Unsupported TARGET_ARCH_ABI for Eco.System1: $(TARGET_ARCH_ABI))
endif

include $(CLEAR_VARS)
LOCAL_MODULE    := lib$(ECO_SYS_LIB)
LOCAL_SRC_FILES := $(ECO_FRAMEWORK)/Eco.System1/BuildFiles/Android/$(TARGET_ARCH_ABI)/StaticRelease/lib$(ECO_SYS_LIB).a 
include $(PREBUILT_STATIC_LIBRARY)
########################

########################
# prepare Eco.InterfaceBus1
include $(CLEAR_VARS)
LOCAL_MODULE    := lib00000000000000000000000042757331
LOCAL_SRC_FILES := $(ECO_FRAMEWORK)/Eco.InterfaceBus1/BuildFiles/Android/$(TARGET_ARCH_ABI)/StaticRelease/lib00000000000000000000000042757331.a 
include $(PREBUILT_STATIC_LIBRARY)
########################

########################
# prepare Eco.MemoryManager1
include $(CLEAR_VARS)
LOCAL_MODULE    := lib0000000000000000000000004D656D31
LOCAL_SRC_FILES := $(ECO_FRAMEWORK)/Eco.MemoryManager1/BuildFiles/Android/$(TARGET_ARCH_ABI)/StaticRelease/lib0000000000000000000000004D656D31.a 
include $(PREBUILT_STATIC_LIBRARY)
########################

########################
# prepare Eco.FileSystemManagement1
include $(CLEAR_VARS)
LOCAL_MODULE    := lib00000000000000000000000046534D31
LOCAL_SRC_FILES := $(ECO_FRAMEWORK)/Eco.FileSystemManagement1/BuildFiles/Android/$(TARGET_ARCH_ABI)/StaticRelease/lib00000000000000000000000046534D31.a 
include $(PREBUILT_STATIC_LIBRARY)
########################


########################
# prepare Lab1
include $(CLEAR_VARS)
LOCAL_MODULE    := lib87EC2D103CFF4BB686DEBFCD955C647A
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../../BuildFiles/Android/$(TARGET_ARCH_ABI)/StaticRelease/lib87EC2D103CFF4BB686DEBFCD955C647A.a 
include $(PREBUILT_STATIC_LIBRARY)
########################

#
# Lab1UnitTest
#
include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(ECO_FRAMEWORK)/Eco.Core1/SharedFiles
LOCAL_C_INCLUDES += $(ECO_FRAMEWORK)/Eco.InterfaceBus1/SharedFiles
LOCAL_C_INCLUDES += $(ECO_FRAMEWORK)/Eco.MemoryManager1/SharedFiles
LOCAL_C_INCLUDES += $(ECO_FRAMEWORK)/Eco.FileSystemManagement1/SharedFiles
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../HeaderFiles
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../HeaderFiles/Android
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../SharedFiles
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../UnitTestFiles/HeaderFiles
TARGET_PLATFORM := android-15
TARGET_OUT := $(LOCAL_PATH)/../build/libs/$(TARGET_ARCH_ABI)/$(CONFIGURATION)
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../../UnitTestFiles/SourceFiles/Lab1.c

ifeq ($(TARGET_LINK),0)

LOCAL_MODULE := Lab1UnitTest
LOCAL_CFLAGS := -std=c99 -DECO_LIB -DECO_ANDROID -DUGUID_UTILITY -D__ANDROID__ -DDISABLE_IMPORTGL
LOCAL_LDLIBS := -lc -lm -llog -landroid -latomic -lEGL -lGLESv1_CM
LOCAL_LDFLAGS += -u ANativeActivity_onCreate
LOCAL_STATIC_LIBRARIES := $(ECO_SYS_LIB) 00000000000000000000000042757331 0000000000000000000000004D656D31 00000000000000000000000046534D31 87EC2D103CFF4BB686DEBFCD955C647A
include $(BUILD_SHARED_LIBRARY)

else

LOCAL_MODULE := Lab1UnitTest
LOCAL_CFLAGS := -std=c99 -DECO_DLL -DECO_ANDROID -DUGUID_UTILITY -D__ANDROID__

include $(BUILD_SHARED_LIBRARY)

endif