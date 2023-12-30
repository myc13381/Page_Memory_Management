/*
 * 实现一个轻量化的队列，用于实现空闲的PCB，Page等资源的获取
 * 将空闲的资源插入队列中，需要时从队列中弹出一个
 * 资源回收后插入队列
*/



#ifndef PAGE_MEMORY_MANAGEMENT_QUEUELITE
#define PAGE_MEMORY_MANAGEMENT_QUEUELITE

#include "head.h"

#define QUEUE_CAPACITY 1024 * 16 // 队列的最大容量

// 轻量级队列
struct QueueLite
{
    base_type *ptr;     // 队列本体
    base_type capacity; // 队列容量
    base_type size;     // 队列长度
    base_type head;     // 队列头部
    base_type tail;     // 队列尾部
};

// 初始化队列
void initQueue(struct QueueLite *q);

// 创建一个队列
struct QueueLite* creatQueue(base_type size);

// 销毁一个队列
void destroyQueue(struct QueueLite **q);

// 重新设置队列长度
void resetQueueSize(struct QueueLite *q, base_type size);

// 获取队列当前长度
base_type getQueueSize(const struct QueueLite *q);

// 判断队列是否为空
bool_type queue_empty(const struct QueueLite *q);

// 尾插元素
bool_type queue_push(struct QueueLite *q, base_type value);

// 返回头部元素
base_type queue_front(struct QueueLite *q);

// 弹出头部元素
void queue_pop(struct QueueLite *q);

// 使用数组初始化队列
void initQueueByArray(struct QueueLite *q, base_type *arr, base_type size);

#endif //PAGE_MEMORY_MANAGEMENT_QUEUELITE
