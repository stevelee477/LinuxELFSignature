#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/ioctl.h>

static sem_t event_sem;
static volatile sig_atomic_t interested_event = 0;

void sig_handler_event1(int sig) {
    interested_event = 1;
    sem_post(&event_sem);
}

static void * event_handler_thread_func() {
    printf("in %s line %d\n", __func__, __LINE__);
    while(1){
        sem_wait(&event_sem);
        if (interested_event){
            printf("%s,%d, received interested_event signal.\n",__func__, __LINE__);

            char buf[256];
            int fd = open("/proc/mydev", O_RDWR);
            int len = read(fd, buf, 256);
            
            printf("%s", buf);

            interested_event = 0;
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t event_thread;
    if (pthread_create(&event_thread, NULL, event_handler_thread_func, NULL) != 0){
        printf("Thread create failed%s.\n", strerror(errno));
        exit(1);
    }
    sem_init(&event_sem, 0, 0);

    // Signal handler init
    struct sigaction usr_action;
    sigset_t block_mask;
    sigfillset (&block_mask);
    usr_action.sa_handler = sig_handler_event1;
    usr_action.sa_mask = block_mask;//block all signal inside signal handler.
    usr_action.sa_flags = SA_NODEFER;//do not block SIGUSR1 within sig_handler_int.
    printf ("handle signal %d\n", SIGRTMIN+1);
    sigaction (SIGRTMIN+1, &usr_action, NULL);

    // Pass pid to dev
    int fd = open("/dev/sign_passer", O_RDWR);
    int my_pid = getpid();
    ioctl(fd, 0x111, &my_pid);
    close(fd);

    while(1)
        sleep(5);
}

