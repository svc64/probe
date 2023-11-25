CFLAGS += -O3

pre_codesign: $(EXECUTABLE)
	strip -xS output/probe
