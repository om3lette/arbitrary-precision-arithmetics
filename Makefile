CC=g++
CFLAGS=-c --std=c++23 -Wall -Wextra -Werror -Wundef -pedantic
LDFLAGS=

BUILD_PATH=build
SRC_PATH=src

BUILD ?= debug

ifeq ($(BUILD), release)
	CFLAGS += -O3 -DNDEBUG
else
	CFLAGS += -g -Og
endif

COMPILE = $(CC) $(CFLAGS)
LINK = $(CC) $(LDFLAGS)

pi: link-pi
	bash -c "time $(BUILD_PATH)/calc-pi 1000"

pi.profile:
	valgrind --tool=callgrind --dump-instr=yes --collect-jumps=yes $(BUILD_PATH)/calc-pi 10000

test: link-tests
	$(BUILD_PATH)/test-build

$(BUILD_PATH):
	mkdir -p $(BUILD_PATH)

link-pi: long.o pi.o
	$(LINK) $(BUILD_PATH)/long.o $(BUILD_PATH)/pi.o -o $(BUILD_PATH)/calc-pi

link-tests: tests.o long.o tester.o | $(BUILD_PATH)
	$(LINK) $(BUILD_PATH)/tests.o $(BUILD_PATH)/tester.o $(BUILD_PATH)/long.o -o $(BUILD_PATH)/test-build

long.o: $(SRC_PATH)/LongNumber.cpp | $(BUILD_PATH)
	$(COMPILE) $(SRC_PATH)/LongNumber.cpp -o $(BUILD_PATH)/long.o

tester.o: $(SRC_PATH)/Tester.cpp | $(BUILD_PATH)
	$(COMPILE) $(SRC_PATH)/Tester.cpp -o $(BUILD_PATH)/tester.o

pi.o: $(SRC_PATH)/pi.cpp | $(BUILD_PATH)
	$(COMPILE) $(SRC_PATH)/pi.cpp -o $(BUILD_PATH)/pi.o

tests.o: $(SRC_PATH)/tests.cpp | $(BUILD_PATH)
	$(COMPILE) $(SRC_PATH)/tests.cpp -o $(BUILD_PATH)/tests.o

clean:
	rm -rf $(BUILD_PATH)
