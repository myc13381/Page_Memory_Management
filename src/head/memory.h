/********************************************************************************
 * 主存类 Memory 和一些内存管理的函数
 * 总内存为32Mbyte，至少需要使用26位来描述，这里使用uint32_t
 * 整个主存分为两段，前一段是system区，后面是用户区，通过offset区分
 * |-----------------------------| 0x01FFFFFF 32M-1 Page 8*1024-1
 * |                             |
 * |------------user-------------|
 * |                             |
 * |                             |
 * |                             |
 * |                             |
 * |                             |
 * |                             |
 * |============offset===========| 0x00800000 8M    Page 2*1024
 * |                             |
 * |                             |
 * |                             |
 * |-----------system------------|
 * |                             |
 * |-----------------------------| 0x00000000       Page 0
 * 整个系统可用内存空间为32M，最多需要32M/4K=8K个page
 * 系统占用8M，用户区24M,用户区页面为24M/4K=6K
 * 每一个Page大小为4Kbyte，系统按字节编址，地址长度为32位
 * 一个页表最多有1024项，每一项指向一个4K大小的页，因此一个页表最多管理4K*1024 =4M内存
 * 假设每个进程最多只允许申请4M内存，所以只需要一级页表
 * |        页号P        |    偏移量        |
 ********************************************************************************/

#ifndef PAGE_MEMORY_MANAGEMENT_MEMORY
#define PAGE_MEMORY_MANAGEMENT_MEMORY

#include "head.h"
#include "queueLite.h"
#include "processControlBlock.h"
#include "pageTable.h"

#define MEMORY_CAPACITY         (32 * 1024 * 1024)              // 主存最大容量
#define USER_ADDR_OFFSET        (8 * 1024 * 1024)               // 用户区偏移量 8M 
#define NUM_OF_PAGE_FOR_USER    (6 * 1024)                      //主存中用户区页面的数量，系统区就不分页了
#define START_PAGE_OF_USER      (2 * 1024)                      //用户区起始页号

/* Memory 代表主存，单例对象，用来描述主存*/
struct Memory
{
    char *memory; // 主存
    address_type offset; // 用户段的偏移量
    address_type sys_used; //系统区已使用量
    address_type user_used;// 用户区已使用量
    struct QueueLite *mem_queue; //用于存放空闲的页的页号
};

// 初始化主存，包括对主存进行分页操作 flag代表分页是否按顺序插入队列
void intiMemory(struct Memory *mem, bool_type flag);

// 创建主存 flag代表分页是否按顺序插入队列
struct Memory *creatMemory(bool_type flag);

// 销毁主存
void destroyMemory(struct Memory **mem);

// 获取用户段偏移量
address_type getMemOffset(const struct Memory *mem);

// 获取系统区已使用量
address_type getSysUsed(const struct Memory *mem);

// 获取用户区已使用量
address_type getUserUsed(const struct Memory *mem);

// 设置系统区已使用量
bool_type setSysUsed(struct Memory *mem, address_type nvalue);

// 设置用户区已使用量
bool_type setUserUsed(struct Memory *mem, address_type value);

// 获取用户区剩余页数
base_type getUserRemainder(const struct Memory *mem);

// 获取队列
struct QueueLite *getQueueOfMen(const struct Memory *mem);

//获取主存首地址
char *getMemory(const struct Memory *mem);


// 内存操作
bool_type memOpt(struct Memory *mem, address_type origin, address_type destination, base_type size);

// 内存中读写操作本质是内存中数据的复制/移动
bool_type memRead(struct Memory *mem, address_type origin, address_type destination, base_type size);
bool_type memWrite(struct Memory *mem, address_type origin, address_type destination, base_type size);


// ！！！下面按个函数是内存分配的操作，操作比较危险，必须要在Process相关函数内部调用，并保证操作的合法性

// 进程申请内存，传入进程pcb以及需要内存的大小(单位:页数)，返回申请的首地址
address_type mem_allocate_user(struct Memory *mem, struct ProcessControlBlock *pcb, base_type n);

// 进程释放内存，传入pcb,和pcb对应释放内存的页号(逻辑地址) 
bool_type mem_deallocate_user(struct Memory *mem, struct ProcessControlBlock *pcb, base_type npage);

// 释放进程所有的内存，进程销毁时调用
bool_type mem_deallocate_all_user(struct Memory *mem, struct ProcessControlBlock *pcb);

#endif // PAGE_MEMORY_MANAGEMENT_MEMORY