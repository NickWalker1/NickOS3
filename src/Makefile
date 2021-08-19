C_SOURCES = $(wildcard kernel/*.c drivers/*.c lib/*.c interrupt/*.c paging/*.c gdt/*.c heap/*.c )
HEADERS   = $(wildcard kernel/*.h drivers/*.h lib/*.h interrupt/*.h paging/*.h gdt/*.h heap/*.h )

OBJ=${C_SOURCES:.c=.o}

CFLAGS=-m32 -ffreestanding

all: os-image

#GCC=gcc
GCC=i386-elf-gcc
#LD=ld
LD=i386-elf-ld

run: all
	qemu-system-x86_64 -fda os-image

#concatenate the binaries together
os-image: boot/mbr.bin kernel.bin
	cat $^ > os-image

kernel.bin: boot/bootstrap.o ${OBJ} interrupt/interrupt.o
	$(LD)  -T "link.ld"


#generic rule for compiling C code into obj file
#for simplicity all C files depend on all the headers
%.o : %.c ${HEADERS}
	$(GCC) -m32 -c $< -o $@

#Assemble the bootstrap.o
%.o : %.asm 
	nasm $< -f elf32 -I './boot/' -o $@


#Assemble the boot asm files
%.bin : %.asm
	nasm $< -f bin -I './boot/' -o $@

clean:
	rm -f boot/*.bin kernel.bin
	rm -f */*.o
	rm -f os-image

