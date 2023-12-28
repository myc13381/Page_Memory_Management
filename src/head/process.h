/*
 * 包含进程模型数据结构,地址计算,文件存取
 * 地址采用32位整型数描述，因为Page大小为4K，所以页内偏移量位12位，剩下20位是页号
 * 我们约定主存空间为32M，所以最多32M/4K=8K个页面，其实只需要13位
*/

#ifndef PAGE_MEMORY_MANAGEMENT_PROCESS
#define PAGE_MEMORY_MANAGEMENT_PROCESS

#include "head.h"
#include "processControlBlock.h"
#include "system.h"

#define MAX_SIZE_OF_PROGRAM         4 * 1024 * 1024

#define PART_OF_PAGE_NUMBER         0xFFFFF000  // 11111111111111111111000000000000
#define PART_OF_PAGE_INSIDE_OFFSET  0x00000FFF  // 00000000000000000000111111111111


struct Process
{
    struct ProcessControlBlock *pcb; // 进程的PCB
    address_type size; // 进程的占用的内存总大小
    struct TinySystem *sys; //系统，单例对象
};

// 初始化进程
void initProcess(struct Process *proc);

// 创建进程
struct Process *creatProcess(struct TinySystem *sys);

// 装入程序 size代表装入程序的初始大小
void loadProgram(struct Process *proc, address_type size);

// 销毁进程
void destroyProcess(struct Process **proc);

// 设置进程PCB
void setProcPCB(struct Process *proc, struct ProcessControlBlock *pcb);

// 进程主动申请内存
address_type procAllocate(struct Process *proc, address_type size);

// 进程释放内存
// 设计未完成，不要调用该函数
void procDeallocate(struct Process *proc, address_type addr, address_type size);

// 获取进程PCB
struct ProcessControlBlock *getProcPCB(const struct Process *proc);

// 获取进程大小
address_type getProcSize(const struct Process *proc);

// 给出字节地址/数量，转换成页面地址/数量
base_type byte2Page(address_type size);

// 通过逻辑地址计算物理地址
address_type getRealAddr(const struct Process *proc, address_type logicAddr);

// 通过逻辑地址获取真实地址的指针
char *procGetMemoryHandle(const struct Process *proc, address_type addr);

// 将进程映像写入磁盘
bool_type writeProc2Disk(const struct Process *proc, const char *fileName);

// 从磁盘恢复进程映像
struct Process *readProcFromDisk(struct TinySystem *sys, const char *fileName);

#endif // PAGE_MEMORY_MANAGEMENT_PROCESS