// 简洁起见，把所有比较函数都写一起

#include "threads/thread-cmp.h"
#include "threads/thread.h"
// lab1 线程休眠时间刻比较函数
bool thread_cmp_wakeup_tick(const struct list_elem *a, const struct list_elem *b, void *aux UNUSED)
{
    struct thread *pta = list_entry(a, struct thread, elem);
    struct thread *ptb = list_entry(b, struct thread, elem);
    return pta->wakeup_ticks < ptb->wakeup_ticks;
}

// lab1 线程优先级比较函数
bool thread_cmp_priority(const struct list_elem *a, const struct list_elem *b, void *aux UNUSED)
{
    struct thread *pta = list_entry(a, struct thread, elem);
    struct thread *ptb = list_entry(b, struct thread, elem);
    return pta->priority < ptb->priority;
}
