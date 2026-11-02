BIN      := galaxyctl
BUILDDIR := build
OUTDIR   := output

PREFIX   ?= /usr/local
BINDIR   ?= $(PREFIX)/bin

CC       ?= cc
CFLAGS   ?= -O2 -g
CFLAGS   += -std=c11 -Wall -Wextra -Wpedantic
CPPFLAGS += -Isrc -Isrc/commands -MMD -MP

# All sources under src/
SRC := $(wildcard src/*.c src/commands/*.c src/commands/*/*.c)
OBJ := $(SRC:src/%.c=$(BUILDDIR)/%.o)
DEP := $(OBJ:.o=.d)

.PHONY: all clean install uninstall

all: $(OUTDIR)/$(BIN)

$(OUTDIR)/$(BIN): $(OBJ)
	@mkdir -p $(OUTDIR)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BUILDDIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

install: $(OUTDIR)/$(BIN)
	install -Dm755 $(OUTDIR)/$(BIN) $(DESTDIR)$(BINDIR)/$(BIN)

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(BIN)

clean:
	rm -rf $(BUILDDIR) $(OUTDIR)

-include $(DEP)
