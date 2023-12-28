#include "processControlBlock.h"

// 初始化PCB,回收PCB到系统时使用
void initPCB(struct ProcessControlBlock *pcb)
{
    assert(pcb != NULL);
    pcb->pid = MAX_PID;
    pcb->status = Undefined;
    if(pcb->page_table != NULL)
    {
        destroyPageTable(&(pcb->page_table));
    }
}

// 创建一个PCB
struct ProcessControlBlock * creatPCB()
{
    struct ProcessControlBlock *pcb = (struct ProcessControlBlock *)malloc(sizeof(struct ProcessControlBlock));
    assert(pcb != NULL);
    pcb->pid = MAX_PID;
    pcb->status = Undefined;
    pcb->page_table = creatPageTbale();
    return pcb;
}

// 销毁一个PCB
void destroyPCB(struct ProcessControlBlock **pcb)
{
    assert(pcb != NULL && *pcb != NULL);
    if((*pcb)->page_table != NULL)
    {
        destroyPageTable(&((*pcb)->page_table));
        // (*pcb)->page_table = NULL;
    }
    (*pcb)->pid = 0;
    (*pcb)->status = Undefined;
    free(*pcb);
    (*pcb) = NULL;
}

// 获得PCB中的页表
struct PageTable* getPTInPCB(const struct ProcessControlBlock *pcb)
{
    assert(pcb != NULL);
    return pcb->page_table;
}

// 获得PCB中的pid
base_type getPidInPCB(const struct ProcessControlBlock *pcb)
{
    assert(pcb != NULL);
    return pcb->pid;
}

// 获得PCB中的处理机状态
CPU_STATUS getStatusInPCB(const struct ProcessControlBlock *pcb)
{
    assert(pcb != NULL);
    return pcb->status;
}

// 设置PCB的pid
void setPCBPid(struct ProcessControlBlock *pcb, base_type pid)
{
    assert(pcb != NULL);
    assert(pid <= MAX_PID);
    pcb->pid = pid;
}