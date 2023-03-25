ARCH := arm64
OUTPUT_DIR = output
BUILD_DIR = build
EXECUTABLE = $(OUTPUT_DIR)/probe
SRC_DIR = src
CFLAGS := -g -I$(SRC_DIR) -O3
LDFLAGS := -lc
OS := $(shell uname -s)
ifeq ($(OS),Darwin)
TARGET ?= macosx
ifeq ($(TARGET),iphoneos)
SYSROOT ?= $(shell xcodebuild -sdk iphoneos -version Path 2> /dev/null)
CFLAGS += -miphoneos-version-min=7.0
else ifeq ($(TARGET),macosx)
SYSROOT ?= $(shell xcodebuild -sdk macosx -version Path 2> /dev/null)
endif
CFLAGS += -isysroot $(SYSROOT)
LDFLAGS += -L$(SYSROOT)/usr/lib
endif
SOURCES = $(shell find $(SRC_DIR) -type f ! -name "*.h" ! -name ".*")
OBJS = $(SOURCES:%=$(BUILD_DIR)/%.o)
$(BUILD_DIR)/%.o: %
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -arch $(ARCH) -c $< -o $@

$(EXECUTABLE): $(OBJS)
	mkdir -p $(dir $@)
	ld $(LDFLAGS) $(OBJS) -o $(EXECUTABLE)
	codesign -fs - $(EXECUTABLE)

clean:
	rm -rf $(BUILD_DIR) $(OUTPUT_DIR)
