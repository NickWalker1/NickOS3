OUTPUT_FORMAT(binary)

KERNEL_PHYSICAL_ADDRESS = 0x00008000; 
KERNEL_VIRTUAL_ADDRESS  = 0xC0000000;

ENTRY(main)

INPUT
(
    boot/bootstrap.o
    drivers/low_level.o
    lib/screen.o
    lib/memory.o
    lib/int.o
    lib/smap.o
    lib/string.o
    lib/panic.o
    lib/timer.o
    lib/list.o
    kernel/start.o
    interrupt/idt.o
    interrupt/handlers.o
    interrupt/interrupt.o
    interrupt/cpu_state.o
    gdt/gdt.o
    paging/paging.o
    paging/pagedir.o
    heap/heap.o
    thread/thread.o
    thread/synch.o
    thread/tswitch.o
    process/tss.o
    


)

OUTPUT
(
    kernel.bin
)

SECTIONS
{   
    . = KERNEL_PHYSICAL_ADDRESS;
    BOOT_SECTION_START = .;
    .boot :
    {
        boot/bootstrap.o
    }
    BOOT_SECTION_END = .;


    . +=KERNEL_VIRTUAL_ADDRESS;


    TEXT_SECTION_START = .;  
	.text ALIGN(4K) : AT(ADDR(.text) - KERNEL_VIRTUAL_ADDRESS)
    {
        *(.text)
    }
    TEXT_SECTION_END = .;


    RODATA_SECTION_START = .;
	.rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_VIRTUAL_ADDRESS)
    {
        *(.rodata)
    }
    RODATA_SECTION_END = .;


    DATA_SECTION_START = .;
	.data ALIGN(4K) : AT(ADDR(.data) - KERNEL_VIRTUAL_ADDRESS)
    {
        *(.data)
    }
    DATA_SECTION_END = .;


    BSS_SECTION_START = .;
	.bss ALIGN(4K) : AT(ADDR(.bss) - KERNEL_VIRTUAL_ADDRESS)
    {
        *(.bss)
    }
    BSS_SECTION_END = .;
}