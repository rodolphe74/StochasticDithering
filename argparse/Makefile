# Defaults
CFLAGS ?= -O3 -g -ggdb
LDFLAGS ?=
CROSS_COMPILE =

BASIC_CFLAGS = -Wall -Wextra -fPIC
BASIC_LDFLAGS = 

# We use ALL_* variants
ALL_CFLAGS = $(BASIC_CFLAGS) $(CFLAGS)
ALL_LDFLAGS = $(BASIC_LDFLAGS) $(LDFLAGS)

LIBNAME=libargparse

DYLIBSUFFIX=so
STLIBSUFFIX=a
DYLIBNAME=$(LIBNAME).$(DYLIBSUFFIX)
DYLIB_MAKE_CMD=$(CROSS_COMPILE)gcc -shared -o $(DYLIBNAME) $(ALL_LDFLAGS)
STLIBNAME=$(LIBNAME).$(STLIBSUFFIX)
STLIB_MAKE_CMD=$(CROSS_COMPILE)ar rcs $(STLIBNAME)

# Platform-specific overrides
uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')
ifeq ($(uname_S),Darwin)
DYLIBSUFFIX=dylib
DYLIB_MAKE_CMD=$(CROSS_COMPILE)gcc -shared -o $(DYLIBNAME) $(ALL_LDFLAGS)
endif

all: $(DYLIBNAME) $(STLIBNAME)

OBJS += argparse.o

$(OBJS): %.o: %.c argparse.h
	$(CROSS_COMPILE)gcc -o $*.o -c $(ALL_CFLAGS) $<

$(DYLIBNAME): argparse.o
	$(DYLIB_MAKE_CMD) $^

$(STLIBNAME): argparse.o
	$(STLIB_MAKE_CMD) $^

test: 
	make -C tests/ test

clean:
	rm -rf *.[ao]
	rm -rf *.so
	rm -rf *.dylib
	make -C tests/ clean
