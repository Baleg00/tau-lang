SRCDIR := src
OBJDIR := obj
BINDIR := bin

CC := gcc
SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))
EXECUTABLE := $(BINDIR)/tauc

CDEFINES := -DTAU_VERSION="\"0.1.0\""
CFLAGS := -std=c17 -Wall -Wextra -Wpedantic -Werror -Iinc

CDEBUG_DEFINES := -DTAU_DEBUG
CDEBUG_FLAGS := -g -O0

.PHONY: all clean debug

$(shell if not exist $(OBJDIR) mkdir $(OBJDIR))
$(shell if not exist $(BINDIR) mkdir $(BINDIR))

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CDEFINES) $(CFLAGS) -c $< -o $@

clean:
	rmdir $(OBJDIR) /s /q
	rmdir $(BINDIR) /s /q

debug: CDEFINES += $(CDEBUG_DEFINES)
debug: CFLAGS += $(CDEBUG_FLAGS)
debug: $(EXECUTABLE)
