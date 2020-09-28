CFLAGS := -Wall -Wextra -pedantic-errors -Iinclude -Isrc -g
LDFLAGS := -lSDL2 -lGL -lGLEW -lm -lcglm

SRCDIR := src
OBJDIR := obj
TARGET := main

# files within SRC dir suffixed .c
SRC := $(wildcard $(SRCDIR)/*.c)
# substitution on SRC replacing .c with .o
OBJ := $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# explicitly state these aren't files
.PHONY = all clean

all: $(TARGET)

run: all
	./$(TARGET)

$(TARGET): $(OBJ)
	$(CC) -g $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR):
	mkdir $@

clean:
	$(RM) -r $(OBJDIR)
