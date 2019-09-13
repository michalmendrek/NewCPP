#define _GNU_SOURCE
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

static volatile int dir_fd;

/* A very weak interface: we report that something changed, but
   the only info is in which directory, but not what the change is. */
static void handler(int sig, siginfo_t *si, void *data) {
        dir_fd = si->si_fd;
        //printf("Cos sie stalo");
}

int main(void) {
        struct sigaction act;
        int fd;

        act.sa_sigaction = handler;
        sigemptyset(&act.sa_mask);
        act.sa_flags = SA_SIGINFO;
        sigaction(SIGRTMIN + 1, &act, NULL);

        fd = open(".", O_RDONLY);
        fcntl(fd, F_SETSIG, SIGRTMIN + 1);
        fcntl(fd, F_NOTIFY, DN_MODIFY | DN_CREATE);

        while (1) {
                pause();
                printf("Got some event on fd=%d\n", dir_fd);
        }
}
