CC=g++
CFLAGS=-c --std=c++23 -Wall -Wextra -Werror -Wundef -pedantic
LDFLAGS=

BUILD_PATH=build
COVERAGE_PATH=coverage
SRC_PATH=src

BUILD ?= debug
DIGITS ?= 100

ifeq ($(BUILD), release)
	CFLAGS += -O3 -DNDEBUG
else
	CFLAGS += -g -Og --coverage
	LDFLAGS += --coverage
endif

COMPILE = $(CC) $(CFLAGS)
LINK = $(CC) $(LDFLAGS)

all: link-tests link-pi

coverage: $(BUILD_PATH)/test-build | $(BUILD_PATH)
	./build/test-build
	lcov --capture --directory $(BUILD_PATH) --output-file $(BUILD_PATH)/coverage.info;\
		genhtml $(BUILD_PATH)/coverage.info --output-directory $(COVERAGE_PATH);

pi: $(BUILD_PATH)/calc-pi
	bash -c "time $(BUILD_PATH)/calc-pi $(DIGITS)"

pi.build: link-pi

pi.profile:
	valgrind --tool=callgrind --dump-instr=yes --collect-jumps=yes $(BUILD_PATH)/calc-pi 3000

test: $(BUILD_PATH)/test-build
	$(BUILD_PATH)/test-build

test.build: link-tests

$(BUILD_PATH):
	mkdir -p $(BUILD_PATH)

link-pi: long.o pi-utils.o pi-console.o
	$(LINK) $(BUILD_PATH)/long.o $(BUILD_PATH)/pi-utils.o $(BUILD_PATH)/pi-console.o -o $(BUILD_PATH)/calc-pi

link-tests: tests.o long.o tester.o pi-utils.o | $(BUILD_PATH)
	$(LINK) $(BUILD_PATH)/tests.o $(BUILD_PATH)/tester.o $(BUILD_PATH)/long.o $(BUILD_PATH)/pi-utils.o -o $(BUILD_PATH)/test-build

long.o: $(SRC_PATH)/LongNumber.cpp | $(BUILD_PATH)
	$(COMPILE) $(SRC_PATH)/LongNumber.cpp -o $(BUILD_PATH)/long.o

tests.o: $(SRC_PATH)/tests/tests.cpp | $(BUILD_PATH)
	$(COMPILE) $(SRC_PATH)/tests/tests.cpp -o $(BUILD_PATH)/tests.o

tester.o: $(SRC_PATH)/tests/Tester.cpp | $(BUILD_PATH)
	$(COMPILE) $(SRC_PATH)/tests/Tester.cpp -o $(BUILD_PATH)/tester.o

pi-console.o: $(SRC_PATH)/pi/pi.cpp | $(BUILD_PATH)
	$(COMPILE) $(SRC_PATH)/pi/pi.cpp -o $(BUILD_PATH)/pi-console.o

pi-utils.o: $(SRC_PATH)/pi/pi-utils.cpp | $(BUILD_PATH)
	$(COMPILE) $(SRC_PATH)/pi/pi-utils.cpp -o $(BUILD_PATH)/pi-utils.o

clean:
	rm -rf $(BUILD_PATH) $(COVERAGE_PATH)
