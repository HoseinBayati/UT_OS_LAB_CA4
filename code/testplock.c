#include "types.h"
#include "user.h"

#define TARGET_LOCK_NUMBER 10
#define RIVAL_PROCS_COUNT 10

int main()
{
    int count = 0;
    plock_init(TARGET_LOCK_NUMBER);

    for (int i = 0; i < RIVAL_PROCS_COUNT; i++)
    {
        if (fork() == 0)
        {
            plock_acquire(TARGET_LOCK_NUMBER);
            sleep(1000);
            count++;
            printf(1, "pid:   %d   |", getpid());
            printf(1, "count:   %d\n", count);
            plock_release(TARGET_LOCK_NUMBER);
            exit();
        }
    }

    for (int i = 0; i < RIVAL_PROCS_COUNT; i++)
    {
        wait();
    }

    exit();
    return 0;
}