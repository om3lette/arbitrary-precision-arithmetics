CC=g++
CFLAGS=-c
LDFLAGS=
BUILD_PATH=./build

all: long-arithmetics
	./long-arithmetics

$(BUILD_PATH):
	mkdir -p $(BUILD_PATH)

long-arithmetics: long.o | $(BUILD_PATH)
	$(CC) $(BUILD_PATH)/long.o -o long-arithmetics

long.o: LongNumber.cpp | $(BUILD_PATH)
	$(CC) $(CFLAGS) LongNumber.cpp -o $(BUILD_PATH)/long.o

clean:
	rm -rf $(BUILD_PATH) long-arithmetics
