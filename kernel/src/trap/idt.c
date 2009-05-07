// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>

void idt_set(u8int n, u32int base, u16int seg, u8int flags);

struct idt_entry {
	u16int base_l;
	u16int seg;
	u8int  reserved;
	u8int  flags;
	u16int base_h;
} __attribute__((packed)) idt[256];

handler_t int_handlers[256];

extern void int0(), int1(), int2(), int3(), int4(), int5(), int6(), int7(), int8(), int9();
extern void int10(), int11(), int12(), int13(), int14(), int15(), int16(), int17(), int18();
extern void int32(), int33(), int34(), int35(), int36(), int37(), int38(), int39(), int40();
extern void int41(), int42(), int43(), int44(), int45(), int46(), int47();

typedef void (*int_handler_t) (void);
int_handler_t idt_raw[] = {
int0, int1, int2, int3, int4, int5, int6, int7, int8, int9, int10, int11, int12, int13, int14, int15, int16, int17, int18, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, int32, int33, int34, int35, int36, int37, int38, int39, int40, int41, int42, int43, int44, int45, int46, int47 };

void init_idt() {
	u32int i;
	memset((u8int*) &idt[0], 0, sizeof(struct idt_entry) * 256);
	for (i = 0; i <= 47; i++) idt_set(i, (u32int) idt_raw[i], 0x08, 0x8E);
	extern void idt_flush();
	idt_flush();
}

void idt_set(u8int n, u32int base, u16int seg, u8int flags) {
	if (!base) return;
	idt[n].base_l = (u32int) base & 0xFFFF;
	idt[n].base_h = (u32int) base >> 16;
	idt[n].seg = seg;
	idt[n].reserved = 0;
	idt[n].flags = flags;
}

void register_int(u8int n, handler_t handler) {
	int_handlers[n] = handler;
}

void *int_handler(image_t *state) {
	if (state->num < 32) {
		u32int cr2;
		asm volatile ("movl %%cr2, %0" : "=r" (cr2));
		printk("exception %d, 0x%x, 0x%x:%x\n", state->num, state->err, cr2 >> 12, cr2 & 0xFFF);
		panic("exception");
	}
	else {
		if (state->num >= 40) outb(0xA0, 0x20);
		outb(0x20, 0x20);
	}

	if (int_handlers[state->num])
		int_handlers[state->num](state);

	return state;
}