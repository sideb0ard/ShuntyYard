CC = gcc

CFLAGS = -std=c11 -Wall -Wextra -pedantic -Wstrict-prototypes -Wmissing-prototypes -g

SRC = shunty.c list.c stack.c

OBJ = $(SRC:.c=.o)
TARGET = shunty

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

