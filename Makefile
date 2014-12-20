PREFIX = /usr/local
LDLIBS = -lm -lpthread

TARGET = libfilteraudio.so

SRC = $(wildcard */*.c) filter_audio.c
OBJ = $(SRC:.c=.o)
HEADER = filter_audio.h

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $(LDLIBS) -shared -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

install: all $(HEADER)
	install -Dm755 $(TARGET) $(DESTDIR)/$(PREFIX)/lib/$(TARGET)
	install -Dm644 $(HEADER) $(DESTDIR)/$(PREFIX)/include/$(HEADER)

clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean install
