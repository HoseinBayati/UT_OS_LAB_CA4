// Priority locks

#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "prioritylock.h"

#define MAX_QUEUE_LENGTH 30

int priority_queue[MAX_QUEUE_LENGTH];
int pq_size = 0;
int highest_priority = 0;

struct prioritylock plocks[PLOCKS_COUNT];

void init_priority_lock(struct prioritylock *lk, char *name)
{
  initlock(&lk->lk, "priority lock");
  lk->name = name;
  lk->locked = 0;
  lk->pid = 0;
}

int remove_from_priority_queue()
{
  int pid = myproc()->pid;

  if (pq_size >= MAX_QUEUE_LENGTH)
  {
    return 0;
  }

  int i = 0;
  while (pid != priority_queue[i] && i < pq_size)
  {
    i++;
  }

  if (i == pq_size)
  {
    cprintf("no such process in priority queue: pid %d\n", myproc()->pid);
    return 0;
  }

  for (int j = i; j < pq_size; j++)
  {
    priority_queue[j] = priority_queue[j + 1];
  }

  priority_queue[pq_size] = 0;
  pq_size--;

  return 1;
}

int add_to_priority_queue()
{
  int pid = myproc()->pid;

  if (pq_size >= MAX_QUEUE_LENGTH)
  {
    return 0;
  }

  int i = 0;
  while (pid > priority_queue[i] && i < pq_size)
  {
    i++;
  }

  for (int j = pq_size; j > i; j--)
  {
    priority_queue[j] = priority_queue[j - 1];
  }

  priority_queue[i] = pid;
  pq_size++;

  return 1;
}

void acquire_priority_lock(struct prioritylock *lk)
{
  acquire(&lk->lk);

  int pid = myproc()->pid;
  add_to_priority_queue();

  while (lk->locked || !(pid == priority_queue[0]))
  {
    sleep(lk, &lk->lk);
  }

  cprintf("\nproc: %d - acquired lock\n", myproc()->pid);
  // cprintf(" || priority_queue_size%d || ", pq_size);
  // cprintf("current min %d\n", priority_queue[0]);

  lk->locked = 1;
  lk->pid = myproc()->pid;
  release(&lk->lk);
}

void release_priority_lock(struct prioritylock *lk)
{
  acquire(&lk->lk);
  // pq_size--;
  remove_from_priority_queue();

  lk->locked = 0;
  lk->pid = 0;
  wakeup(lk);
  release(&lk->lk);
  cprintf("proc: %d - released lock\n", myproc()->pid);
  // cprintf(" || priority_queue_size%d || ", pq_size);
  // cprintf("current min %d\n", priority_queue[0]);
}

int holding_priority_lock(struct prioritylock *lk)
{
  int r;

  acquire(&lk->lk);
  r = lk->locked && (lk->pid == myproc()->pid);
  release(&lk->lk);
  return r;
}

void plock_init(int plock_id)
{
  // cprintf("hi 0 \n");
  init_priority_lock(&(plocks[plock_id]), "priority_lock");
}

void plock_acquire(int plock_id)
{
  // cprintf("hi 1 \n");
  acquire_priority_lock(&(plocks[plock_id]));
}

void plock_release(int plock_id)
{
  // cprintf("hi 2 \n");
  release_priority_lock(&(plocks[plock_id]));
}