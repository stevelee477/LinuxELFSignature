#include "hooks.h"
#include "sys_hook.h"
#include "pass_pid.h"
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>

// Hook variable
struct sys_hook *lkh_sys_hook;
extern char filename_k[256];
extern char pwd_k[256];

// Dev file variable
extern int user_pid;
extern struct task_struct *curtask;
extern struct file_operations dev_file_ops;
static int major_num;

// Proc file variable
#define BUFSIZE  200
static struct proc_dir_entry *ent;

static
uintptr_t hex_addr_to_pointer(const char *str) {
  uintptr_t sum;

  /* Go through str char by char and accumulate into sum */
  for (sum = 0; *str != '\0'; str++) {
    sum *= 16;
    if (*str >= '0' && *str <= '9')
      sum += (*str - '0');
    else if (*str >= 'a' && *str <= 'f')
      sum += (*str - 'a') + 10;
    else if (*str >= 'A' && *str <= 'F')
      sum += (*str - 'A') + 10;
    else
      return 0;
  }

  return sum;
}

static
ssize_t sign_write(struct file *file, const char __user *ubuf,size_t count, loff_t *ppos) {
  printk( KERN_DEBUG "write handler\n");
	return -1;
}
 
static
ssize_t sign_read(struct file *file, char __user *ubuf,size_t count, loff_t *ppos) {
  char buf[BUFSIZE];
  int len;

  len = 0;
	printk( KERN_DEBUG "SIGN CHECKER: proc read handler\n");
  if(*ppos > 0 || count < BUFSIZE)
		return 0;
  len += sprintf(buf, "%s %s\n", filename_k, pwd_k);

  if(copy_to_user(ubuf,buf,len))
		return -EFAULT;

	*ppos = len;
	return len;	return 0;
}
 
static struct file_operations myops = {
	.owner = THIS_MODULE,
	.read = sign_read,
	.write = sign_write,
};

/* Module parameter macros */
static char *kbase32 = NULL, *kbase64 = NULL;
module_param(kbase32, charp, 0);
MODULE_PARM_DESC(kbase32, "Base address of the x86 syscall table, in hex");
module_param(kbase64, charp, 0);
MODULE_PARM_DESC(kbase64, "Base address of the x64 syscall table, in hex");

static int __init module_entry(void) {
  uintptr_t k32, k64;

  printk(KERN_INFO "SIGN CHECKER: kernel module initializing...\n");

  // Init hook
  /* Validate that we got parameters */
  if (kbase32 == NULL || kbase32[0] == '\0') {
    printk(KERN_INFO "failed to get x86 syscall table\n");
    return 1;
  } else if (kbase64 == NULL || kbase64[0] == '\0') {
    printk(KERN_INFO "failed to get x64 syscall table\n");
    return 1;
  }

  /* Validate that we got valid syscall base addresses */
  if ((k32 = hex_addr_to_pointer(kbase32)) == 0) {
    printk(KERN_ALERT "SIGN CHECKER: invalid x86 syscall address %p\n", (void *)k32);
    return 1;
  } else if ((k64 = hex_addr_to_pointer(kbase64)) == 0) {
    printk(KERN_ALERT "SIGN CHECKER: invalid x64 syscall address %p\n", (void *)k64);
    return 1;
  }

  if ((lkh_sys_hook = sys_hook_init(k32, k64)) == NULL) {
    printk(KERN_ALERT "SIGN CHECKER: failed to initialize sys_hook\n");
    return 1;
  }

  sys_hook_add64(lkh_sys_hook, __NR_execve, (void *)execve_hook);

  printk(KERN_INFO "SIGN CHECKER: execve hook successful\n");

  // Init Dev file
  major_num = register_chrdev(0, "sign_passer", &dev_file_ops);
  if (major_num < 0) {
    printk(KERN_ALERT "SIGN CHECKER: Could not register device: %d\n", major_num);
    return -1;
  } else {
    printk(KERN_INFO "SIGN CHECKER: sign_passer registered with device major number %d\n", major_num);
  }

  // Init Proc file
  ent=proc_create("mydev",0660,NULL,&myops);
  printk(KERN_INFO "SIGN CHECKER: proc created\n");

  printk(KERN_INFO "SIGN CHECKER: kernel module loaded\n");
  return 0;
}

static
void __exit module_cleanup(void) {
  sys_hook_free(lkh_sys_hook);
  unregister_chrdev(major_num, "sign_passer");
  proc_remove(ent);
  printk(KERN_INFO "SIGN CHECKER: kernel module has finished\n");
}

/* Declare the entry and exit points of our module */
module_init(module_entry);
module_exit(module_cleanup);

/* Shut up kernel warnings about tainted kernels due to non-free software */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("liyuanhao");
MODULE_DESCRIPTION("Linux ELF Sign Checker");