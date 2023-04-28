ifeq ($(OS),Darwin)
SDK := macosx
SYSROOT := $(shell xcodebuild -sdk $(SDK) -version Path 2> /dev/null)
CFLAGS += -isysroot $(SYSROOT) -Iinclude
LDFLAGS += -L$(SYSROOT)/usr/lib -F$(SYSROOT)/System/Library/Frameworks
endif
