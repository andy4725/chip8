CC = gcc
CFLAGS = -Wall -Wextra
TARGET = chip8

$(TARGET): main.o chip8.o
	$(CC) $(CFLAGS) main.o chip8.o -o $(TARGET)

main.o: main.c chip8.h
	$(CC) $(CFLAGS) -c main.c

chip8.o: chip8.c chip8.h
	$(CC) $(CFLAGS) -c chip8.c

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) main.o chip8.o