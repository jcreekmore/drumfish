
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig/
BUILD_CFLAGS = -std=gnu99 -g \
			   $(shell PKG_CONFIG_LIBDIR=/usr/local/lib/pkgconfig/ pkg-config --cflags simavr) \
			   $(CFLAGS)
BUILD_LDFLAGS = $(LDFLAGS)
BUILD_LDADD = $(shell PKG_CONFIG_LIBDIR=/usr/local/lib/pkgconfig/ pkg-config --libs simavr) \
			  -pthread -lutil $(LDADD)

SOURCES = $(wildcard *.c)
OBJS = $(SOURCES:.c=.o)
TARGET = drumfish

# Very basic quiet rules
ifneq ($(V),)
	Q=
else
	Q=@
endif

.PHONY: all
all: $(TARGET)

%.o: %.c
	@echo "  CC $@"
	$(Q)$(CC) $(BUILD_CFLAGS) -o $@ -c $<

$(TARGET): $(OBJS)
	@echo "  CCLD $(TARGET)"
	$(Q)$(CC) $(BUILD_LDFLAGS) -o $@ $^ $(BUILD_LDADD)

.PHONY: clean
clean:
	$(Q)rm -f $(OBJS)
	$(Q)rm -f $(TARGET)
