SECTIONS {
    . = 0x32001000;
    __section_start = .;
    .text          :   { *(.text) }
	
	. = ALIGN(4);
    .rodata        :   { *(.rodata) } 
	
	. = ALIGN(4);
    .data  		   :   { *(.data) }
     
	. = ALIGN(4);
	__bss_start = .;
    .bss   		   :   { *(.bss)  *(COMMON) }
    __bss_end = .;
    __section_end = .;
}
