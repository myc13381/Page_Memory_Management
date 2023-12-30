#include "queueLite.h"

// 初始化队列
void initQueue(struct QueueLite *q)
{
    assert(q != NULL);
    q->ptr = NULL;
    q->head = 0;
    q->tail = 0;
    q->capacity = 0;
    q->size = 0;
}

// 创建一个队列
struct QueueLite* creatQueue(base_type size)
{
    assert(size <= QUEUE_CAPACITY);
    struct QueueLite *q = (struct QueueLite *)malloc(sizeof(struct QueueLite));
    assert(q != NULL);
    q->ptr = (base_type *)malloc(size * sizeof(base_type));
    assert(q->ptr != NULL);
    q->head = q->tail = 0;
    q->size = 0;
    q->capacity = size;
    return q;
}

// 销毁一个队列
void destroyQueue(struct QueueLite **q)
{
    assert(q != NULL && *q != NULL);
    if((*q)->ptr != NULL)
    {
        free((*q)->ptr);
        (*q)->ptr = NULL;
    }
    initQueue((*q));
    free((*q));
    (*q) = NULL;
}


// 重新设置队列容量
void resetQueueSize(struct QueueLite *q, base_type size)
{
    assert(q != NULL);
    if(q->capacity != size)
    {
        q->ptr = (base_type *)realloc(q->ptr, size * sizeof(base_type));
        q->capacity = size;
    }
}

// 获取队列当前长度
base_type getQueueSize(const struct QueueLite *q)
{
    assert(q != NULL);
    return q->size;
}

// 判断队列是否为空
bool_type queue_empty(const struct QueueLite *q)
{
    assert(q != NULL);
    if(q->size == 0) return TYPE_TRUE;
    else return TYPE_FALSE;
}


// 尾插元素
bool_type queue_push(struct QueueLite *q, base_type value)
{
    assert(q != NULL);
    if(q->capacity == 0 && q->size == q->capacity) return TYPE_FALSE;
    else
    {
        (q->ptr)[q->tail] = value;
        q->tail = (q->tail + 1) % q->capacity;
        ++(q->size);
        return TYPE_TRUE;
    }
}

// 返回头部元素
base_type queue_front(struct QueueLite *q)
{
    assert(q != NULL);
    assert(q->capacity != 0 && q->size != 0);
    return (q->ptr)[q->head];
}

// 弹出头部元素
void queue_pop(struct QueueLite *q)
{
    assert(q != NULL);
    assert(q->capacity != 0 && q->size != 0);
    q->head = (q->head + 1) % q->capacity;
    --q->size;
}

// 使用数组初始化队列,需要提前为队列设置好容量
void initQueueByArray(struct QueueLite *q, base_type *arr, base_type size)
{
    assert(q != NULL && arr != NULL);
    assert(size <= q->capacity);
    q->size = size;
    q->head = 0;
    q->tail = size;
    for(base_type i = 0; i < size; ++i)
    {
        q->ptr[i] = arr[i];
    }
}