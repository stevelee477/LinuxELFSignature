#include <linux/fs.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/uaccess.h>

#include "pass_pid.h"

int user_pid = 0;
struct task_struct *curtask;
struct file_operations dev_file_ops;

typedef enum {
  THIS_MODULE_IOCTL_SET_OWNER = 0x111,
} MODULE_IOCTL_CMD;

/* Prototypes for device functions */
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static long device_ioctl(struct file *file, unsigned int cmd,
                         unsigned long arg);
static int send_sig_info(int sig, struct siginfo *info, struct task_struct *p);
static int device_open_count = 0;

/* This structure points to all of the device functions */
struct file_operations dev_file_ops = {
        .read = device_read,
        .write = device_write,
        .open = device_open,
        .release = device_release,
        .unlocked_ioctl = device_ioctl,
        .compat_ioctl = device_ioctl
};

/* When a process reads from our device, this gets called. */
static ssize_t device_read(struct file *flip, char *buffer, size_t len,
                           loff_t *offset) {
  /* not implemented yet */
  return 0;
}

/* Called when a process tries to write to our device */
static ssize_t device_write(struct file *flip, const char *buffer, size_t len,
                            loff_t *offset) {
  /* not implemented yet */
  return len;
}

/* Called when a process opens our device */
static int device_open(struct inode *inode, struct file *file) {
  /* If device is open, return busy */
  if (device_open_count) {
    return -EBUSY;
  }
  device_open_count++;
  try_module_get(THIS_MODULE);
  return 0;
}

/* Called when a process closes our device */
static int device_release(struct inode *inode, struct file *file) {
  /* Decrement the open counter and usage count. Without this, the module would
   * not unload. */
  device_open_count--;
  module_put(THIS_MODULE);
  return 0;
}

static long device_ioctl(struct file *file, unsigned int cmd,
                         unsigned long arg) {
  if (cmd == THIS_MODULE_IOCTL_SET_OWNER) {
    if (copy_from_user(&user_pid, (int *)arg,
                       sizeof(int))) { /* 从copy from user 改为memcpy*/
      return -EFAULT;
    }
    printk("SIGN CHECKER: %s, user_pid pid %d\n", __func__, user_pid);
    curtask = NULL;
    return 0;
  } else
    return -ENOIOCTLCMD;
}

void send_signal(int sig_num) {
  struct siginfo info;
  int ret;

  if (user_pid == 0)
    return;
  printk(KERN_INFO "SIGN CHECKER: %s,%d.sending signal %d to user_pid %d\n", __func__, __LINE__,
         sig_num, user_pid);

  memset(&info, 0, sizeof(struct siginfo));
  info.si_signo = sig_num;
  info.si_code = 0;
  info.si_int = 1234;
  if (curtask == NULL) {
    rcu_read_lock();
    curtask = pid_task(find_vpid(user_pid), PIDTYPE_PID);
    rcu_read_unlock();
  }
  ret = send_sig_info(sig_num, &info, curtask);
  if (ret < 0) {
    printk("SIGN CHECKER: error sending signal\n");
  }
}