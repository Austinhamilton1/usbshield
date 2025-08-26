CC=clang
BIN=bin
INC=include
OBJ=obj
SRC=src
ARCH=x86

BPF_CFLAGS=-target bpf -I$(INC) -I/usr/$(shell uname -m)-linux-gnu -g -O2 -Wall
CC_CFLAGS=-O2 -Wall
LDFLAGS=-lbpf -lelf -lz

BPF_TARGETS=$(patsubst $(SRC)/%.bpf.c, $(OBJ)/%.bpf.o, $(wildcard $(SRC)/*.bpf.c))
CC_TARGETS=$(patsubst $(SRC)/%.user.c, $(BIN)/%, $(wildcard $(SRC)/*.user.c))

.PHONY: all clean

all: $(BPF_TARGETS) $(CC_TARGETS)

$(OBJ)/%.bpf.o: $(SRC)/%.bpf.c
	$(CC) $(BPF_CFLAGS) -c $< -o $@

$(BIN)/%: $(SRC)/%.user.c
	$(CC) $(CC_CFLAGS) $< -o $@

clean:
	rm $(BIN)/*
	rm $(OBJ)/*