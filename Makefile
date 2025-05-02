CC := gcc
CFLAGS := -std=gnu99 -Wall -Wextra -Werror -pedantic -g
LDFLAGS := -lrt -pthread
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


PROG_ARGS = 3 3 30 20 10

# run (make run)
run-not-clean: all
	@echo ; echo Running file "\""$(TARGET)"\"" with: $(PROG_ARGS) ; echo
	@./$(TARGET) $(PROG_ARGS)
	@echo

# run (make run clean)
run: run-not-clean
	@echo printing proj2.out ; echo
	@cat proj2.out ; echo
	$(MAKE) clean

# provided test
test: run-not-clean
	@bash extras/kontrola-vystupu.sh < proj2.out ; echo
	$(MAKE) clean

# zip
zip:
	zip proj2.zip *.c *.h Makefile