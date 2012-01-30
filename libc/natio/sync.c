/*
 * Copyright (C) 2009-2012 Nick Johnson <nickbjohnson4224 at gmail.com>
 * Copyright (C) 2011 Jaagup Repan <jrepan at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
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

#include <rho/natio.h>
#include <rho/proc.h>
#include <rho/ipc.h>

/****************************************************************************
 * sync
 *
 * "Synchronizes" the given resource. The specifics of this are very 
 * dependent on the resource, but in general, the operation flushes caches of
 * some sort and guarantees pending writes are performed.
 */

int rp_sync(uint64_t file, rk_t key) {
	struct msg *msg;

	msg = aalloc(sizeof(struct msg), PAGESZ);
	if (!msg) return 1;
	msg->source = RP_CURRENT_THREAD;
	msg->target = file;
	msg->key    = key;
	msg->length = 0;
	msg->action = ACTION_SYNC;
	msg->arch   = ARCH_NAT;

	if (msend(msg)) return 1;
	msg = mwait(ACTION_REPLY, file);

	free(msg);
	return 0;
}
