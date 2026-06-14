AS = gcc
ASFLAGS = -m32 -nostdlib 

all: main

main: main.S
	$(AS) $(ASFLAGS) main.S -o main

clean:
	rm -f main