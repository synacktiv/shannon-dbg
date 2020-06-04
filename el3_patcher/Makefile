ifeq ($(ANDROID_NDK_ROOT),)
$(error Error : Set the env variable 'ANDROID_NDK_ROOT' with the path of the Android NDK (version 20))
endif

CC := $(ANDROID_NDK_ROOT)/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android27-clang
LINKER := /system/bin/linker64

DEBUG := -g

CFLAGS := $(DEBUG) -fno-short-enums -fPIE -pie -I./include
CFLAGS += -Wl,-rpath-link=,-dynamic-linker=$(LINKER) -L./lib -fuse-ld=gold -Wall -Wextra

all: el3_patch

el3_patch:
	xxd -i ta/s7/fffffffff0000000000000000000001b.tlbin > include/ta_s7_sem.h
	xxd -i ta/s7/ffffffffd00000000000000000000004.tlbin > include/ta_s7_validator.h
	$(CC) src/patch_el3.c src/tools.c -o bin/patch_el3 $(CFLAGS) -lMcClient

install: all
	adb push bin/patch_el3 /data/local/tmp
