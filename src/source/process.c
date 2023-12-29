#include "process.h"

// 初始化进程
void initProcess(struct Process *proc)
{
    assert(proc != NULL);
    proc->pcb = NULL;
    proc->size = 0;
    proc->sys = NULL;
}

// 创建进程
struct Process *creatProcess(struct TinySystem *sys)
{
    assert(sys != NULL);
    // 首先进行系统判断  获取进程pid
    base_type pid = sys_allocatePid(sys);
    assert(pid != MAX_PID); // 无法创建新进程
    // 创建进程实体
    struct Process *proc = (struct Process *)malloc(sizeof(struct Process));
    assert(proc != NULL);
    // 初始化
    initProcess(proc);
    // 向操作系统注册，以获得系统资源
    proc->sys = sys; 
    // 通过pid获取对应PCB
    proc->pcb = sys->PCBs[pid];
    proc->pcb->status = Ready;
    // 设置pid
    setPCBPid(proc->pcb, pid);
    return proc;
}

// 装入程序 size代表装入程序的初始大小 创建进程后需要装入程序 如代码段
void loadProgram(struct Process *proc, address_type size)
{
    assert(proc != NULL);
    assert(proc->size == 0);
    assert(size != 0 && size <= MAX_SIZE_OF_PROGRAM); // 一个进程最大开辟4M用户空间
    // 装入程序后首先要在系统区减少一个PCB的大小
    setSysUsed(proc->sys->mem, (address_type)(getSysUsed(proc->sys->mem) + SIZE_OF_PCB));
    // 向主存申请内存,并在PCB中记录
    mem_allocate_user(sys_getMemory(proc->sys), proc->pcb, byte2Page(size));
    // 更新进程地址空间大小
    proc->size = size;
}

// 销毁进程
void destroyProcess(struct Process **proc)
{
    assert(proc != NULL);
    // 归还系统资源
    if((*proc)->sys != NULL)
    {
        // 归还pid
        sys_deallocatePid((*proc)->sys,getPidInPCB((*proc)->pcb));
        // 归还全部内存
        mem_deallocate_all_user((*proc)->sys->mem,(*proc)->pcb);
        // 初始化PCB
        initPCB((*proc)->pcb);
    }
    (*proc)->size = 0;
    // 销毁进程实体
    free(*proc);
    proc = NULL;
}

// 设置进程PCB
void setProcPCB(struct Process *proc, struct ProcessControlBlock *pcb)
{
    assert(proc != NULL && pcb != NULL);
    proc->pcb = pcb;
}


// 进程主动申请内存
address_type procAllocate(struct Process *proc, address_type size)
{
    assert(proc != NULL);
    assert(size <= (MAX_SIZE_OF_PROGRAM - proc->size));
    address_type addr = 0;
    address_type free = proc->pcb->page_table->size * SIZE_OF_PAGE - proc->size;
    address_type need = size - free;
    // 判断是否需要向主存申请内存, 如果页中有空闲空间，则不申请
    if(need > 0)
    {
        // 向主存申请内存,并在PCB中记录
        addr = mem_allocate_user(sys_getMemory(proc->sys), proc->pcb, byte2Page(need));
    }
    addr = proc->size;
    // 更新proc->size
    proc->size += size;
    return addr;
}

// 进程释放内存
// 由于设计比较简单，对于Page类没有额外设计
// 所以这里释放 [addr, addr + size]的内存是将对应的页面全部释放
// size 的单位 byte
// 设计未完成，不要调用该函数
void procDeallocate(struct Process *proc, address_type addr, address_type size)
{
    assert(proc != NULL);
    assert(addr < MAX_SIZE_OF_PROGRAM);
    assert(size < (MAX_SIZE_OF_PROGRAM - proc->size));
    address_type temp = addr + size;
    do
    {
        base_type npage = byte2Page(temp);
        // 向系统归还内存，并在PCB中修改对应页表
        mem_deallocate_user(proc->sys->mem, proc->pcb, npage);
        temp -= MIN(temp, 4 * _KB);
    } while (temp > addr);
    proc->size -= size;
}

// 获取进程PCB
struct ProcessControlBlock *getProcPCB(const struct Process *proc)
{
    assert(proc != NULL);
    return proc->pcb;
}

// 获取进程大小
address_type getProcSize(const struct Process *proc)
{
    assert(proc != NULL);
    return proc->size;
}

// 给出字节地址/数量，转换成页面地址/数量
base_type byte2Page(address_type size)
{
    // assert(size != 0 && size <= sizeof(base_type) * SIZE_OF_PAGE);
    return (base_type)((size + 4 * _KB -1)/(4 * _KB));
}

// 通过逻辑地址计算物理地址
address_type getRealAddr(const struct Process *proc, address_type logicAddr)
{
    assert(proc != NULL);
    assert(logicAddr < proc->size);
    address_type numberOfPage = ((logicAddr & (address_type)PART_OF_PAGE_NUMBER) >> 12);        // 逻辑页号
    assert(numberOfPage < getPageTableSize(proc->pcb->page_table));
    address_type offsetInsidPage = logicAddr & PART_OF_PAGE_INSIDE_OFFSET;                      // 页内偏移
    numberOfPage = getValueInPageTable(proc->pcb->page_table, (numberOfPage));                  // 替换为物理页号
    assert(numberOfPage != PAGE_NOT_USED);                                                      // 如果该页号已回收，则终止
    address_type realAddr = (numberOfPage << 12) | offsetInsidPage;
    return realAddr;
}

// 通过逻辑地址获取真实地址的指针
char *procGetMemoryHandle(const struct Process *proc, address_type addr)
{
    assert(proc != NULL);
    assert(addr <= MAX_SIZE_OF_PROGRAM);
    addr = getRealAddr(proc, addr); // 物理地址
    return (char *)(getMemory(proc->sys->mem) + addr);
}

// 将进程映像写入磁盘
bool_type writeProc2Disk(const struct Process *proc, const char *fileName)
{
    assert(proc != NULL && fileName != NULL);
    address_type size = proc->size;
    FILE *fp = fopen(fileName,"wb");
    if(fp == NULL) return TYPE_FALSE;
    for(address_type i = 0;i < size;)
    {
        address_type ssize = MIN(size - i, SIZE_OF_PAGE);
        fwrite(procGetMemoryHandle(proc, i), ssize, 1, fp);
        i += SIZE_OF_PAGE;
    }
    fclose(fp);
    return TYPE_TRUE;
}

// 从磁盘恢复进程映像
struct Process *readProcFromDisk(struct TinySystem *sys, const char *fileName)
{
    assert(sys != NULL && fileName != NULL);
    char *temp = (char *)malloc(MAX_SIZE_OF_PROGRAM);
    assert(temp != NULL);
    // 关联文件
    FILE *fp = fopen(fileName, "rb");
    // 读取文件
    size_t size = fread(temp, sizeof(char), MAX_SIZE_OF_PROGRAM + 1, fp);
    if(size > MAX_SIZE_OF_PROGRAM || size > (MEMORY_CAPACITY - USER_ADDR_OFFSET - (sys->mem->user_used))) return NULL; // 文件太大，无法创建
    // 创建进程实体
    struct Process *proc = creatProcess(sys);
    loadProgram(proc,size); //申请len大小空间
    // 复制内存
    char* source = temp;
    for(address_type i = 0; i < size;)
    {
        address_type ssize = MIN(size - i, SIZE_OF_PAGE);
        memmove(procGetMemoryHandle(proc, i), source, ssize);
        i += SIZE_OF_PAGE;
        source += SIZE_OF_PAGE;
    }
    free(temp);
    temp = source = NULL;
    fclose(fp);
    return proc;
}