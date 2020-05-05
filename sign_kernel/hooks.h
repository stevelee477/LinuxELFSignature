#pragma once

#include <linux/syscalls.h>
#include "types.h"

//typedef asmlinkage int (*sys_mkdir_t)(const char *, int);

//asmlinkage int
//mkdir_hook(const char *, int);

typedef asmlinkage long (*sys_execve_t)(const char __user *filename, const char __user *const __user *argv, const char __user *const __user *envp);

asmlinkage long
execve_hook(const char __user *filename, const char __user *const __user *argv, const char __user *const __user *envp);