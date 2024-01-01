#include "types.h"
#include "user.h"

#define TARGET_LOCK_NUMBER 10
#define RIVAL_PROCS_COUNT 10

int main()
{
    plock_init(TARGET_LOCK_NUMBER);

    for (int i = 0; i < RIVAL_PROCS_COUNT; i++)
    {
        if (fork() == 0)
        {
            plock_acquire(TARGET_LOCK_NUMBER);
            sleep(100);
            printf(1, "critical section pid:   %d   \n", getpid());
            sleep(100);
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