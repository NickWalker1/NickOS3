OUTPUT_FORMAT(binary)

ENTRY(main)

INPUT
(
    boot/bootstrap.o
    drivers/low_level.o
    lib/screen.o
    lib/memory.o
    lib/int.o
    lib/smap.o
    kernel/start.o
)

OUTPUT
(
    kernel.bin
)

SECTIONS
{
    . = 0x8000;

    .text : ALIGN(0x1000)
    {
        *(.text)
    }

    .data : ALIGN(0x1000)
    {
        *(.data)
    }

    .rodata : ALIGN(0x1000)
    {
        *(.rodata)
    }

    .bss : ALIGN(0x1000)
    {
        *(COMMON)
        *(.bss)
    }
}