BASE_NAME = libfilteraudio
VERSION = 0.0.0
PREFIX ?= /usr/local

TARGET = $(BASE_NAME).so.$(VERSION)
SHARED_LIB = $(BASE_NAME).so.$(shell echo $(VERSION) | rev | cut -d "." -f 1 | rev)
STATIC_LIB = $(BASE_NAME).a
PC_FILE = filteraudio.pc

SRC = $(wildcard */*.c) filter_audio.c
OBJ = $(SRC:.c=.o)
HEADER = filter_audio.h
LDFLAGS += -lm -lpthread -Wl,-soname=$(SHARED_LIB)

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
	@$(CC) $(LDFLAGS) -shared -o $@ $^
	@if [ "$(NO_STATIC)" != "1" ]; then \
		echo "  AR    $(STATIC_LIB)" ;\
		ar rcs $(STATIC_LIB) $(OBJ) ;\
	fi

%.o: %.c
	@echo "  CC    $@"
	@$(CC) $(CFLAGS) -fPIC -c -o $@ $<

install: all $(HEADER) $(PC_FILE)
	mkdir -p $(abspath $(DESTDIR)/$(PREFIX)/$(LIBDIR)/pkgconfig)
	mkdir -p $(abspath $(DESTDIR)/$(PREFIX)/include)
	@echo "Installing $(TARGET)"
	@install -m755 $(TARGET) $(abspath $(DESTDIR)/$(PREFIX)/$(LIBDIR)/$(TARGET))
	@echo "Installing $(HEADER)"
	@install -m644 $(HEADER) $(abspath $(DESTDIR)/$(PREFIX)/include/$(HEADER))
	@echo "Installing $(PC_FILE)"
	@install -m644 $(PC_FILE) $(abspath $(DESTDIR)/$(PREFIX)/$(LIBDIR)/pkgconfig/$(PC_FILE))
	@sed -i'' -e 's:__PREFIX__:'$(abspath $(PREFIX))':g' $(abspath $(DESTDIR)/$(PREFIX)/$(LIBDIR)/pkgconfig/$(PC_FILE))
	@sed -i'' -e 's:__LIBDIR__:'$(abspath $(PREFIX)/$(LIBDIR))':g' $(abspath $(DESTDIR)/$(PREFIX)/$(LIBDIR)/pkgconfig/$(PC_FILE))
	@sed -i'' -e 's:__VERSION__:'$(VERSION)':g' $(abspath $(DESTDIR)/$(PREFIX)/$(LIBDIR)/pkgconfig/$(PC_FILE))
	@cd $(abspath $(DESTDIR)/$(PREFIX)/$(LIBDIR)) ; ln -sf $(TARGET) $(SHARED_LIB) ; ln -sf $(SHARED_LIB) $(BASE_NAME).so
	@if [ "$(NO_STATIC)" != "1" ]; then \
		echo "Installing $(STATIC_LIB)" ;\
		install -m644 $(STATIC_LIB) $(abspath $(DESTDIR)/$(PREFIX)/$(LIBDIR)/$(STATIC_LIB)) ;\
	fi

clean:
	rm -f $(TARGET) $(STATIC_LIB) $(OBJ)

.PHONY: all clean install
