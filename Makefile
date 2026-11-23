CC = gcc
CFLAGS = -O2 -std=c11 -Wall -Wextra -Wpedantic
SRCS = main.c experimento.c func_hash.c lista.c hash_aberto.c hash_separado.c
TARGET = projeto

ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
    RM = del /Q
else
    RM = rm -f
endif

all: $(TARGET)

$(TARGET): $(SRCS) aluno.h experimento.h func_hash.h lista.h hash_aberto.h hash_separado.h
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	$(RM) $(TARGET)

.PHONY: all clean
