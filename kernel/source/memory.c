#include "memory.h"

// 初始化主存，包括对主存进行分页操作
void intiMemory(struct Memory *mem, bool_type flag)
{
    assert( mem != NULL);
    assert( mem->memory != NULL);// 只允许初始化一次
    assert( mem->mem_queue != NULL);
    mem->offset = USER_ADDR_OFFSET;
    mem->sys_used = 1 * 1024 * 1024; //假设系统其他资源已经使用1M内存
    mem->user_used = 0;
    //resetQueueSize(mem->mem_queue,NUM_OF_PAGE_FOR_USER); //设置队列长度
    // 开始分页，使用队列
    if(flag == TYPE_FALSE)
    {
        // 四个page为一组，计算indexOfPage%4的结果，相同的为一组取结果为组号，组号从0到3的依次插入
        for(base_type i = 0; i < 4; ++i)
        {
            for(base_type j = i; j < NUM_OF_PAGE_FOR_USER; j+=4)
            {
                queue_push(mem->mem_queue,j + START_PAGE_OF_USER);
            }
        }
    }
    else
    {
        for(base_type i = 0; i < NUM_OF_PAGE_FOR_USER; ++i)
        {
            queue_push(mem->mem_queue,i + START_PAGE_OF_USER);// 按顺序插入页号
        }
    }
    return;
}

// 创建主存
struct Memory *creatMemory(bool_type flag)
{
    struct Memory *mem = (struct Memory *)malloc(sizeof(struct Memory));
    assert(mem != NULL);
    mem->mem_queue = creatQueue(NUM_OF_PAGE_FOR_USER);
    mem->memory = (char *)malloc(MEMORY_CAPACITY); // 开辟主存内存
    assert(mem->memory != NULL);
    intiMemory(mem, flag);
    return mem;
}

// 销毁主存
void destroyMemory(struct Memory **mem)
{
    assert(mem != NULL && *mem != NULL);
    if((*mem)->memory != NULL)
    {
        free((*mem)->memory);
        (*mem)->memory = NULL;
    }
    destroyQueue(&((*mem)->mem_queue));
    (*mem)->offset = 0;
    (*mem)->sys_used = 0;
    (*mem)->user_used = 0;
    *mem = NULL;
    return;
}

// 获取用户段偏移量
address_type getMemOffset(const struct Memory *mem)
{
    assert(mem != NULL);
    return mem->offset;
}

// 获取系统区已使用量
address_type getSysUsed(const struct Memory *mem)
{
    assert(mem != NULL);
    return mem->sys_used;
}

// 获取用户区已使用量
address_type getUserUsed(const struct Memory *mem)
{
    assert(mem != NULL);
    return mem->user_used;
}

// 设置系统区已使用量
bool_type setSysUsed(struct Memory *mem, address_type value)
{
    assert(mem != NULL);
    assert(value <= USER_ADDR_OFFSET);
    mem->sys_used = value;
    return TYPE_TRUE;
}

// 设置用户区已使用量
bool_type setUserUsed(struct Memory *mem, address_type value)
{
    assert(mem != NULL);
    assert(value <= (address_type)(MEMORY_CAPACITY - USER_ADDR_OFFSET));
    mem->user_used = value;
    return TYPE_TRUE;
}

// 获取用户区剩余页数
base_type getUserRemainder(const struct Memory *mem)
{
    assert(mem != NULL);
    address_type a = mem->user_used;
    address_type b = SIZE_OF_PAGE;
    address_type c = a/b;
    return (base_type)(NUM_OF_PAGE_FOR_USER - c);
}

// 获取队列
struct QueueLite *getQueueOfMen(const struct Memory *mem)
{
    assert(mem != NULL);
    return mem->mem_queue;
}

//获取主存首地址
char *getMemory(const struct Memory *mem)
{
    assert(mem != NULL);
    return mem->memory;
}


// 内存操作
bool_type memOpt(struct Memory *mem, address_type origin, address_type destination, base_type size)
{
    assert(mem != NULL);
    assert(mem->memory != NULL);
    if( origin >= mem->offset && origin <= MEMORY_CAPACITY &&               // 源地址合法
        destination >= mem->offset && destination <= MEMORY_CAPACITY &&     // 目的地址合法
        size <= (MEMORY_CAPACITY - destination)                             // 有足够多的大小进行复制
      )
    {
        return TYPE_FALSE;
    }
    void *ret = memmove(mem->memory + origin, mem->memory + destination, size);
    if(ret == NULL) return TYPE_FALSE; // memmove失败
    else return TYPE_TRUE;
}

// 内存中读写操作本质是内存中数据的复制/移动
bool_type memRead(struct Memory *mem, address_type origin, address_type destination, base_type size)
{
    return memOpt(mem, origin, destination, size);
}

bool_type memWrite(struct Memory *mem, address_type origin, address_type destination, base_type size)
{
    return memOpt(mem, origin, destination, size);
}

// 进程申请内存，传入进程pcb以及需要内存的大小(单位:byte)
address_type mem_allocate_user(struct Memory *mem, struct ProcessControlBlock *pcb, base_type n)
{
    // 需要上锁

    assert(mem != NULL && pcb != NULL);
    assert(n <= getUserRemainder(mem));
    // 获取首地址，该地址需要返回
    base_type npage = writePageTable(pcb->page_table, queue_front(mem->mem_queue));
    queue_pop(mem->mem_queue);
    // 后续内存申请
    for(base_type i = 1; i < n; ++i)
    {
        writePageTable(pcb->page_table, queue_front(mem->mem_queue));
        queue_pop(mem->mem_queue);
    }
    mem->user_used += (address_type)(n * SIZE_OF_PAGE);
    //需要解锁
    
    // 返回逻辑地址
    return (npage << 12);
}

// 进程释放内存，传入传入进程pcb以及释放内存的页号(逻辑)
bool_type mem_deallocate_user(struct Memory *mem, struct ProcessControlBlock *pcb, base_type npage)
{
    // 需要上锁

    assert(mem != NULL && pcb != NULL);
    assert(npage < PAGE_CAPACITY);
    // 回收内存
    if(queue_push(mem->mem_queue, getValueInPageTable(pcb->page_table,npage)) != TYPE_TRUE)
        return TYPE_FALSE;
    // 回收空闲PCB中页表的空闲页表项
    recyclePageElem(pcb->page_table,npage);
    // 更新内存使用情况
    mem->user_used -= MIN(mem->user_used, (address_type)SIZE_OF_PAGE);
    return TYPE_TRUE;

    // 需要解锁
}

// 释放进程所有的内存，进程销毁时调用
bool_type mem_deallocate_all_user(struct Memory *mem, struct ProcessControlBlock *pcb)
{
    // 需要上锁

    assert(mem != NULL && pcb != NULL);
    base_type value = 0;
    for(base_type i = 0; i < PAGE_CAPACITY; ++i)
    {
        value = getValueInPageTable(pcb->page_table,i);
        if(value != PAGE_NOT_USED)
        {
            if(mem_deallocate_user(mem,pcb,i) != TYPE_TRUE)
                return TYPE_FALSE;
        }
    }
    return TYPE_TRUE;

    // 需要解锁
}
