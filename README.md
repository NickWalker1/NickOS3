# NickOS3
32-bit basic OS

Now with paging! (WIP)

Physical Memory Locations:
#idt_base            0x1000 now just in kernel static memory
memory_map          0x5000 
boot_sect_base      0x7c00
bootloader_entry    0x8000 //Kernel also directly after this

vga_text_base       0x000b8000

# Virtual Addresses
kernel_base         0xC0000000
kernel_stacks       0xC1100000 (grows downwards remember)
page_tables         0xC1100000 (grows upwards)


TODO:
* paging
* process
