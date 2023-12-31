// Long-term locks for processes
struct prioritylock {
  uint locked;       // Is the lock held?
  struct spinlock lk; // spinlock protecting this priority lock
  
  // For debugging:
  char *name;        // Name of lock.
  int pid;           // Process holding lock
};

