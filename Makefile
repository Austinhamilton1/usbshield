CC=clang
BIN=bin
INC=include
OBJ=obj
SRC=src
ARCH=x86

BPF_CFLAGS=-target bpf -D __TARGET_ARCH_$(ARCH) -I$(INC) -I/usr/$(shell uname -m)-linux-gnu -g -O2 -Wall
CC_CFLAGS=-I$(INC) -O2 -Wall
LDFLAGS=-lbpf -lelf -lz

BPF_TARGETS=$(patsubst $(SRC)/%.bpf.c, $(OBJ)/%.bpf.o, $(wildcard $(SRC)/*.bpf.c))
CC_TARGETS=$(patsubst $(SRC)/%.user.c, $(BIN)/%, $(wildcard $(SRC)/*.user.c))
BPF_SKELS=$(patsubst $(OBJ)/%.bpf.o, $(INC)/%.skel.h, $(BPF_TARGETS))

.PHONY: all bpf header user clean

all: bpf header user

bpf: $(BPF_TARGETS) 

header: $(BPF_SKELS)

user: $(CC_TARGETS)

$(OBJ)/%.bpf.o: $(SRC)/%.bpf.c | $(OBJ)
	$(CC) $(BPF_CFLAGS) -c $< -o $@
	llvm-strip -g $@

$(INC)/%.skel.h: $(OBJ)/%.bpf.o
	bpftool gen skeleton $< > $@

$(BIN)/%: $(SRC)/%.user.c | $(BIN)
	$(CC) $(CC_CFLAGS) $< -o $@ $(LDFLAGS)

$(OBJ):
	mkdir -p $@

$(BIN):
	mkdir -p $@

clean:
	rm -rf $(BIN)
	rm -rf $(OBJ)
	rm $(INC)/*.skel.h