C_SOURCES = $(wildcard kernel/*.c drivers/*.c lib/*.c idt/*.c)
HEADERS   = $(wildcard kernel/*.h drivers/*.h lib/*.h idt/*.h)

OBJ=${C_SOURCES:.c=.o}

all: os-image

run: all
	qemu-system-x86_64 -fda os-image

#concatenate the binaries together
os-image: boot/mbr.bin kernel.bin
	cat $^ > os-image

kernel.bin: boot/bootstrap.o ${OBJ} idt/interrupt.o
	i386-elf-ld -T "link.ld"


#generic rule for compiling C code into obj file
#for simplicity all C files depend on all the headers
%.o : %.c ${HEADERS}
	i386-elf-gcc -m32 -Ttext 0x10000 -c $< -o $@

#Assemble the bootstrap.o
%.o : %.asm 
	nasm $< -f elf32 -I './boot/' -o $@


#Assemble the boot asm files
%.bin : %.asm
	nasm $< -f bin -I './boot/' -o $@

clean:
	rm -f boot/*.bin boot/*.o
	rm -f kernel/*.o kernel.bin
	rm -f drivers/*.o
	rm -f os-image

