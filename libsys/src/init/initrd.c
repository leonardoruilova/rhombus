// Copyright 2009 Nick Johnson

#include <lib.h>
#include <init.h>
#include <exec.h>

// Standard V7 TAR header
struct tar_header {
	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char chksum[8];
	char link;
	char linkname[100];
	u8int padding[255];
} __attribute__ ((packed));

static int tar_header_check(struct tar_header *t) {
	u32int i, sum = 0;
	u8int *header_byte = (u8int*) t;
	for (i = 0;   i < 512; i++) sum += header_byte[i];
	for (i = 148; i < 156; i++) sum -= header_byte[i]; 	// Discount checksum itself
	for (i = 148; i < 156; i++) sum += (u8int) ' ';		// Count checksum as spaces
	if (atoi(t->chksum, 8) == sum) return sum;
	return -1;
}

static int tar_end_check(u8int *block) {
	u32int i;
	for (i = 0; i < 0x1000; i++) if (block[i]) return 0; // Terminator is one page of zeroes
	return 1;
}

void init_load_init() {
	struct tar_header *initrd = (void*) 0x10000000;
	struct tar_header *header[256];
	elf_t *init_base;
	load_image_t image;
	int i, n;
	char buffer[10];
	
	// Index tar file
	for (i = 0, n = 0; tar_end_check((void*) &initrd[i]) == 0; i++)
		if (tar_header_check(&initrd[i]) != -1)
			header[n++] = &initrd[i];

	// Find "init"
	for (i = 0; i < n; i++) if (!strcmp(header[i]->name, "init")) break;
	if (i == n) {
		eout("no init found - aborting boot\n");
		for(;;);
	}

	eout("Found init\n");

	// Load init
	init_base = ((u32int) header[i] + 512);
	elf_load(init_base, &image);
	eout("Loaded init; entry point is 0x");
	eout(itoa(image.entry, buffer, 16));
	eout("\n");

	for(;;);
}
