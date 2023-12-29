#include "client.h"
#include <stdio.h>

#define BUFF_SIZE 512
#define NAME_LEN 129 // 文件名最长128字节

char *cpu_status[4] = {"Ready", "Execute", "Block", "Undefined"};
char *PATH = "F:\\project\\PageMemoryManagement\\procImage\\";

// 输出进程的页表
void showProcPageTable(const struct Process *proc)
{
    assert(proc != NULL);
    struct PageTable *pt = proc->pcb->page_table;
    printf("capacity of a pagetable is 1024, the used number of elem is %u\n", pt->size);
    printf("logic Page numbers\t---->\treal Page numbers\n");
    // 程序大小为0，页表为空
    if (proc->size == 0)
        printf("pagetable is empty!\n");
    // 程序大小不为0
    base_type start = 1, end = 0;
    start = end = getValueInPageTable(pt, 0);
    for (base_type i = 1; i < pt->size; ++i)
    {
        base_type temp = getValueInPageTable(pt, i);
        if (temp == end + 1)
            ++end;
        else
        {
            if (start == end)
                printf("%u\t\t\t---->\t%u\n", i - 1, end);
            else
                printf("%u--%u\t\t\t---->\t%u--%u\n", i - (end - start) - 1, i - 1, start, end);
            start = end = temp;
        }
    }
    if (start == end)
        printf("%u\t\t\t---->\t%u\n", pt->size - 1, end);
    else
        printf("%u--%u\t\t\t---->\t%u--%u\n", pt->size - (end - start) - 1, pt->size - 1, start, end);
}

// 输出进程的PCB信息
void showProcPCB(const struct Process *proc)
{
    assert(proc != NULL);
    struct ProcessControlBlock *pcb = proc->pcb;
    // printf("show message of process's PCB\n");
    printf("CPU status is %s\n", cpu_status[proc->pcb->status]);
    showProcPageTable(proc);
}

// 输出进程信息
void showProc(const struct Process *proc)
{
    assert(proc != NULL);

    printf("process pid is %llu\n", proc->pcb->pid);
    printf("size of process is %llukb\n", proc->size / _KB);
    showProcPCB(proc);
}

/*********************************************************************
 * 客户端界面函数
 ********************************************************************/

// 展示菜单
void client_showMenu(char **message)
{
    if (message == NULL || *message == NULL)
    {
        printf("Please enter the command, enter help to get more message...\n");
    }
    else
    {
        **message = 0;
        (*message) = "Please enter the command, enter help to get more message...\n";
    }
}

// 查看系统信息
void client_showSystemMessage(const struct TinySystem *sys, char **message)
{
    assert(sys != NULL);
    if (message == NULL || *message == NULL)
    {
        printf("memory capacity:%llukb\nsystem space capacity:%llukb, system used space:%llukb\nuser capacity:%llukb, user used space:%llukb\n",
               MEMORY_CAPACITY / _KB, USER_ADDR_OFFSET / _KB, sys->mem->sys_used / _KB, (MEMORY_CAPACITY - USER_ADDR_OFFSET) / _KB, sys->mem->user_used / _KB);
    }
    else
    {
        **message = 0;
        sprintf(
            *message, "memory capacity:%llukb\nsystem space capacity:%llukb, system used space:%llukb\nuser capacity:%llukb, user used space:%llukb\n",
            MEMORY_CAPACITY / _KB, USER_ADDR_OFFSET / _KB, sys->mem->sys_used / _KB, (MEMORY_CAPACITY - USER_ADDR_OFFSET) / _KB, sys->mem->user_used / _KB);
    }
}

// 查看进程列表
void client_showProcessList(const struct Process **procList, char **message)
{
    assert(procList != NULL);
    base_type count = 0;
    if (message == NULL || *message == NULL)
    {
        for (int i = 0; i < MAX_PID; ++i)
        {
            if (procList[i] != NULL && procList[i]->pcb->pid != MAX_PID)
            {
                printf("process\tpid:%u\tsize:%llu\n", procList[i]->pcb->pid, procList[i]->size);
                ++count;
            }
        }
        if (count == 0)
            printf("no process exist!\n");
    }
    else
    {
        **message = 0;
        char *temp = (char *)malloc(BUFF_SIZE);
        for (int i = 0; i < MAX_PID; ++i)
        {
            if (procList[i] != NULL && procList[i]->pcb->pid != MAX_PID)
            {
                sprintf(temp, "process pid:%u\n", procList[i]->pcb->pid);
                strcat(*message, temp);
                *temp = 0;
            }
        }
        free(temp);
    }
}

// 查看某一进程具体信息
void client_showProcessMessage(const struct Process *proc, char **message)
{
    assert(proc != NULL);
    if (message == NULL || *message == NULL)
    {
        showProc(proc);
    }
    else
    {
        **message = 0;
        printf("Sorry! The feature has not yet been implemented!\n");
    }
}

/*********************************************************************
 * 客户端界面操作
 ********************************************************************/

// 开机初始化系统，返回系统指针
struct TinySystem *client_initSystem(bool_type order_mode)
{
    struct TinySystem *sys = creatSystem(order_mode);
    assert(sys != NULL);
    return sys;
}

// 关闭系统
void client_shutdownSystem(struct TinySystem **sys)
{
    assert(sys != NULL && *sys != NULL);
    destroySystem(sys);
    *sys = NULL;
}

// 创建一个进程 传入进程列表，返回进程编号(pid), pid==MAX_PID代表创建失败
// 在procList中，如果一个元素值为NULL，代表该位置可以创建新的进程
base_type client_creatProcess(struct TinySystem *sys, struct Process **procList, address_type size)
{
    assert(sys != NULL && procList != NULL && size <= MAX_SIZE_OF_PROGRAM);
    // 通过线性查找找到一个可用元素，如果找不到返回MAX_PID
    for (base_type i = 0; i < MAX_PID; ++i)
    {
        if (procList[i] == NULL)
        {
            procList[i] = creatProcess(sys);
            loadProgram(procList[i], size);
            return procList[i]->pcb->pid;
        }
    }
    return MAX_PID;
}

// 销毁一个进程
void client_destroyProcess(struct Process **procList, base_type pid)
{
    assert(procList != NULL && pid < MAX_PID);
    if (procList[pid] == NULL)
        return; // 释放一个不存在的进程
    else
    {
        destroyProcess(&(procList[pid]));
        procList[pid] = NULL;
    }
    return;
}

// 为进程分配新的内存
void client_procAllocate(struct Process **procList, base_type pid, address_type size)
{
    assert(procList != NULL && pid < MAX_PID && size < MAX_SIZE_OF_PROGRAM);
    assert(procList[pid] != NULL); // 进程不存在
    procAllocate(procList[pid], size);
}

// 将某一进程存储进磁盘
bool_type client_storeProc2Disk(struct Process **procList, base_type pid, const char *fileName)
{
    assert(procList != NULL && pid < MAX_PID && fileName != NULL);
    assert(procList[pid] != NULL);
    return writeProc2Disk(procList[pid], fileName);
}

// 将某一进程载入进磁盘
base_type client_loadProcFromDisk(struct TinySystem *sys, struct Process **procList, const char *fileName)
{
    assert(procList != NULL && fileName != NULL);
    // 通过线性查找找到一个可用元素，如果找不到返回MAX_PID
    for (base_type i = 0; i < MAX_PID; ++i)
    {
        if (procList[i] == NULL)
        {
            procList[i] = readProcFromDisk(sys, fileName);
            if(procList[i] == NULL) return MAX_PID; // 创建失败
            else return i;
        }
    }
    return MAX_PID;
}

// 命令行解析操作，注意管理procList 和 procNameList

// 初始化客户端
void client_init(struct Process ***procList)
{
    assert(procList != NULL);
    printf("Init system ...\n");
    *procList = (struct Process **)malloc(sizeof(struct Process *) * MAX_PID);
    for (int i = 0; i < MAX_PID; ++i)
    {
        (*procList)[i] = NULL;
    }
}

// help
void client_help()
{
    SET_MESSAGE_COLOR;
    printf("* command   (args)      (...)\n");
    printf("* help                                  get help\n");
    printf("* exit                                  shutdown system\n");
    printf("* cp        (size:KB)                   creat process\n");
    printf("* dp        (pid)                       destroy process\n");
    printf("* pa        (pid)       (size:KB)       process allocate memory\n");
    printf("* sd        (pid)                       store process to disk\n");
    printf("* ld        (fileName)                  load program from disk\n");
    printf("* sp        (pid)                       show process message\n");
    printf("* ls                                    list all process\n");
    printf("* ss                                    show system message\n");
    SET_DEFAULT_COLOR;
}

// exit
void client_exit(struct TinySystem **sys, struct Process **procList)
{
    assert(sys != NULL && *sys != NULL && procList != NULL);
    // 销毁所有真正运行的进程
    SET_MESSAGE_COLOR;
    for (base_type i = 0; i < MAX_PID; ++i)
    {
        if (procList[i] != NULL)
        {
            client_destroyProcess(procList, i);
        }
    }
    printf("destroy all process!\n");
    free(procList);
    // 关闭系统
    client_shutdownSystem(sys);
    printf("Shutdown system ...\n");
    SET_DEFAULT_COLOR;
}

// cp creat process
void client_cp(struct TinySystem *sys, struct Process **procList, address_type size)
{
    assert(sys != NULL && &procList != NULL);
    SET_ERROR_COLOR;
    if (size == 0 || size > MAX_SIZE_OF_PROGRAM)
    {
        printf("creat new process failed! number of size is unvalid!\n");
        return;
    }
    address_type free = (MEMORY_CAPACITY - USER_ADDR_OFFSET - (sys->mem->user_used));

    if ((MEMORY_CAPACITY - USER_ADDR_OFFSET - (sys->mem->user_used)) <= size)
    {
        printf("creat new process failed! memory space is not enough!\n");
        return;
    }
    base_type pid = client_creatProcess(sys, procList, size);
    if (pid == MAX_PID)
        printf("creat new process failed! number of processes is full!\n");
    else
    {
        SET_MESSAGE_COLOR;
        printf("creat new process sucessfully! pid is %llu\n", pid);
    }
    SET_DEFAULT_COLOR;
    return;
}

// dp destroy process
void client_dp(struct Process **procList, base_type pid)
{
    assert(procList != NULL);
    SET_ERROR_COLOR;
    if (pid >= MAX_PID)
    {
        printf("destroy process failed! pid is unvalid!\n");
        return;
    }
    base_type i = 0;
    for (i = 0; i < MAX_PID; ++i)
    {
        if (procList[i] != NULL && procList[i]->pcb->pid == pid)
        {
            break;
        }
    }
    if (i == MAX_PID)
    {
        printf("destroy process failed! process is not exist!\n");
    }
    else 
    {
        SET_MESSAGE_COLOR;
        client_destroyProcess(procList, i);
        procList[pid] = NULL;
        printf("destroy process success! pid is %u\n", pid);
    }
    SET_DEFAULT_COLOR;
    return;
}

// pa process allocate memory
void client_pa(struct Process **procList, base_type pid, address_type size)
{
    assert(procList != NULL);
    SET_ERROR_COLOR;
    if (pid >= MAX_PID)
    {
        printf("process allocate failed! pid is unvalid\n");
        return;
    }
    if (size == 0 || size > MAX_SIZE_OF_PROGRAM)
    {
        printf("process allocate failed! number of size is unvalid!\n");
        return;
    }
    if ((MEMORY_CAPACITY - USER_ADDR_OFFSET - (procList[pid]->sys->mem->user_used)) <= size)
    {
        printf("process allocate failed! memory space is not enough!\n");
        return;
    }
    if (procList[pid] == NULL)
    {
        printf("process allocate failed! process not exist!\n");
    }
    else if ((MAX_SIZE_OF_PROGRAM - (procList[pid]->size)) <= size)
    {
        printf("process allocate failed! memory space is not enough!\n");
    }
    else if ((MEMORY_CAPACITY - USER_ADDR_OFFSET - procList[pid]->sys->mem->user_used) < size)
    {
        printf("process allocate failed! memory space is not enough!\n");
    }
    else
    {
        SET_MESSAGE_COLOR;
        client_procAllocate(procList, pid, size);
        printf("process allocate memory success! pid is %llu\n", pid);
    }
    SET_DEFAULT_COLOR;
    return;
}

// sd store process to disk
void client_sd(struct Process **procList, base_type pid, const char *fileName)
{
    assert(procList != NULL && fileName != NULL);
    SET_ERROR_COLOR;
    if (pid >= MAX_PID)
    {
        printf("store process to disk failed! pid is unvalid!\n");
        return;
    }
    if (procList[pid] == NULL)
    {
        printf("store process to disk failed! process is not exist!\n");
        return;
    }

    // ！！！为了测试，我们将进程的空间去不赋值为循环的a->z->a,文件写入后可以通过编辑器查看
    char *mem = procGetMemoryHandle(procList[pid], 0);
    for (address_type i = 0; i < procList[pid]->size; ++i)
    {
        *(mem + i) = (char)('a' + (i % 26));
    }

    char *path = malloc(sizeof(char) * BUFF_SIZE);
    memcpy(path, PATH, BUFF_SIZE);
    sprintf(path, "%s%s", path, fileName);
    bool_type ret = client_storeProc2Disk(procList, pid, path);
    free(path);
    if (ret == TYPE_FALSE)
        printf("store process to disk failed!\n");
    else
    {
        SET_MESSAGE_COLOR;
        printf("store process to disk success! pid is %u\n", pid);
    }
    SET_DEFAULT_COLOR;
    return;
}

// ld load process from disk
void client_ld(struct TinySystem *sys, struct Process **procList, const char *fileName)
{
    assert(sys != NULL && procList != NULL && fileName != NULL);
    base_type pid = MAX_PID;
    // 获取文件完整路径名
    char *file = malloc(sizeof(char) * BUFF_SIZE);
    memcpy(file, PATH, BUFF_SIZE);
    sprintf(file, "%s%s", file, fileName);

    pid = client_loadProcFromDisk(sys, procList, file);
    free(file);
    SET_ERROR_COLOR;
    if (pid == MAX_PID)
    {
        printf("load process from disk failed!\n");
    }
    else
    {
        SET_MESSAGE_COLOR;
        printf("load process from disk success! pid is %llu\n", pid);

        // 通过输出验证读入内容是否正确
        int len = 26 * 2; //a->z输出两变
        const char *mem = procGetMemoryHandle(procList[pid], 0);
        char buff[BUFF_SIZE];
        memmove(buff,mem,26*2);
        buff[len] = 0;
        printf("%s\n",buff);
        for(base_type i = 0; i < len; ++i)
        {
            if(buff[i] != 'a' + i%26)
            {
                SET_ERROR_COLOR;
                printf("load program error!\n");
                break;
            }
        }
    }
    SET_DEFAULT_COLOR;
    return;
}

// sp show process message
void client_sp(const struct Process **procList, base_type pid)
{
    assert(procList != NULL);
    SET_ERROR_COLOR;
    if (pid >= MAX_PID)
        printf("pid is unvalid!\n");
    else if (procList[pid] == NULL)
        printf("process is not exist! input pid is %u\n", pid);
    else
    {
        SET_MESSAGE_COLOR;
        client_showProcessMessage(procList[pid], NULL);
    }
    SET_DEFAULT_COLOR;
    return;
}

// ls list all process
void client_ls(const struct Process **procList)
{
    assert(procList != NULL);
    SET_MESSAGE_COLOR;
    client_showProcessList(procList, NULL);
    SET_DEFAULT_COLOR;
    return;
}

// ss show system message
void client_ss(const struct TinySystem *sys)
{
    assert(sys != NULL);
    SET_MESSAGE_COLOR;
    client_showSystemMessage(sys, NULL);
    SET_DEFAULT_COLOR;
    return;
}

// 解析命令
void start()
{
    SET_DEFAULT_COLOR;
    // 启动系统，初始化
    struct TinySystem *sys = client_initSystem(TYPE_TRUE);
    struct Process **procList = NULL;
    client_init(&procList);
    // 解析命令
    char *command = malloc(sizeof(char) * NAME_LEN);
    char *fileName = malloc(sizeof(char) * NAME_LEN);
    char *procName = malloc(sizeof(char) * NAME_LEN);
    char *buff = malloc(sizeof(char) * BUFF_SIZE);
    int pid = MAX_PID;
    long long size = 0;
    system("cls");
    for (;;)
    {
        SET_DEFAULT_COLOR;
        printf("===========================================================\n");
        client_showMenu(NULL);
        // 获取命令
        SET_COMMAND_COLOR;
        scanf("%s", command);
        if (strcmp(command, "help") == 0)
        {
            fgets(buff, BUFF_SIZE, stdin);
            SET_MESSAGE_COLOR;
            client_help();
        }
        else if (strcmp(command, "exit") == 0)
        {
            break;
        }
        else if (strcmp(command, "cp") == 0)
        {
            scanf("%llu", &size);
            fgets(buff, BUFF_SIZE, stdin);
            SET_MESSAGE_COLOR;
            client_cp(sys, procList, (address_type)(size * _KB));
        }
        else if (strcmp(command, "dp") == 0)
        {
            scanf("%u", &pid);
            fgets(buff, BUFF_SIZE, stdin);
            SET_MESSAGE_COLOR;
            client_dp(procList, pid);
        }
        else if (strcmp(command, "pa") == 0)
        {
            scanf("%u%llu", &pid, &size);
            fgets(buff, BUFF_SIZE, stdin);
            SET_MESSAGE_COLOR;
            client_pa(procList, pid, (address_type)(size * _KB));
        }
        else if (strcmp(command, "sd") == 0)
        {
            scanf("%llu%s", &pid, fileName);
            fgets(buff, BUFF_SIZE, stdin);
            SET_MESSAGE_COLOR;
            client_sd(procList, pid, fileName);
        }
        else if (strcmp(command, "ld") == 0)
        {
            scanf("%s", fileName);
            fgets(buff, BUFF_SIZE, stdin);
            SET_MESSAGE_COLOR;
            client_ld(sys, procList, fileName);
        }
        else if (strcmp(command, "sp") == 0)
        {
            scanf("%d", &pid);
            fgets(buff, BUFF_SIZE, stdin);
            SET_MESSAGE_COLOR;
            client_sp((const struct Process **)procList, pid);
        }
        else if (strcmp(command, "ls") == 0)
        {
            fgets(buff, BUFF_SIZE, stdin);
            SET_MESSAGE_COLOR;
            client_ls((const struct Process **)procList);
        }
        else if (strcmp(command, "ss") == 0)
        {
            fgets(buff, BUFF_SIZE, stdin);
            SET_MESSAGE_COLOR;
            client_ss(sys);
        }
        else
        {
            SET_MESSAGE_COLOR;
            printf("command is unvalid\n");
            fgets(buff, BUFF_SIZE, stdin);
        }
        SET_DEFAULT_COLOR;
        printf("\n");
    }
    // 关闭客户端，系统，释放资源
    client_exit(&sys, procList);
    free(command);
    free(fileName);
    free(procName);
    free(buff);
    SET_DEFAULT_COLOR;
    // exit(0);
}

// 设置控制台颜色
#ifdef _WIN32

void SetColor(UINT uFore, UINT uBack)
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(handle, uFore + uBack * 0x10);
}

#else // linux

#endif
