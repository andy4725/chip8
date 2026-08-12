CC = gcc
CFLAGS = -Wall -Wextra -I$(shell brew --prefix sdl3)/include -Isrc
LDFLAGS = -L$(shell brew --prefix sdl3)/lib
LIBS = -lSDL3

TARGET = chip8
OBJS = src/main.o src/chip8.o src/platform.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(LIBS) -o $(TARGET)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	@if [ -z "$(ROM)" ]; then \
		echo "Usage: make run ROM=<rom>"; \
		exit 1; \
	fi
	./$(TARGET) $(ROM)

clean:
	rm -f $(TARGET) $(OBJS)