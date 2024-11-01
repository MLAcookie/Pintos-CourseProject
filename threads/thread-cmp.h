// 简洁起见，把所有比较函数都写一起

#ifndef THREADS_THREAD_CMP_H
#define THREADS_THREAD_CMP_H

#include <debug.h>
#include <list.h>
#include <stdbool.h>

// lab1 线程休眠时间刻比较函数
bool thread_cmp_wakeup_tick(const struct list_elem *a, const struct list_elem *b, void *aux UNUSED);
// lab1 线程优先级比较函数
bool thread_cmp_priority(const struct list_elem *a, const struct list_elem *b, void *aux UNUSED);

#endif /* threads/thread-cmp.h */