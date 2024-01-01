/*
 * Unit Test
 */

#include <stdio.h>
#include "head.h"
#include "pageTable.h"
#include "queueLite.h"
#include "processControlBlock.h"
#include "memory.h"
#include "system.h"
#include "process.h"
#include "client.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format)                                                           \
    do                                                                                                             \
    {                                                                                                              \
        test_count++;                                                                                              \
        if (equality)                                                                                              \
            test_pass++;                                                                                           \
        else                                                                                                       \
        {                                                                                                          \
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual); \
            main_ret = 1;                                                                                          \
        }                                                                                                          \
    } while (0)

#define EXPECT_EQ_UINT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%lu")
#define EXPECT_TRUE(actual) EXPECT_EQ_BASE((actual) != TYPE_FALSE, "true", "false", "%s")
#define EXPECT_FALSE(actual) EXPECT_EQ_BASE((actual) == TYPE_FALSE, "false", "true", "%s")

void testPageTable();
void testQueue();
void testPCB();
void testMemory();
void testSystem();
void testProcess();
void testShow();
void testFileRW();
void testClient();
void testUI();
void testStart();

#ifdef UNIT_TEST

int main()
{
    printf("unit test, please wait...\n");
    testPageTable();
    testQueue();
    testPCB();
    testMemory();
    testSystem();
    testProcess();
    testShow();
    testFileRW();
    testClient();
    testUI();
    //// testStart();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return 0;
}

#endif

void testPageTable()
{
    printf("|*********************************|\n");
    printf("|****    test PageTable       ****|\n");
    printf("|******   please wait...   *******|\n");
    struct PageTable *p = creatPageTbale();
    EXPECT_EQ_UINT(0, getPageTableSize(p));
    EXPECT_FALSE(getPageTablePtr(p) == NULL);
    writePageTable(p, 1);
    EXPECT_EQ_UINT(1, getValueInPageTable(p, 0));
    modifyPageTable(p, 0, 5);
    EXPECT_EQ_UINT(5, getValueInPageTable(p, 0));
    // 剩下的页值应该都是宏 PAGE_NOT_USED
    for (base_type i = 1; i < PAGE_CAPACITY; ++i)
    {
        EXPECT_EQ_UINT(PAGE_NOT_USED, (p->table)[i]);
        EXPECT_EQ_UINT(i, (p->queue->ptr)[i]);
    }
    recyclePageElem(p, 0);
    EXPECT_EQ_UINT(PAGE_NOT_USED, getValueInPageTable(p, 0));
    destroyPageTable(&p);
    printf("|****  test PageTable finish  ****|\n");
    printf("|*********************************|\n\n");
}

void testQueue()
{
    printf("|*********************************|\n");
    printf("|****    test QueueLite       ****|\n");
    printf("|******   please wait...   *******|\n");
    struct QueueLite *q = creatQueue(10);
    EXPECT_EQ_UINT(0, getQueueSize(q));
    for (int i = 0; i < 5; ++i)
        queue_push(q, i);
    EXPECT_EQ_UINT(5, getQueueSize(q));
    EXPECT_FALSE(queue_empty(q));
    for (int i = 0; i < 5; ++i)
    {
        EXPECT_TRUE(queue_front(q) == i);
        queue_pop(q);
    }
    EXPECT_TRUE(queue_empty(q));
    destroyQueue(&q);
    EXPECT_TRUE(q == NULL);
    q = creatQueue(20);
    base_type arr[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    initQueueByArray(q, arr, 10);
    for (int i = 0; i < 5; ++i)
    {
        EXPECT_EQ_UINT(i, queue_front(q));
        queue_pop(q);
    }
    destroyQueue(&q);
    EXPECT_TRUE(q == NULL);
    printf("|****  test QueueLite finish  ****|\n");
    printf("|*********************************|\n\n");
}

void testPCB()
{
    printf("|*********************************|\n");
    printf("|*******    test PCB       *******|\n");
    printf("|******   please wait...   *******|\n");
    struct ProcessControlBlock *pcb = creatPCB();
    EXPECT_FALSE(pcb == NULL);
    EXPECT_FALSE(getPTInPCB(pcb) == NULL);
    EXPECT_EQ_UINT(MAX_PID, getPidInPCB(pcb));
    EXPECT_TRUE(getStatusInPCB(pcb) == Undefined);
    destroyPCB(&pcb);
    EXPECT_TRUE(pcb == NULL);
    printf("|*******  test PCB finish  *******|\n");
    printf("|*********************************|\n\n");
}

void testMemory()
{

    printf("|*********************************|\n");
    printf("|*****    test Memory       ******|\n");
    printf("|******   please wait...   *******|\n");
    struct Memory *mem = creatMemory(ORDER_MODE);
    EXPECT_EQ_UINT((address_type)(8 * 1024 * 1024), getMemOffset(mem));
    EXPECT_EQ_UINT(getUserUsed(mem), 0);
    setUserUsed(mem, (address_type)1024);
    EXPECT_EQ_UINT(getUserUsed(mem), (address_type)(1024));
    EXPECT_EQ_UINT((address_type)(1 * 1024 * 1024), getSysUsed(mem));
    setSysUsed(mem, (address_type)(getSysUsed(mem) + 1024 * 1024));
    EXPECT_EQ_UINT(2 * 1024 * 1024, getSysUsed(mem));
    struct QueueLite *q = mem->mem_queue;
    EXPECT_EQ_UINT(NUM_OF_PAGE_FOR_USER, q->size);
#ifdef MEMORY_NOT_ORDER // 乱序插入队列
    for (base_type i = 0; i < 4; ++i)
    {
        for (base_type j = 0; j < NUM_OF_PAGE_FOR_USER; j += 4)
        {
            EXPECT_EQ_UINT(j + START_PAGE_OF_USER + i, queue_front(q));
            queue_pop(q);
        }
    }
    EXPECT_TRUE(queue_empty(q) == TYPE_TRUE);
#else
    for (base_type i = 0; i < NUM_OF_PAGE_FOR_USER; ++i)
    {
        EXPECT_EQ_UINT(i + START_PAGE_OF_USER, queue_front(q));
        queue_pop(q);
    }
    EXPECT_TRUE(queue_empty(q) == TYPE_TRUE);
#endif
    address_type address = 16 * 1024 * 1024; // 随便取一个地址测试
    char *memory = getMemory(mem);
    *(base_type *)memory = (base_type)(64 * 1024);
    memWrite(mem, address, address + 32, sizeof(base_type));
    EXPECT_EQ_UINT((base_type)(64 * 1024), *(base_type *)(memory + address + 32));
    memRead(mem, address + 32, address + 33, sizeof(base_type));
    EXPECT_EQ_UINT((base_type)(64 * 1024), *(base_type *)(memory + address + 33));
    destroyMemory(&mem);

    mem = creatMemory(ORDER_MODE);
    struct ProcessControlBlock *pcb = creatPCB();
    EXPECT_EQ_UINT(NUM_OF_PAGE_FOR_USER, getUserRemainder(mem));
    mem_allocate_user(mem, pcb, 512);
    EXPECT_EQ_UINT(512, getPageTableSize(pcb->page_table));
#ifdef MEMORY_NOT_ORDER
    for (base_type i = 0; i < 512; ++i)
    {
        // ....
    }
#else
    for (base_type i = 0; i < 512; ++i)
    {
        EXPECT_EQ_UINT(i + START_PAGE_OF_USER, getValueInPageTable(pcb->page_table, i));
    }
#endif
    EXPECT_EQ_UINT(NUM_OF_PAGE_FOR_USER - 512, getUserRemainder(mem));
    mem_deallocate_user(mem, pcb, 0);
    EXPECT_EQ_UINT(PAGE_NOT_USED, pcb->page_table->table[0]);
    EXPECT_TRUE(PAGE_NOT_USED != getValueInPageTable(pcb->page_table, 1));
    EXPECT_EQ_UINT(NUM_OF_PAGE_FOR_USER - 511, getUserRemainder(mem));
    mem_deallocate_all_user(mem, pcb);
    EXPECT_EQ_UINT(NUM_OF_PAGE_FOR_USER, getUserRemainder(mem));
    destroyPCB(&pcb);
    destroyMemory(&mem);
    printf("|*****  test Memory finish  ******|\n");
    printf("|*********************************|\n\n");
}

void testSystem()
{
    printf("|*********************************|\n");
    printf("|******    test System      ******|\n");
    printf("|******   please wait...   *******|\n");
    struct TinySystem *sys = creatSystem(ORDER_MODE);
#ifdef MEMORY_NOT_ORDER

#else
    for (base_type i = 0; i < MAX_PID; ++i)
    {
        struct ProcessControlBlock *pcb = sys->PCBs[i];
        EXPECT_TRUE(pcb != NULL);
    }
#endif
    EXPECT_EQ_UINT(MAX_PID, getQueueSize(sys->pidQueue));
    base_type pid = sys_allocatePid(sys);
    EXPECT_EQ_UINT(0, pid);
    EXPECT_EQ_UINT(MAX_PID - 1, getQueueSize(sys->pidQueue));
    sys_deallocatePid(sys, 0);
    EXPECT_EQ_UINT(MAX_PID, getQueueSize(sys->pidQueue));
    destroySystem(&sys);
    printf("|*****  test System finish  ******|\n");
    printf("|*********************************|\n\n");
}

void testProcess()
{
    printf("|*********************************|\n");
    printf("|******    test Process     ******|\n");
    printf("|******   please wait...   *******|\n");
    // 测试进程类需要首先创建系统
    struct TinySystem *sys = creatSystem(ORDER_MODE);
    struct Process *procA = creatProcess(sys);
    loadProgram(procA, 8 * _KB);
    // 进程申请内存测试
    procAllocate(procA, 3 * _KB);
    EXPECT_EQ_UINT((8 + 3) * _KB, getProcSize(procA));
#ifdef MEMORY_NOT_ORDER
    for (base_type i = 0; i < byte2Page(getProcSize(procA)); ++i)
    {
        EXPECT_EQ_UINT(START_PAGE_OF_USER + i * 4, getValueInPageTable(procA->pcb->page_table, i));
    }
#else
    for (base_type i = 0; i < byte2Page(getProcSize(procA)); ++i)
    {
        EXPECT_EQ_UINT(i + START_PAGE_OF_USER, getValueInPageTable(procA->pcb->page_table, i));
    }
#endif
    // 地址计算测试 这里只有一个进程，所以内存在物理上也是连续的
#ifdef MEMORY_NOT_ORDER

#else
    for (base_type i = 1023; i < 8193; ++i)
    {
        address_type addr = getRealAddr(procA, i);
        EXPECT_EQ_UINT(USER_ADDR_OFFSET + i, addr);
    }
#endif
    // 测试数据存取
    char *handle = procGetMemoryHandle(procA, 0);
    EXPECT_TRUE(handle = sys->mem->memory + USER_ADDR_OFFSET);
    char *ch = handle;
    for (base_type i = 0; i < 128; ++i)
    {
        *ch = i;
        ch += sizeof(base_type) / sizeof(char);
        EXPECT_EQ_UINT(i, *(base_type *)procGetMemoryHandle(procA, i * sizeof(base_type) / sizeof(char)));
    }
    // 进程释放内存测试
    procDeallocate(procA, 0, 1 * _KB);
    EXPECT_EQ_UINT((8 + 3 - 1) * _KB, getProcSize(procA));
    destroyProcess(&procA);
    destroySystem(&sys);
    printf("|*****  test Process finish ******|\n");
    printf("|*********************************|\n\n");
}

void testShow()
{
    printf("**************test Show**************\n");
    struct TinySystem *sys = creatSystem(ORDER_MODE);
    struct Process *procB = creatProcess(sys);
    struct Process *procC = creatProcess(sys);
    struct Process *procD = creatProcess(sys);
    loadProgram(procB, 32 * _KB);
    loadProgram(procC, 16 * _KB);
    loadProgram(procD, 128 * _KB);
    procAllocate(procB, 256 * _KB);
    procAllocate(procC, 521 * _KB);
    procAllocate(procD, 1024 * _KB);
    showProc(procB);
    showProc(procC);
    showProc(procD);
    destroyProcess(&procB);
    destroyProcess(&procC);
    destroyProcess(&procD);
    destroySystem(&sys);
}

void testFileRW()
{
    printf("**************testFileRW************************\n");
    const char *fileName = "F:\\project\\PageMemoryManagement\\procImage\\procTest";
    struct TinySystem *sys = creatSystem(ORDER_MODE);
    struct Process *procB = creatProcess(sys);
    loadProgram(procB, 32 * _KB);
    procAllocate(procB, 256 * _KB);
    showProc(procB);
    // 测试数据写入文件
    char *mem = procGetMemoryHandle(procB, 0);
    for (address_type i = 0; i < 1024; ++i)
    {
        *(mem + i) = (char)('a' + (i % 26));
    }
    writeProc2Disk(procB, fileName);
    destroyProcess(&procB);
    procB = readProcFromDisk(sys, fileName); // 从磁盘读取一个进程映像相当于创建一个新进程，所以最后需要
    // 测试文件读取数据
    mem = procGetMemoryHandle(procB, 0);
    for (address_type i = 0; i < 1024; ++i)
    {
        EXPECT_TRUE(*(mem + i) == (char)('a' + (i % 26)));
    }
    destroyProcess(&procB);
    destroySystem(&sys);
}

void testClient()
{
    printf("**************testClient************************\n");
    struct TinySystem *sys = client_initSystem(ORDER_MODE);
    struct Process **procList = malloc(sizeof(struct Process *) * MAX_PID);
    char **procNameList = malloc(sizeof(char *) * MAX_PID);
    for (int i = 0; i < MAX_PID; ++i)
    {
        procList[i] = NULL;
        procNameList[i] = "default";
    }
    client_showMenu(NULL);
    client_showSystemMessage(sys, NULL);
    for (base_type i = 0; i < 3; ++i)
    {
        client_creatProcess(sys, procList, (address_type)(4 * (i + 1) * _KB));
    }
    client_showProcessList((const struct Process **)procList, NULL);
    client_procAllocate(procList, 0, 16 * _KB);
    for (base_type i = 0; i < 3; ++i)
    {
        client_showProcessMessage(procList[i], NULL);
    }
    client_showSystemMessage(sys, NULL);
    for (base_type i = 0; i < 3; ++i)
    {
        client_destroyProcess(procList, i);
    }
    free(procList);
    free(procNameList);
    client_shutdownSystem(&sys);
    free(sys);
}

void testUI()
{
    // 获得系统指针
    struct TinySystem *sys = client_initSystem(ORDER_MODE);
    // 创建进程列表指针
    struct Process **procList = NULL;
    // 初始化进程列表指针
    client_init(&procList);
    // 测试菜单展示
    client_showMenu(NULL);
    // 测试帮助命令
    client_help();
    // 查看pid为3的进程信息，应该返回错误信息
    client_sp((const struct Process **)procList, 3);
    // 创建进程，初始内存4M
    client_cp(sys, procList, 4 * _MB);
    // 查看进程列表，应该只有一项
    client_ls((const struct Process **)procList);
    // 查看pid=0的进程信息
    client_sp((const struct Process **)procList, 0);
    // 为pid=0的进程申请4KB内存，应该申请失败，因为单个进程可用内存已满
    client_pa(procList, 0, 4 * _KB);
    // 查看pid=1的进程信息，此进程暂未创建，应报出错误信息
    client_sp((const struct Process **)procList, 1);
    // 创建进程，初始内存4KB
    client_cp(sys, procList, 4 * _KB);
    // 查看pid=1的进程信息
    client_sp((const struct Process **)procList, 1);
    // 将pid=0的进程内存存入磁盘，命名为procTestA,内容应该是a->z->a的循环，且大小为4MB
    const char *name = "procTestA";
    client_sd(procList, 0, name);
    // 将procTestA文件作为进程映像载入
    // 尝试载入进程映像，但是只有前4次会成功，后面会因为用户空间不足而创建失败
    for (base_type i = 0; i < 7; ++i)
    {
        client_ld(sys, procList, name);
    }
    // 创建新进程，初始内存为4MB，均不会成功，因为用户空间不足而创建失败
    for (base_type i = 0; i < 7; ++i)
    {
        client_cp(sys, procList, 4 * _MB);
    }
    // 为pid=1的进程新开辟4K内存
    client_pa(procList, 1, 4 * _KB);
    // 查看系统信息
    client_ss((const struct TinySystem *)sys);
    // 销毁pid=1的进程，此时用户剩余4M可用空间，第二次销毁失败，因为不能摧毁不存在的进程
    client_dp(procList, 1);
    client_dp(procList, 1);
    // 尝试创建MAX_PID个进程，只能创建15个进程，因为进程数目已满
    for (base_type i = 0; i < MAX_PID; ++i)
    {
        client_cp(sys, procList, 4 * _KB);
    }
    // 销毁合法进程
    client_dp(procList, 5);
    client_dp(procList, 10);
    client_dp(procList, 19);
    // 销毁不合法进程，会报出错误信息
    client_dp(procList, 20);
    client_dp(procList, 21);
    // 将一个进程存入磁盘
    name = "procTestB";
    client_sd(procList, 1, name);
    // 测试载入较多进程映像
    for (base_type i = 0; i < MAX_PID; ++i)
    {
        client_ld(sys, procList, name);
    }
    // 销毁所有进程
    for (base_type i = 0; i < MAX_PID; ++i)
    {
        if (procList[i] != NULL)
        {
            client_dp(procList, procList[i]->pcb->pid);
        }
    }
    // 查看进程列表，应该为空
    client_ls((const struct Process **)procList);
    // 关闭系统
    client_exit(&sys, procList);
    
    // 重启系统
    // 测试分页输出
    // 初始化进程列表指针
    sys = client_initSystem(ORDER_MODE);
    client_init(&procList);
    base_type n = 5;
    // 创建n个进程，每个初始大小为4K
    for(base_type i = 0; i < n; ++i)
    {
        client_cp(sys, procList, 4*_KB);
    }
    // 依次申请内存
    for(int i = 0; i < 6; ++i)
    {
        for(base_type j = 0; j < n; ++j)
        {
            client_pa(procList, procList[j]->pcb->pid, 4*_KB);
        }
    }
    //依次查看进程信息,n个进程，对每个进程来说，分页的间隔应该是n
    for(base_type j = 0; j < n; ++j)
    {
        client_sp((const struct Process **)procList, procList[j]->pcb->pid);
    }
    // 关闭系统
    client_exit(&sys, procList);
}

void testStart()
{
    start();
}