
bin_PROGRAMS =

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
  -Wwrite-strings -Wmissing-format-attribute
BUILD_CFLAGS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags simavr)
BUILD_CFLAGS += $(CFLAGS)

# Rules to build drumfish
bin_PROGRAMS += drumfish
drumfish_SOURCES = drumfish.c flash.c m128rfa1.c uart_pty.c df_log.c
drumfish_OBJS = $(drumfish_SOURCES:.c=.o)
drumfish_LDFLAGS = $(LDFLAGS)
drumfish_LDADD = $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs simavr)
drumfish_LDADD += -pthread -lutil $(LDADD)

# Very basic quiet rules
ifneq ($(V),)
	Q=
else
	Q=@
endif

.PHONY: all
all: $(bin_PROGRAMS)

%.o: %.c
	@echo "  CC $@"
	$(Q)$(CC) $(BUILD_CFLAGS) -o $@ -c $<

drumfish: $(drumfish_OBJS)
	@echo "  CCLD $@"
	$(Q)$(CC) $($@_LDFLAGS) -o $@ $^ $($@_LDADD)

.PHONY: clean
clean:
	$(Q)rm -f $(drumfish_OBJS)
	$(Q)rm -f $(bin_PROGRAMS)
