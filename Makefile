CC=g++
CFLAGS=-c
LDFLAGS=
BUILD_PATH=build
SRC_PATH=src

all: long-arithmetics
	./long-arithmetics

test:
	./long-arithmetics

$(BUILD_PATH):
	mkdir -p $(BUILD_PATH)

long-arithmetics: main.o long.o tester.o | $(BUILD_PATH)
	$(CC) $(BUILD_PATH)/main.o $(BUILD_PATH)/tester.o $(BUILD_PATH)/long.o -o long-arithmetics

long.o: $(SRC_PATH)/LongNumber.cpp | $(BUILD_PATH)
	$(CC) $(CFLAGS) $(SRC_PATH)/LongNumber.cpp -o $(BUILD_PATH)/long.o

tester.o: $(SRC_PATH)/Tester.cpp | $(BUILD_PATH)
	$(CC) $(CFLAGS) $(SRC_PATH)/Tester.cpp -o $(BUILD_PATH)/tester.o

main.o: $(SRC_PATH)/main.cpp | $(BUILD_PATH)
	$(CC) $(CFLAGS) $(SRC_PATH)/main.cpp -o $(BUILD_PATH)/main.o

clean:
	rm -rf $(BUILD_PATH) long-arithmetics
