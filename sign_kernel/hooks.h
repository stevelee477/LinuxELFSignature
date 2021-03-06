#pragma once

#include <linux/syscalls.h>
#include "types.h"

typedef asmlinkage long (*sys_execve_t)(const char __user *filename, const char __user *const __user *argv, const char __user *const __user *envp);
typedef asmlinkage void (*sys_exit_t)(const int __user status);

asmlinkage long
execve_hook(const char __user *filename, const char __user *const __user *argv, const char __user *const __user *envp);