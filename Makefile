ARCH := arm64
OUTPUT_DIR = output
BUILD_DIR = build
EXECUTABLE = $(OUTPUT_DIR)/probe
SRC_DIR = src
CFLAGS := -g -I$(SRC_DIR)
LDFLAGS := -lc
FLAVOR := vanilla
CONF := debug
SOURCES = $(shell find $(SRC_DIR) -type f ! -name "*.h" ! -name ".*")
OBJS = $(SOURCES:%=$(BUILD_DIR)/%.o)
OS = $(shell uname -s)

all: $(EXECUTABLE) codesign

$(BUILD_DIR)/%.o: %
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -arch $(ARCH) -c $< -o $@

$(EXECUTABLE): $(OBJS)
	mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) $(OBJS) -o $(EXECUTABLE)

pre_codesign: $(EXECUTABLE)

codesign: $(EXECUTABLE) pre_codesign
ifeq ($(OS),Darwin)
	codesign -fs - $(EXECUTABLE)
endif

clean:
	rm -rf $(BUILD_DIR) $(OUTPUT_DIR)

include flavors/$(FLAVOR).mk
include configs/$(CONF).mk
