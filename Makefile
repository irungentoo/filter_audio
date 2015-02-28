BASE_NAME = libfilteraudio
VERSION = 0.0.0
PREFIX ?= /usr/local
LIBDIR ?= lib
INCLUDEDIR ?= include

STATIC_LIB = $(BASE_NAME).a
PC_FILE = filteraudio.pc

SRC = $(wildcard */*.c) filter_audio.c
OBJ = $(SRC:.c=.o)
HEADER = filter_audio.h
LDFLAGS += -lm -lpthread
TARGET_ONLY = NO

# Check on which platform we are running
ifeq ($(shell uname), Linux)
    SHARED_EXT = so
    TARGET = $(BASE_NAME).$(SHARED_EXT).$(VERSION)
    SHARED_LIB = $(BASE_NAME).$(SHARED_EXT).$(shell echo $(VERSION) | rev | cut -d "." -f 1 | rev)
    LDFLAGS += -Wl,-soname=$(SHARED_LIB)
else ifeq ($(shell uname), Darwin)
    SHARED_EXT = dylib
    TARGET = $(BASE_NAME).$(VERSION).$(SHARED_EXT)
    SHARED_LIB = $(BASE_NAME).$(shell echo $(VERSION) | rev | cut -d "." -f 1 | rev).$(SHARED_EXT)
    LDFLAGS += -Wl,-install_name,$(SHARED_LIB)
else ifeq ($(shell uname -o), Msys)
    SHARED_EXT = dll
    TARGET = $(BASE_NAME).$(SHARED_EXT)
    TARGET_ONLY = YES
	LDFLAGS += -Wl,--out-implib,$(LINKING_LIB)
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
	mkdir -p $(abspath $(DESTDIR)/$(PREFIX)/$(INCLUDEDIR))
	@echo "Installing $(TARGET)"
	@install -m755 $(TARGET) $(abspath $(DESTDIR)/$(PREFIX)/$(LIBDIR)/$(TARGET))
	@echo "Installing $(HEADER)"
	@install -m644 $(HEADER) $(abspath $(DESTDIR)/$(PREFIX)/$(INCLUDEDIR)/$(HEADER))
	@echo "Installing $(PC_FILE)"
	@install -m644 $(PC_FILE) $(abspath $(DESTDIR)/$(PREFIX)/$(LIBDIR)/pkgconfig/$(PC_FILE))
	@if [ "$(NO_STATIC)" != "1" ]; then \
		echo "Installing $(STATIC_LIB)" ;\
		install -m644 $(STATIC_LIB) $(abspath $(DESTDIR)/$(PREFIX)/$(LIBDIR)/$(STATIC_LIB)) ;\
	fi
	@if [ "$(TARGET_ONLY)" != "YES" ]; then \
    	cd $(abspath $(DESTDIR)/$(PREFIX)/$(LIBDIR)) ;\
		ln -sf $(TARGET) $(SHARED_LIB) ;\
		ln -sf $(SHARED_LIB) $(BASE_NAME).$(SHARED_EXT) ;\
	fi
	@sed -i'' -e 's:__PREFIX__:'$(abspath $(PREFIX))':g' $(abspath $(DESTDIR)/$(PREFIX)/$(LIBDIR)/pkgconfig/$(PC_FILE))
	@sed -i'' -e 's:__LIBDIR__:'$(abspath $(PREFIX)/$(LIBDIR))':g' $(abspath $(DESTDIR)/$(PREFIX)/$(LIBDIR)/pkgconfig/$(PC_FILE))
	@sed -i'' -e 's:__INCLUDEDIR__:'$(abspath $(PREFIX)/$(INCLUDEDIR))':g' $(abspath $(DESTDIR)/$(PREFIX)/$(LIBDIR)/pkgconfig/$(PC_FILE))
	@sed -i'' -e 's:__VERSION__:'$(VERSION)':g' $(abspath $(DESTDIR)/$(PREFIX)/$(LIBDIR)/pkgconfig/$(PC_FILE))

clean:
	rm -f $(TARGET) $(STATIC_LIB) $(OBJ)

.PHONY: all clean install
