// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>
#include <task.h>
#include <mem.h>

pool_t tpool[SIZEOF_POOL(MAX_TASKS)];	// Pool allocator for task structures
task_t *task = (void*) TASK_TBL; 		// Array of task structures (max 65536)
pid_t curr_pid = 0;

task_t *task_get(pid_t pid) {
	if (pid >= MAX_TASKS) return NULL;
	if ((page_get((uint32_t) &task[pid]) & PF_PRES) == 0) {
		p_alloc((uint32_t) &task[pid], (PF_PRES | PF_RW));
		pgclr((void*) &task[pid]);
	}
	return (task[pid].magic == 0x4224) ? &task[pid] : NULL;
}

// Used to initialize new tasks that are not marked yet
void task_touch(pid_t pid) {
	if (pid >= MAX_TASKS) return;
	if ((page_get((uint32_t) &task[pid]) & PF_PRES) == 0) {
		p_alloc((uint32_t) &task[pid], (PF_PRES | PF_RW));
		pgclr((void*) &task[pid]);
	}
	task[pid].magic = 0x4224;
}

task_t *task_new(task_t *src) {
	if (!src) panic ("bad parent");
	pid_t new_pid = pool_alloc(tpool);
	task_touch(new_pid);
	task_t *new = task_get(new_pid);
	if (!new) panic ("bad pid");

	new->map = map_clone();
	new->image = src->image;
	new->flags = src->flags;
	new->parent = src->pid;
	new->pid = new_pid;
	new->tss_esp = src->tss_esp;

	sched_ins(new_pid);
	return new;
}

uint32_t task_rem(task_t *t) {
	t->magic = 0x0000;
	sched_rem(t->pid);
	return pool_free(tpool, t->pid);
}

image_t *task_switch(task_t *t) {
	curr_pid = t->pid;

	map_load(t->map);
	tss_set_esp(t->tss_esp);
	return t->image;
}

