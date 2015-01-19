PREFIX ?= /usr/local
LDLIBS = -lm -lpthread

TARGET = libfilteraudio.so

SRC = $(wildcard */*.c) filter_audio.c
OBJ = $(SRC:.c=.o)
HEADER = filter_audio.h

# Check on which platform we are running
UNAME_M = $(shell uname -m)
ifeq ($(UNAME_M), x86_64)
	LIBDIR = lib64
else
	LIBDIR = lib
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	@echo "  LD    $@"
	@$(CC) $(LDFLAGS) $(LDLIBS) -shared -o $@ $^

%.o: %.c
	@echo "  CC    $@"
	@$(CC) $(CFLAGS) -fPIC -c -o $@ $<

install: all $(HEADER)
	mkdir -p $(DESTDIR)/$(PREFIX)/$(LIBDIR)/pkgconfig
	mkdir -p $(DESTDIR)/$(PREFIX)/include
	install -m755 $(TARGET) $(DESTDIR)/$(PREFIX)/$(LIBDIR)/$(TARGET)
	install -m644 $(HEADER) $(DESTDIR)/$(PREFIX)/include/$(HEADER)
	install -m644 $(HEADER) $(DESTDIR)/$(PREFIX)/$(LIBDIR)/filteraudio.pc
	sed -i'' -e 's:__PREFIX__:'$(PREFIX)':g' $(DESTDIR)/$(PREFIX)/$(LIBDIR)/filteraudio.pc
	sed -i'' -e 's:__LIBDIR__:'$(PREFIX)/$(LIBDIR)':g' $(DESTDIR)/$(PREFIX)/$(LIBDIR)/filteraudio.pc

clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean install
