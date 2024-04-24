INCLUDE_PATH=include
CC=gcc
CFLAGS= -Wall -c -I $(INCLUDE_PATH)
NAME=elf-test

SDIR=source
_SOURCES=*.c
SOURCES=$(patsubst %,$(SDIR)/%,$(_SOURCES))

ODIR=objects
_OBJS=$(_SOURCES:.c=.o)
OBJS= $(patsubst %,$(ODIR)/%,$(_OBJS))

EXECUTABLE_DIR=exec/
EXECUTABLE=$(EXECUTABLE_DIR)$(NAME).elf
LOG=LOG.txt

all:	build $(SOURCES)  

build: $(OBJS)
	$(CC)  -o $(EXECUTABLE) $(OBJS) > $(LOG)
	
$(OBJS): 
	$(CC) $(CFLAGS) $(SOURCES)
	mv $(_OBJS) $(ODIR)
clean:
	rm $(OBJS)
