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
#include <natio.h>
#include <errno.h>

/*****************************************************************************
 * fs_remove
 *
 * Attempts to remove the fileystem object <fobj>. Returns zero on success,
 * nonzero on failure.
 */

int fs_remove(uint64_t fobj) {
	struct fs_cmd command;

	command.op = FS_REMV;
	command.v0 = 0;
	command.v1 = 0;
	
	if (!fs_send(fobj, &command)) {
		errno = EBADMSG;
		return 1;
	}

	/* check for errors */
	if (command.op == FS_ERR) {
		switch (command.v0) {
		case ERR_NULL: errno = EUNK; break;
		case ERR_FILE: errno = ENOENT; break;
		case ERR_DENY: errno = EACCES; break;
		case ERR_FUNC: errno = ENOSYS; break;
		case ERR_TYPE: errno = EUNK; break;
		case ERR_FULL: errno = ENOTEMPTY; break;
		}

		return 1;
	}

	return 0;
}