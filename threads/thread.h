#ifndef THREADS_THREAD_H
#define THREADS_THREAD_H

#include <debug.h>
#include <list.h>
#include <stdint.h>
#include "threads/fp.h"
#include "threads/synch.h"
#ifdef FILESYS
#include "filesys/file.h"
#endif

/* States in a thread's life cycle. */
enum thread_status
{
    THREAD_RUNNING, /* Running thread. */
    THREAD_READY,   /* Not running but ready to run. */
    THREAD_BLOCKED, /* Waiting for an event to trigger. */
    THREAD_DYING    /* About to be destroyed. */
};

/* Thread identifier type.
   You can redefine this to whatever type you like. */
typedef int tid_t;
#define TID_ERROR ((tid_t) - 1) /* Error value for tid_t. */

/* Thread priorities. */
#define PRI_MIN 0      /* Lowest priority. */
#define PRI_DEFAULT 31 /* Default priority. */
#define PRI_MAX 63     /* Highest priority. */

// lab1 高级调度 niceness部分
#define NICENESS_MIN -20
#define NICENESS_DEFAULT 0
#define NICENESS_MAX 20

// lab2 子线程信息
struct child_thread
{
    tid_t tid;
    struct list_elem child_elem;
    struct semaphore child_exit_sema;
    int exit_error;
    bool is_running;
};

// lab2 线程的文件管理
struct thread_file
{
    int fd;
    struct list_elem file_elem;
    struct file *using_file;
};

/* A kernel thread or user process.

   Each thread structure is stored in its own 4 kB page.  The
   thread structure itself sits at the very bottom of the page
   (at offset 0).  The rest of the page is reserved for the
   thread's kernel stack, which grows downward from the top of
   the page (at offset 4 kB).  Here's an illustration:

        4 kB +---------------------------------+
             |          kernel stack           |
             |                |                |
             |                |                |
             |                V                |
             |         grows downward          |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             +---------------------------------+
             |              magic              |
             |                :                |
             |                :                |
             |               name              |
             |              status             |
        0 kB +---------------------------------+

   The upshot of this is twofold:

    1. First, `struct thread' must not be allowed to grow too
        big.  If it does, then there will not be enough room for
        the kernel stack.  Our base `struct thread' is only a
        few bytes in size.  It probably should stay well under 1
        kB.

    2. Second, kernel stacks must not be allowed to grow too
        large.  If a stack overflows, it will corrupt the thread
        state.  Thus, kernel functions should not allocate large
        structures or arrays as non-static local variables.  Use
        dynamic allocation with malloc() or palloc_get_page()
        instead.

   The first symptom of either of these problems will probably be
   an assertion failure in thread_current(), which checks that
   the `magic' member of the running thread's `struct thread' is
   set to THREAD_MAGIC.  Stack overflow will normally change this
   value, triggering the assertion. */
/* The `elem' member has a dual purpose.  It can be an element in
   the run queue (thread.c), or it can be an element in a
   semaphore wait list (synch.c).  It can be used these two ways
   only because they are mutually exclusive: only a thread in the
   ready state is on the run queue, whereas only a thread in the
   blocked state is on a semaphore wait list. */

struct thread
{
    /* Owned by thread.c. */
    tid_t tid;                 // Thread identifier.
    enum thread_status status; // Thread state.
    char name[16];             // Name (for debugging purposes).
    uint8_t *stack;            // Saved stack pointer.
    int priority;              // Priority.
    int base_priority;         // lab1 添加基础优先级
    struct list_elem allelem;  // List element for all threads list.
    int wakeup_ticks;          // lab1 添加休眠终止时间刻

#ifdef THREAD
    struct lock *wait_on_lock; // lab1 等待的锁
    struct list lock_list;     // lab1 锁列表

    int niceness;  // lab1 高级调度 线程的好心程度
    fp recent_cpu; // lab1 高级调度 线程使用的cpu时间
#endif

    struct thread *parent_thread;     // lab2 父线程
    struct semaphore child_load_sema; // lab2 子线程加载信号量
    struct list child_list;           // lab2 子线程列表
    struct child_thread *child_info;  // lab2 作为子线程会用到的信息
    int exit_error;                   // lab2 错误退出代码
    bool is_create_success;           // lab2 子线程是否创建成功

    struct list file_descriptor_list; // lab2 线程文件描述列表
    int next_fd;                      // lab2 fd
    struct file *current_file;        // lab2 当前使用的文件

    /* Shared between thread.c and synch.c. */
    struct list_elem elem; /* List element. */

#ifdef USERPROG
    /* Owned by userprog/process.c. */
    uint32_t *pagedir; /* Page directory. */
#endif

    /* Owned by thread.c. */
    unsigned magic; /* Detects stack overflow. */
};

/* If false (default), use round-robin scheduler.
   If true, use multi-level feedback queue scheduler.
   Controlled by kernel command-line option "-o mlfqs". */
extern bool thread_mlfqs;

// lab1 线程休眠时间刻比较函数
bool thread_less_wakeup_tick(const struct list_elem *a, const struct list_elem *b, void *aux UNUSED);
// lab1 线程优先级比较函数
bool thread_more_priority(const struct list_elem *a, const struct list_elem *b, void *aux UNUSED);

#ifdef THREAD
// lab1 高级调度 系统负载
extern fp thread_load_avg;
// lab1 高级调度 基于nice与recent_cpu计算线程优先级
void thread_mlfqs_refresh_priority(struct thread *t);
// lab1 高级调度 当前线程recent_cpu自增1
void thread_mlfqs_increase_recent_cpu(void);
// lab1 高级调度 计算线程最近cpu使用率
void thread_mlfqs_refresh_recent_cpu(struct thread *t);
// lab1 高级调度 计算系统使用负载
void thread_mlfqs_refresh_load_avg(void);
// lab1 当前线程是否持有锁
bool thread_is_holding_lock(void);
#endif

void thread_init(void);
void thread_start(void);

// lab1 线程休眠函数 休眠至目标时间刻
void thread_sleep(int64_t target_ticks);
// lab1 线程苏醒函数 遍历休眠队列，苏醒达到条件的线程
void thread_wakeup(void);

// lab2 按照fd寻找文件
struct file *thread_find_file(int fd);

void thread_tick(void);
void thread_print_stats(void);

typedef void thread_func(void *aux);
tid_t thread_create(const char *name, int priority, thread_func *, void *);

void thread_block(void);
void thread_unblock(struct thread *);

struct thread *thread_current(void);
tid_t thread_tid(void);
const char *thread_name(void);

void thread_exit(void) NO_RETURN;
void thread_yield(void);

/* Performs some operation on thread t, given auxiliary data AUX. */
typedef void thread_action_func(struct thread *t, void *aux);
void thread_foreach(thread_action_func *, void *);

int thread_get_priority(void);
void thread_set_priority(int);

int thread_get_nice(void);
void thread_set_nice(int);
int thread_get_recent_cpu(void);
int thread_get_load_avg(void);

#endif
