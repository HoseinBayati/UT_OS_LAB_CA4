#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"


#define NUM_PROCESSES 10

void writeToFiles(int p_order) {
    char filename[15] = "output_text";
    
    filename[11] = '0' + p_order;
    filename[12] = '\0'; 

    int fd = open(filename, O_WRONLY | O_CREATE);
    if (fd >= 0) {
        write(fd, "content", 7);
        close(fd);
    }
}

int main(int argc, char *argv[]) {
    for (int i = 0; i < NUM_PROCESSES; i++) {
        int pid = fork();
        if (pid == 0) {
            // Child process
            writeToFiles(i);
            exit();
        }
    }

    for (int i = 0; i < NUM_PROCESSES; i++) {
        wait();
    }

    getsyscnt();

    exit();
}