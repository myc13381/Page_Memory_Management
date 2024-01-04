/*
 * 实现PCB数据结构
*/

#ifndef PAGE_MEMORY_MANAGEMENT_PCB
#define PAGE_MEMORY_MANAGEMENT_PCB

#include "head.h"
#include "pageTable.h"

#define SIZE_OF_PCB  (sizeof(struct ProcessControlBlock))           // 一个PCB的大小(单位:byte)

typedef enum {Ready, Execute, Block, Undefined} CPU_STATUS;         // Undefined表示未知

// PCB
struct ProcessControlBlock
{
    base_type pid;                                              // pid==MAX_PID 表示此PCB未被使用，否则代表进程编号
    CPU_STATUS status;                                          // 处理机状态就绪，执行，阻塞，未知
    struct PageTable *page_table;                               // 指向内存中页表的指针
};

// 初始化PCB
void initPCB(struct ProcessControlBlock *pcb);

// 创建一个PCB
struct ProcessControlBlock * creatPCB();

// 销毁一个PCB
void destroyPCB(struct ProcessControlBlock **pcb);

// 获得PCB中的页表
struct PageTable* getPTInPCB(const struct ProcessControlBlock *pcb);

// 获得PCB中的pid
base_type getPidInPCB(const struct ProcessControlBlock *pcb);

// 获得PCB中的处理机状态
CPU_STATUS getStatusInPCB(const struct ProcessControlBlock *pcb);

// 设置PCB的pid
void setPCBPid(struct ProcessControlBlock *pcb, base_type pid);

#endif // PAGE_MEMORY_MANAGEMENT_PCB