#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

#include "lib/string.h"
#include "devices/shutdown.h"
#include "userprog/process.h"
#include "threads/synch.h"
#include "threads/vaddr.h"
#include "filesys/file.h"
#include "filesys/filesys.h"

#define SYSCALL_COUNT 20

typedef int pid_t;
typedef int mapid_t;

static void syscall_handler(struct intr_frame *);
static void (*syscall_list[SYSCALL_COUNT])(uint32_t *args_stack, uint32_t *return_ptr);

void exception_exit()
{
    thread_current()->exit_error = -1;
    thread_exit();
}

static int get_user(const uint8_t *uaddr)
{
    int result;
    asm("movl $1f, %0; movzbl %1, %0; 1:" : "=&a"(result) : "m"(*uaddr));
    return result;
}

void *check_addr(const void *vaddr)
{
    // lab2 检查地址
    if (!is_user_vaddr(vaddr))
    {
        exception_exit();
    }
    // lab2 检测页
    void *ptr = pagedir_get_page(thread_current()->pagedir, vaddr);
    if (!ptr)
    {
        exception_exit();
    }
    // lab2 检查页内容
    uint8_t *check_byteptr = (uint8_t *)vaddr;
    for (uint8_t i = 0; i < 4; i++)
    {
        if (get_user(check_byteptr + i) == -1)
        {
            exception_exit();
        }
    }
    return ptr;
}

bool check_pointer(void *esp, uint8_t argc)
{
    for (uint8_t i = 0; i < argc; ++i)
    {
        if ((!is_user_vaddr(esp)) || (pagedir_get_page(thread_current()->pagedir, esp) == NULL))
        {
            return false;
        }
    }
    return true;
}

void syscall_halt(uint32_t *args_stack, uint32_t *return_ptr)
{
    shutdown_power_off();
}

void syscall_exit(uint32_t *args_stack, uint32_t *return_ptr)
{
    // arg0
    int exit_num = *args_stack;
    thread_current()->exit_error = exit_num;
    thread_exit();
}

void syscall_exec(uint32_t *args_stack, uint32_t *return_ptr)
{
    // arg0
    check_addr(*args_stack);
    const char *file_path = (const char *)*(args_stack);
    // retern pid_t
    *return_ptr = process_execute(file_path);
}

void syscall_wait(uint32_t *args_stack, uint32_t *return_ptr)
{
    // arg0
    int wait_pid = *args_stack;
    // return int
    *return_ptr = process_wait(wait_pid);
}

void syscall_create(uint32_t *args_stack, uint32_t *return_ptr)
{
    // arg0
    check_addr(*args_stack);
    const char *file_path = (const char *)*(args_stack);
    args_stack++;
    // arg1
    unsigned file_size = *args_stack;
    // return bool
    lock_acquire_filesys();
    *return_ptr = filesys_create(file_path, file_size);
    lock_release_filesys();
}

void syscall_remove(uint32_t *args_stack, uint32_t *return_ptr)
{
    // arg0
    check_addr(*args_stack);
    const char *file_path = (const char *)*(args_stack);
    // return bool
    lock_acquire_filesys();
    *return_ptr = filesys_remove(file_path);
    lock_release_filesys();
}

void syscall_open(uint32_t *args_stack, uint32_t *return_ptr)
{
    // arg0
    check_addr(*args_stack);
    const char *file_path = (const char *)*(args_stack);

    lock_acquire_filesys();
    struct file *p_file = filesys_open(file_path);
    lock_release_filesys();
    if (p_file)
    {
        struct thread *cur = thread_current();
        struct thread_file *p_thread_file = malloc(sizeof(struct thread_file));
        p_thread_file->fd = cur->next_fd;
        cur->next_fd++;
        p_thread_file->using_file = p_file;
        list_push_back(&cur->file_descriptor_list, &p_thread_file->file_elem);
        // return int
        *return_ptr = p_thread_file->fd;
    }
    else
    {
        // return int
        *return_ptr = -1;
    }
}

void syscall_filesize(uint32_t *args_stack, uint32_t *return_ptr)
{
    // arg0
    int fd = *args_stack;

    struct thread_file *p_thread_file = thread_find_file(fd);
    if (p_thread_file)
    {
        // return int
        lock_acquire_filesys();
        *return_ptr = file_length(p_thread_file->using_file);
        lock_release_filesys();
    }
    else
    {
        // return int
        *return_ptr = -1;
    }
}

void syscall_read(uint32_t *args_stack, uint32_t *return_ptr)
{
    // arg0
    int fd = *args_stack;
    args_stack++;
    // arg1
    char *buffer = (char *)*(args_stack);
    args_stack++;
    // arg2
    unsigned size = *args_stack;

    if (!check_pointer(buffer, 1) || !check_pointer(buffer + size, 1))
    {
        exception_exit();
    }
    if (fd == 0)
    {
        for (int i = 0; i < size; i++)
        {
            buffer[i] = input_getc();
        }
        // return int
        *return_ptr = size;
    }
    else
    {
        struct thread_file *p_thread_file = thread_find_file(fd);
        if (p_thread_file)
        {
            // return int
            lock_acquire_filesys();
            *return_ptr = file_read(p_thread_file->using_file, buffer, size);
            lock_release_filesys();
        }
        else
        {
            // return int
            *return_ptr = -1;
        }
    }
}

void syscall_write(uint32_t *args_stack, uint32_t *return_ptr)
{
    // arg0
    int fd = *args_stack;
    args_stack++;
    // arg1
    check_addr(*args_stack);
    const char *buffer = (const char *)*(args_stack);
    args_stack++;
    // arg2
    unsigned size = *args_stack;
    if (fd == 1)
    {
        putbuf(buffer, size);
        // return int
        *return_ptr = size;
    }
    else
    {
        struct thread_file *p_thread_file = thread_find_file(fd);
        if (p_thread_file)
        {
            // return int
            lock_acquire_filesys();
            *return_ptr = file_write(p_thread_file->using_file, buffer, size);
            lock_release_filesys();
        }
        else
        {
            // return int
            *return_ptr = 0;
        }
    }
}

void syscall_seek(uint32_t *args_stack, uint32_t *return_ptr)
{
    // arg0
    int fd = *args_stack;
    args_stack++;
    // arg1
    unsigned position = *args_stack;

    struct thread_file *p_thread_file = thread_find_file(fd);
    if (p_thread_file)
    {
        lock_acquire_filesys();
        file_seek(p_thread_file->using_file, position);
        lock_release_filesys();
    }
}

void syscall_tell(uint32_t *args_stack, uint32_t *return_ptr)
{
    // arg0
    int fd = *args_stack;

    struct thread_file *p_thread_file = thread_find_file(fd);
    if (p_thread_file)
    {
        // return unsigned
        lock_acquire_filesys();
        *return_ptr = file_tell(p_thread_file->using_file);
        lock_release_filesys();
    }
    else
    {
        // return unsigned
        *return_ptr = -1;
    }
}

void syscall_close(uint32_t *args_stack, uint32_t *return_ptr)
{
    // arg0
    int fd = *args_stack;

    struct thread_file *p_thread_file = thread_find_file(fd);
    if (p_thread_file)
    {
        lock_acquire_filesys();
        file_close(p_thread_file->using_file);
        lock_release_filesys();
        list_remove(&p_thread_file->file_elem);
        free(p_thread_file);
    }
}

// lab3之后才需要实现的部分
void syscall_mmap(uint32_t *args_stack, uint32_t *return_ptr)
{
    // arg0
    int fd = *args_stack;
    args_stack++;
    // arg1
    void *address = args_stack;
    // return mapid_t
}

void syscall_munmap(uint32_t *args_stack, uint32_t *return_ptr)
{
    // arg0
    mapid_t mapid = *args_stack;
}

void syscall_chdir(uint32_t *args_stack, uint32_t *return_ptr)
{
    // arg0
    const char *dir = (const char *)*(args_stack);
    // return bool
}

void syscall_mkdir(uint32_t *args_stack, uint32_t *return_ptr)
{
    // arg0
    const char *dir = (const char *)*(args_stack);
    // return bool
}

void syscall_readdir(uint32_t *args_stack, uint32_t *return_ptr)
{
    // arg0
    int fd = *args_stack;
    args_stack++;
    // arg1
    const char *dir = (const char *)*(args_stack);
    // return bool
}

void syscall_isdir(uint32_t *args_stack, uint32_t *return_ptr)
{
    // arg0
    int fd = *args_stack;
    // return bool
}

void syscall_inumber(uint32_t *args_stack, uint32_t *return_ptr)
{
    // arg0
    int fd = *args_stack;
    // return int
}

void syscall_init(void)
{
    // lab2 初始化全局文件系统锁
    lock_init(&filesys_lock);

    intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");

    syscall_list[SYS_HALT] = &syscall_halt;
    syscall_list[SYS_EXIT] = &syscall_exit;
    syscall_list[SYS_EXEC] = &syscall_exec;
    syscall_list[SYS_WAIT] = &syscall_wait;
    syscall_list[SYS_CREATE] = &syscall_create;
    syscall_list[SYS_REMOVE] = &syscall_remove;
    syscall_list[SYS_OPEN] = &syscall_open;
    syscall_list[SYS_FILESIZE] = &syscall_filesize;
    syscall_list[SYS_READ] = &syscall_read;
    syscall_list[SYS_WRITE] = &syscall_write;
    syscall_list[SYS_SEEK] = &syscall_seek;
    syscall_list[SYS_TELL] = &syscall_tell;
    syscall_list[SYS_CLOSE] = &syscall_close;

    syscall_list[SYS_MMAP] = &syscall_mmap;
    syscall_list[SYS_MUNMAP] = &syscall_munmap;

    syscall_list[SYS_CHDIR] = &syscall_chdir;
    syscall_list[SYS_MKDIR] = &syscall_mkdir;
    syscall_list[SYS_READDIR] = &syscall_readdir;
    syscall_list[SYS_ISDIR] = &syscall_isdir;
    syscall_list[SYS_INUMBER] = &syscall_inumber;
}

static void syscall_handler(struct intr_frame *f UNUSED)
{
    uint32_t *args_stack = (uint32_t *)(f->esp) + 1;
    check_addr(args_stack);
    uint32_t *return_ptr = &f->eax;
    int syscall_type = *(int *)(f->esp);
    if (syscall_type < 0 || syscall_type >= SYSCALL_COUNT)
    {
        exception_exit();
    }
    syscall_list[syscall_type](args_stack, return_ptr);
}
