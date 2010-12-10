/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdint.h>
#include <stdlib.h>
#include <driver.h>
#include <abi.h>
#include <ipc.h>

/****************************************************************************
 * rirq
 *
 * Redirects all IRQs of number <irq> to this process as events. Only driver
 * processes have permission to do this.
 */

void rirq(uint8_t irq) {
	_rirq(irq);
}

/*****************************************************************************
 * irq_wrapper
 *
 * Handles and redirects irqs to the current active driver.
 */

void irq_wrapper(struct msg *msg) {
	
	if (!active_driver->irq) {
		return;
	}

	active_driver->irq();

	free(msg);
}