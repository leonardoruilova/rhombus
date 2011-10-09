/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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
#include <string.h>
#include <mutex.h>

#include <robject.h>

/*
 * various field manipulations
 */

void robject_set_call(struct robject *ro, const char *call, rcall_t hook) {
	
	if (ro) {
		mutex_spin(&ro->mutex);
		ro->call_table = __data_table_set(ro->call_table, call, (void*) (uintptr_t) hook);
		mutex_free(&ro->mutex);
	}
}

rcall_t robject_get_call(struct robject *ro, const char *call) {
	rcall_t hook;
	
	if (ro) {
		mutex_spin(&ro->mutex);
		hook = (rcall_t) (uintptr_t) __data_table_get(ro->call_table, call);
		if (!hook) hook = robject_get_call(ro->parent, call);
		mutex_free(&ro->mutex);

		return hook;
	}
	else {
		return NULL;
	}
}

void robject_set_data(struct robject *ro, const char *field, void *data) {
	
	if (ro) {
		mutex_spin(&ro->mutex);
		ro->data_table = __data_table_set(ro->data_table, field, data);
		mutex_free(&ro->mutex);
	}
}

void *robject_get_data(struct robject *ro, const char *field) {
	void *data;

	if (ro) {
		mutex_spin(&ro->mutex);
		data = __data_table_get(ro->data_table, field);
		mutex_free(&ro->mutex);

		return data;
	}
	else {
		return NULL;
	}
}

/*
 * event management
 */

void robject_set_event_hook(struct robject *ro, const char *type, rcall_t hook) {
	
	if (ro) {
		mutex_spin(&ro->mutex);
		ro->evnt_table = __data_table_set(ro->evnt_table, type, (void*) (uintptr_t) hook);
		mutex_free(&ro->mutex);
	}
}

rcall_t robject_get_event_hook(struct robject *ro, const char *type) {
	rcall_t hook;
	
	if (ro) {
		mutex_spin(&ro->mutex);
		hook = (rcall_t) (uintptr_t) __data_table_get(ro->evnt_table, type);
		if (!hook) hook = robject_get_event_hook(ro->parent, type);
		mutex_free(&ro->mutex);
		if (!hook) hook = robject_get_event_hook(robject_root, type);

		return hook;
	}
	else {
		return NULL;
	}
}

void robject_add_subscriber(struct robject *ro, rp_t target) {
	
	if (ro) {
		mutex_spin(&ro->mutex);
		ro->event_subs = __event_set_add(ro->event_subs, target);
		mutex_free(&ro->mutex);
	}
}

void robject_del_subscriber(struct robject *ro, rp_t target) {
	
	if (ro) {
		mutex_spin(&ro->mutex);
		ro->event_subs = __event_set_del(ro->event_subs, target);
		mutex_free(&ro->mutex);
	}
}

/*
 * basic interface
 */

void robject_cause_event(struct robject *ro, const char *event) {
	
	if (ro) {
		mutex_spin(&ro->mutex);
		__event_set_send(ro->event_subs, event);
		mutex_free(&ro->mutex);
	}
}

void robject_event(struct robject *ro, rp_t source, const char *event) {
	rcall_t hook;
	int argc;
	char **argv;

	// parse argument list
	argv = strparse(event, " ");
	if (!argv) return;

	for (argc = 0; argv[argc]; argc++);

	hook = robject_get_event_hook(ro, argv[0]);

	if (!hook) {
		for (argc = 0; argv[argc]; argc++) free(argv[argc]);
		free(argv);
	}
	else {
		hook(ro, source, argc, argv);
	}
}

char *robject_call(struct robject *ro, rp_t source, const char *args) {
	rcall_t call;
	int argc;
	char **argv;
	char *rets;

	// parse argument list
	argv = strparse(args, " ");
	if (!argv) return NULL;

	for (argc = 0; argv[argc]; argc++);

	call = robject_get_call(ro, argv[0]);

	if (!call) {
		for (argc = 0; argv[argc]; argc++) free(argv[argc]);
		free(argv);

		return NULL;
	}

	rets = call(ro, source, argc, argv);

	for (argc = 0; argv[argc]; argc++) free(argv[argc]);
	free(argv);

	return rets;
}

void *robject_data(struct robject *ro, const char *field) {
	return robject_get_data(ro, field);
}