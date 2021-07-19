# NickOS3
32-bit basic OS

Now with paging! (WIP)

Memory Locations:
idt_base            0x1000
memory_map          0x5000 
boot_sect_base      0x7c00
bootloader_entry    0x8000 //Kernel also directly after this

vga_text_base       0x000b8000
kernel_stacks       0x00090000
