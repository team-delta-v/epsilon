TOOLCHAIN ?= ios
EXE = bin

IOS_PLATFORM ?= iphonesimulator
IOS_MIN_VERSION = 8.0
IOS_IDENTIFIER ?= unknown
IOS_PROVISIONNING_PROFILE ?= unknown

BUILD_DIR := $(BUILD_DIR)/$(IOS_PLATFORM)

ifdef ARCH
BUILD_DIR := $(BUILD_DIR)/$(ARCH)
endif
