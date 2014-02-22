
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig/
BUILD_CFLAGS = -std=gnu99 -g \
  -Wall -Wextra -W \
  -Wbad-function-cast \
  -Waggregate-return -Wcast-align \
  -Wfloat-equal -Wimplicit \
  -Wmissing-declarations -Wmissing-prototypes \
  -Wnested-externs -Wpointer-arith \
  -Wsign-compare -Wchar-subscripts \
  -Wstrict-prototypes -Wshadow \
  -Wformat-security -Wtype-limits \
  -Werror=format-security \
  -Wformat -Wformat=2 -Wmissing-braces \
  -Wreturn-type -Wunused-label \
  -Wcast-qual -Wdisabled-optimization \
  -Wwrite-strings -Wmissing-format-attribute \
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
