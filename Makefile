CC := gcc
CFLAGS := -std=gnu99 -Wall -Wextra -Werror -pedantic
LDFLAGS := #-lrt -pthread
SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:%.c=bin/%.o)
TARGET := proj2

.PHONY: all clean run zip test run-clean

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

# run (make run)
run: all
	./$(TARGET) $(FLAGS_1)
	@echo

# run (make run clean)
run-clean: run
	$(MAKE) clean

# provided test
test: run
	@bash extras/kontrola-vystupu.sh < proj2.out
	$(MAKE) clean

# zip
zip:
	zip proj2.zip *.c *.h Makefile