include flavors/vanilla_ios.mk
CFLAGS += -DIOS_RAMDISK=1
LDFLAGS += -framework CoreFoundation -framework IOKit
