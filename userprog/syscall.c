#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

#include "threads/synch.h"
#include "devices/shutdown.h"
#include "lib/string.h"
#include "userprog/process.h"
#include "filesys/off_t.h"

typedef int pid_t;

static void syscall_handler(struct intr_frame *);

void syscall_exit(int status)
{
    struct list_elem *e;
    thread_current()->exit_error = status;
    thread_exit();
}

void syscall_exec(struct intr_frame *f)
{
    
    f->eax = process_execute(NULL);
}

void syscall_write(struct intr_frame *f)
{
    uint32_t *user_ptr = f->esp;
    // check_ptr2(user_ptr + 7);
    // check_ptr2(*(user_ptr + 6));
    *user_ptr++;
    int temp2 = *user_ptr;
    const char *buffer = (const char *)*(user_ptr + 1);
    off_t size = *(user_ptr + 2);
    if (temp2 == 1)
    {
        /* Use putbuf to do testing */
        putbuf(buffer, size);
        f->eax = size;
    }
    // else
    // {
    //     /* Write to Files */
    //     struct thread_file *thread_file_temp = find_file_id(*user_ptr);
    //     if (thread_file_temp)
    //     {
    //         acquire_lock_f();
    //         f->eax = file_write(thread_file_temp->file, buffer, size);
    //         release_lock_f();
    //     }
    //     else
    //     {
    //         f->eax = 0;
    //     }
    // }
}

void syscall_init(void)
{
    intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void syscall_handler(struct intr_frame *f UNUSED)
{
    int *p = f->esp;
    int system_call = *p;
    switch (system_call)
    {
    case SYS_HALT:
        shutdown_power_off();
        break;

    case SYS_EXIT:
        syscall_exit(*(p + 1));
        break;

    case SYS_EXEC:

        break;
    case SYS_WAIT:
        break;

    case SYS_CREATE:
        break;

    case SYS_REMOVE:
        break;

    case SYS_OPEN:
        break;

    case SYS_FILESIZE:
        break;

    case SYS_READ:
        break;

    case SYS_WRITE:
        syscall_write(f);
        break;

    case SYS_SEEK:
        break;

    case SYS_TELL:
        break;

    case SYS_CLOSE:
        break;

    default:
        printf("invalid system call!\n");
    }
}
