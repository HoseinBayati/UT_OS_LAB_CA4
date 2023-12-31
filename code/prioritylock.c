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

#define MAX_QUEUE_LENGTH 20

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
  int target_pid = myproc()->pid;
  
  for (int i = 0; i < pq_size; i++)
  {
    if (priority_queue[i] == target_pid)
    {
      for (int j = i; j < pq_size; j++)
      {
        priority_queue[j] = priority_queue[j+1];
      }
      return 1;
    }
  }

  return 0;
}

int add_to_priority_queue(){
  int pid = myproc()->pid;
  if (pq_size == MAX_QUEUE_LENGTH)
  {
    return 0;
  }

  int i = 0;
  while(pid > priority_queue[i] && i < pq_size)
  {
    i++;
  }

  for (int j = pq_size; j > i; j--)
  {
    priority_queue[j] = priority_queue[j-1];
  }

  priority_queue[i] = pid;

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

  lk->locked = 1;
  lk->pid = myproc()->pid;
  release(&lk->lk);
}

void release_priority_lock(struct prioritylock *lk)
{
  acquire(&lk->lk);
  pq_size--;
  remove_from_priority_queue();
  lk->locked = 0;
  lk->pid = 0;
  wakeup(lk);
  release(&lk->lk);
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
  init_priority_lock(&(plocks[plock_id]), "priority_lock");
}

void plock_acquire(int plock_id)
{
  acquire_priority_lock(&(plocks[plock_id]));
}

void plock_release(int plock_id){
  release_priority_lock(&(plocks[plock_id]));
}