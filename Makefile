CC := gcc
CFLAGS := -std=gnu99 -Wall -Wextra -Werror -pedantic
LDFLAGS := #-lrt -pthread
SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:%.c=bin/%.o)
TARGET := proj2
FUNCS := all clean run-not-clean run test zip

.PHONY: $(FUNCS)

all: $(TARGET)

$(TARGET): $(OBJECTS) | bin
	$(CC) $(CFLAGS) $(OBJECTS) $(LDFLAGS) -o $@

bin/%.o: %.c | bin
	$(CC) $(CFLAGS) -c $< -o $@

bin:
	mkdir -p $@

# clean
clean:
	rm -rf proj2 bin *.zip *.out

FLAGS_1 := -h
FLAGS_2 := a b c d e
FLAGS_3 := -1 -1 -1 -1 -1
FLAGS = $(FLAGS_3)

# run (make run)
run-not-clean: all
	@echo ; echo Running file "\""$(TARGET)"\"" with: $(FLAGS) ; echo
	@./$(TARGET) $(FLAGS)
	@echo

# run (make run clean)
run: run-not-clean
	$(MAKE) clean

# provided test
test: run-not-clean
	@bash extras/kontrola-vystupu.sh < proj2.out
	$(MAKE) clean

# zip
zip:
	zip proj2.zip *.c *.h Makefile