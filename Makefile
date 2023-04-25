ARCH := arm64
OUTPUT_DIR = output
BUILD_DIR = build
EXECUTABLE = $(OUTPUT_DIR)/probe
SRC_DIR = src
CFLAGS := -g -I$(SRC_DIR) -O3
OS := $(shell uname -s)
ifeq ($(OS),Darwin)
SDK ?= macosx
ifeq ($(SDK),iphoneos)
CFLAGS += -miphoneos-version-min=7.0 -DIOS=1
LDFLAGS += -target arm64-apple-ios7.0 -framework CoreFoundation -framework IOKit
else ifeq ($(SDK),watchos)
CFLAGS += -mwatchos-version-min=1.0 -DWATCHOS=1
LDFLAGS += -target arm64-apple-watchos1.0
endif
SYSROOT ?= $(shell xcodebuild -sdk $(SDK) -version Path 2> /dev/null)
CFLAGS += -isysroot $(SYSROOT) -Iinclude
LDFLAGS += -L$(SYSROOT)/usr/lib -F$(SYSROOT)/System/Library/Frameworks
endif
SOURCES = $(shell find $(SRC_DIR) -type f ! -name "*.h" ! -name ".*")
OBJS = $(SOURCES:%=$(BUILD_DIR)/%.o)
$(BUILD_DIR)/%.o: %
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -arch $(ARCH) -c $< -o $@

$(EXECUTABLE): $(OBJS)
	mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) $(OBJS) -o $(EXECUTABLE)
	codesign -fs - $(EXECUTABLE)

clean:
	rm -rf $(BUILD_DIR) $(OUTPUT_DIR)
