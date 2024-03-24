DEBUG_FLAGS = -Wall -Wextra -pedantic
TARGET = cwisdom
COMPILER_FLAGS = $(DEBUG_FLAGS)
LIBNOTIFY_FLAGS = `pkg-config --cflags --libs libnotify`


all: $(TARGET)

$(TARGET): object
	gcc -o $@ ./*.o $(LIBNOTIFY_FLAGS) $(LINKER_FLAGS)

object:
	gcc $(COMPILER_FLAGS) $(LIBNOTIFY_FLAGS) -I include/ -c src/*.c
