# p4
OS p4



important xv6 files:


kernel space:

kernel/sysproc.c
  - system call handlers get arguments from stack for clone and join system calls

kernel/proc.c
  - clone and join work functions of the system calls

kernel/sysfunc.h
  - define sys_clone(void), sys_join(void) system call handlers

kernel/defs.h
  - define clone and join work functions for system calls



user space:

user/ulib.c
  - implement user library thread and lock functions. Thread functions call into kernel via system calls

user/user.h
  - define clone and join system calls for user, define thread_create, thread_join, lock_init, lock_acquire, 

lock_release user library functions

user/usys.S - handle system calls, sending clone and join into kernel
