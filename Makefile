SYSROOT ?= $(shell xcodebuild -sdk macosx -version Path 2> /dev/null)
ARCH := arm64
OUTPUT_DIR = output
BUILD_DIR = build
EXECUTABLE = $(OUTPUT_DIR)/probe
SRC_DIR = src
CFLAGS := -g -I$(SRC_DIR) -O3 -isysroot $(SYSROOT)
LDFLAGS := -L$(SYSROOT)/usr/lib -lc
SOURCES = $(shell find $(SRC_DIR) -type f ! -name "*.h" ! -name ".*")
OBJS = $(SOURCES:%=$(BUILD_DIR)/%.o)
$(BUILD_DIR)/%.o: %
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -arch $(ARCH) -c $< -o $@

$(EXECUTABLE): $(OBJS)
	mkdir -p $(dir $@)
	ld $(LDFLAGS) $(OBJS) -o $(EXECUTABLE)

clean:
	rm -rf $(BUILD_DIR) $(OUTPUT_DIR)
