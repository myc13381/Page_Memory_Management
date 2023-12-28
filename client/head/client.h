/*
 * 包含输出进程相关信息的函数,以及命令输入实现，颜色控制
*/

#ifndef PAGE_MEMORY_MANAGEMENT_SHOW
#define PAGE_MEMORY_MANAGEMENT_SHOW

// #define UNIT_TEST

#include "head.h"
#include "process.h"

#ifdef _WIN32

#include <windows.h>
#define SET_COMMAND_COLOR do{SetColor(0xE ,0);}while(0)
#define SET_MESSAGE_COLOR do{SetColor(3, 0);}while(0)
#define SET_DEFAULT_COLOR do{SetColor(7 ,0);}while(0)
#define SET_ERROR_COLOR   do{SetColor(4, 0);}while(0)

#else //Linux

// linux 平台
#define SET_COMMAND_COLOR do{}while(0)
#define SET_MESSAGE_COLOR do{}while(0)
#define SET_DEFAULT_COLOR do{}while(0)
#define SET_ERROR_COLOR   do{}while(0)

#endif


/*********************************************************************
 * 进程基本信息输出
********************************************************************/


// 输出进程的页表
void showProcPageTable(const struct Process *proc);

// 输出进程的PCB信息
void showProcPCB(const struct Process *proc);

// 输出进程信息
void showProc(const struct Process *proc);



/*********************************************************************
 * 客户端界面函数
********************************************************************/

// 展示菜单
void client_showMenu(char **message);

// 查看系统信息
void client_showSystemMessage(const struct TinySystem *sys, char **message);

// 查看进程列表 进程列表是一个数组，数组的每个元素是进程指针
void client_showProcessList(const struct Process **procList, char **message);

// 查看某一进程具体信息
void client_showProcessMessage(const struct Process *proc, char **message);



/*********************************************************************
 * 客户端界面操作
********************************************************************/

// 开机初始化系统，返回系统指针
struct TinySystem *client_initSystem(bool_type order_mode);

// 关闭系统
void client_shutdownSystem(struct TinySystem **sys);

// 创建一个进程 传入进程列表，返回进程编号(pid), pid==MAX_PID代表创建失败
base_type client_creatProcess(struct TinySystem *sys, struct Process **procList, address_type size);

// 销毁一个进程
void client_destroyProcess(struct Process **procList, base_type pid);

// 为进程分配新的内存
void client_procAllocate(struct Process **procList, base_type pid, address_type size);

// 将某一进程存储进磁盘
bool_type client_storeProc2Disk(struct Process **procList, base_type pid, const char *fileName);

// 从磁盘中载入进程
base_type client_loadProcFromDisk(struct TinySystem *sys, struct Process **procList, const char *fileName);




/*****************************************************************************
 * 命令行输入解析
 * 命令列表：command (args) (...)
 * help                     get help                        --获取帮助信息
 * exit                     shutdown system                 --关闭系统退出程序
 * cp (name) (size:KB)      creat process                   --创建进程
 * dp (pid)                 destroy process                 --销毁进程
 * pa (pid) (size:KB)       process allocate memory         --进程开辟内存
 * sd (pid)                 store process to disk           --将进程存入磁盘
 * ld (fileName)            load program from disk          --从磁盘载入程序
 * sp (pid)                 show process message            --显示进程具体信息
 * ls                       list all process                --显示所有进程
 * ss                       show system message             --显示当前系统信息
*******************************************************************************/


// 初始化客户端
void client_init(struct Process *** procList);

// help
void client_help();

// exit
void client_exit(struct TinySystem **sys, struct Process **procList);

// cp creat process
void client_cp(struct TinySystem *sys, struct Process **procList, address_type size);

// dp destroy process
void client_dp(struct Process **procList, base_type pid);

// pa process allocate
void client_pa(struct Process **procList, base_type pid, address_type size);

// sd store process to disk
void client_sd(struct Process **procList, base_type pid, const char *fileName);

// ld load program from disk
void client_ld(struct TinySystem *sys, struct Process **procList, const char *fileName);

// sp show process message
void client_sp(const struct Process **procList, base_type pid);

// ls list all process
void client_ls(const struct Process **procList);

// ss show system message
void client_ss(const struct TinySystem *sys);

// 解析命令
void start();


void SetColor(UINT uFore,UINT uBack);


// 终端颜色控制
#ifdef _WIN32

/* * * * * * * * * * * * * * *
 *   0 = 黑色       8 = 灰色
 *   1 = 蓝色       9 = 淡蓝色
 *   2 = 绿色       A = 淡绿色
 *   3 = 浅绿色     B = 淡浅绿色
 *   4 = 红色       C = 淡红色
 *   5 = 紫色       D = 淡紫色
 *   6 = 黄色       E = 淡黄色
 *   7 = 白色       F = 亮白色
 * * * * * * * * * * * * * * */

void SetColor(UINT uFore,UINT uBack);

#else //Linux

#endif

#endif // PAGE_MEMORY_MANAGEMENT_SHOW

