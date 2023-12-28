#include "system.h"

// 初始化系统
void initSystem(struct TinySystem* sys)
{
    assert(sys != NULL);
    sys->mem = NULL;
    sys->pidQueue = NULL;
    sys->PCBs = NULL;
}

// 创建一个系统
struct TinySystem *creatSystem(bool_type flag)
{
    struct TinySystem *sys = (struct TinySystem *)malloc(sizeof(struct TinySystem));
    assert(sys != NULL);
    initSystem(sys);
    // 建立PCB队列 对应索引即是PID
    sys->PCBs = (struct ProcessControlBlock **)malloc(sizeof(struct ProcessControlBlock *) * MAX_PID);
    assert(sys->PCBs != NULL);
    for(base_type i = 0; i < MAX_PID; ++i)
    {
        sys->PCBs[i]=creatPCB();
    }
    // 创建主存储器
    sys->mem = creatMemory(flag);
    // 创建空闲pid队列
    sys->pidQueue = creatQueue(MAX_PID);
    for(base_type i = 0; i < MAX_PID; ++i)
    {
        if(queue_push(sys->pidQueue, i) == TYPE_FALSE) // 如果失败，回退
        {
            destroyMemory(&(sys->mem));
            destroyQueue(&(sys->pidQueue));
            return NULL;
        }
    }
    return sys;
}

// 销毁一个系统
void destroySystem(struct TinySystem **sys)
{
    assert(*sys != NULL);
    if((*sys)->mem != NULL)
        destroyMemory(&((*sys)->mem));
    if((*sys)->pidQueue != NULL)
        destroyQueue(&((*sys)->pidQueue));
    if((*sys)->PCBs != NULL)
    {
        for(base_type i = 0; i < MAX_PID; ++i)
        {
            destroyPCB(&((*sys)->PCBs[i]));
        }
    }
    free(*sys);
    *sys = NULL;
}

// 获取空闲PCB队列
struct QueueLite *sys_getPidQueue(const struct TinySystem *sys)
{
    assert(sys != NULL);
    return sys->pidQueue;
}

// 获取主存储器句柄
struct Memory *sys_getMemory(const struct TinySystem *sys)
{
    assert(sys != NULL);
    return sys->mem;
}

// 获取一个空闲pid
base_type sys_allocatePid(struct TinySystem *sys)
{
    assert(sys != NULL);
    if(queue_empty(sys->pidQueue) == TYPE_FALSE)
    {
        base_type ret = queue_front(sys->pidQueue);
        queue_pop(sys->pidQueue);
        return ret;
    }
    return MAX_PID; //pid=MAX_PID表示不合法
}

// 回收一个空闲pid
void sys_deallocatePid(struct TinySystem *sys, base_type pid)
{
    assert(sys != NULL);
    assert(pid < MAX_PID);
    queue_push(sys->pidQueue,pid);
}