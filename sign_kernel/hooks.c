#include <linux/module.h>
#include <linux/semaphore.h>
#include "hooks.h"
#include "sys_hook.h"
#include "pass_pid.h"

extern struct sys_hook *lkh_sys_hook;
extern struct semaphore sema;
extern bool_t pass_checker;

static const char *file_prefixs[] = {/*"./", "/usr/local/bin/",*/ "/opt/google/", "/usr/bin/cat", "/usr/bin/deepin-editor"};

static const char *pwd_prefix = "PWD=";

char filename_k[256];
char pwd_k[256];
char buf[256];

asmlinkage
long execve_hook(const char __user *filename, const char __user *const __user *argv, const char __user *const __user *envp) {
    sys_execve_t sys_execve;
    sys_exit_t sys_exit;
    bool_t need_check = FALSE;

    long res;

    sys_execve = (sys_execve_t)sys_hook_get_orig64(lkh_sys_hook, __NR_execve);
    sys_exit = (sys_exit_t)sys_hook_get_orig64(lkh_sys_hook, __NR_exit);

    buf[255] = '\0';
    pwd_k[255] = '\0';

    res = strncpy_from_user(buf, filename, 255);
    if (res > 0) {
        int i;
        size_t lenpre, lenstr;

        lenstr = strlen(buf);

        for (i = 0; i < sizeof(file_prefixs)/sizeof(char *); i++) {
            lenpre = strlen(file_prefixs[i]);
            if (lenstr >= lenpre && memcmp(file_prefixs[i], buf, lenpre) == 0) {
                char *env;
                
                int i = 0;
                size_t lenpwd_prefix;
                size_t lenenv;

                filename_k[255] = '\0';
                strncpy(filename_k, buf, 255);

                need_check = TRUE;

                get_user(env, (char **)envp+i);
                lenpwd_prefix = strlen(pwd_prefix);

                while (1){
                    strncpy_from_user(pwd_k, env, 255);
                    i++;

                    lenenv = strlen(pwd_k);
                    if (lenenv > lenpwd_prefix && memcmp(pwd_prefix, pwd_k, lenpwd_prefix) == 0) {
                        break;
                    }

                    get_user(env, (char **)envp+i);
                    if (env == NULL)
                        break;
                }

                break;
            }
        }
    }

    if (need_check) {
        printk(KERN_INFO "Execve hooked: %s %s\n", filename_k, pwd_k); //Here is what matters
        send_signal(SIGRTMIN + 1);
        down(&sema);
        if (!pass_checker)
            sys_exit(-1);
    }

    return sys_execve(filename, argv, envp);
}