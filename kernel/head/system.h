/*
 * 操作系统简单模型，仅包含PCB队列，主存储器,pid分配器
*/

#ifndef PAGE_MEMORY_MANAGEMENT_SYSTEM
#define PAGE_MEMORY_MANAGEMENT_SYSTEM

#include "head.h"
#include "memory.h"
#include "queueLite.h"
#include "processControlBlock.h"

// 系统模型
struct TinySystem
{
    struct QueueLite *pidQueue;         // 空闲Pid队列
    struct Memory *mem;                 // 主存储器实例
    struct ProcessControlBlock **PCBs;  // PCB队列
};

// 初始化系统
void initSystem(struct TinySystem* sys);

// 创建一个系统 参数使用采用顺序存储
struct TinySystem *creatSystem(bool_type flag);

// 销毁一个系统
void destroySystem(struct TinySystem **sys);

// 获取空闲PCB队列
struct QueueLite *sys_getPCBQueue(const struct TinySystem *sys);

// 获取主存储器句柄
struct Memory *sys_getMemory(const struct TinySystem *sys);

// 获取一个空闲pid
base_type sys_allocatePid(struct TinySystem *sys);

// 回收一个空闲pid
void sys_deallocatePid(struct TinySystem *sys, base_type pid);

#endif // PAGE_MEMORY_MANAGEMENT_SYSTEM