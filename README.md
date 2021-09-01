# NickOS3
32-bit basic OS

Now with paging!

Physical Memory Locations:
#idt_base           0x1000 //now just in kernel static memory
memory_map          0x5000 
kernel_pd           0x6000
kernel TCB          0x7000
bootloader_entry    0x8000 //Kernel also directly after this

vga_text_base       0x000b8000

# Virtual Addresses
kernel_base         0xC0000000
kernel_page_dir     0xC0006000


TODO:
* sort out TSS
* bug fix
* clean up code
