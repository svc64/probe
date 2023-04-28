SDK = iphoneos
SYSROOT := $(shell xcodebuild -sdk $(SDK) -version Path 2> /dev/null)
CFLAGS += -miphoneos-version-min=7.0 -isysroot $(SYSROOT) -Iinclude
LDFLAGS += -target arm64-apple-ios7.0 -L$(SYSROOT)/usr/lib -F$(SYSROOT)/System/Library/Frameworks
