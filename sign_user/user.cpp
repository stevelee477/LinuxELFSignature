#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "pubkey.h"
#include "signelf.h"

static sem_t event_sem;
static volatile sig_atomic_t interested_event = 0;

static void *event_handler_thread_func(void *);

void sig_handler_event1(int sig) {
    interested_event = 1;
    sem_post(&event_sem);

    pthread_t event_thread;
    pthread_create(&event_thread, NULL, event_handler_thread_func, NULL);
}

static void *event_handler_thread_func(void *) {
    printf("%s\n", "event_handler_thread_func()");
    printf("in %s line %d\n", __func__, __LINE__);
    sem_wait(&event_sem);

    printf("%s,%d, received interested_event signal.\n", __func__, __LINE__);

    char buf[256];
    int fd = open("/proc/mydev", O_RDWR);
    int len = read(fd, buf, 256);

    buf[strlen(buf) - 1] = '\0';
    printf("%s\n", buf);

    lseek(fd, 0, SEEK_SET);

    char str[2];
    str[1] = '\0';
    bool pass = signelf::verify(keyBuf, sizeof(keyBuf), buf);
    if (pass)
    {
        str[0] = '1';
    }
    else
    {
        str[0] = '0';
    }
    printf("Sign: %s\n", str);

    write(fd, str, 2);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    sem_init(&event_sem, 0, 0); // 起一个线程然后初始化信号量

    // Signal handler init
    struct sigaction usr_action; //hanlder初始化
    sigset_t block_mask;
    sigfillset(&block_mask);
    usr_action.sa_handler = sig_handler_event1;
    usr_action.sa_mask = block_mask;  //block all signal inside signal handler.
    usr_action.sa_flags = SA_NODEFER; //do not block SIGUSR1 within sig_handler_int.
    printf("handle signal %d\n", SIGRTMIN + 1);
    sigaction(SIGRTMIN + 1, &usr_action, NULL);

    // Pass pid to dev
    int fd = open("/dev/sign_passer", O_RDWR);
    int my_pid = getpid();
    ioctl(fd, 0x111, &my_pid);
    close(fd);

    while (1)
        sleep(5);
}
