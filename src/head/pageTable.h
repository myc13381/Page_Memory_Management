/********************************************************************************
 * 实现页表数据结构
 * 整个系统可用内存空间为32M，最多需要32M/4K=8K个page
 * 每一个Page大小为4Kbyte，系统按字节编址，地址长度为32位
 * 一个页表最多有1024项，每一项指向一个4K大小的页，因此一个页表最多管理4K*1024 =4M内存
 * 假设每个进程最多只允许申请4M内存，所以只需要一级页表
 * |************************************************|
 * |        逻辑页号        |         物理页号        |
 * |        逻辑页号        |         物理页号        |
 * |        逻辑页号        |         物理页号        |
 * |        逻辑页号        |         物理页号        |
 * |        逻辑页号        |         物理页号        |
 * |        逻辑页号        |         物理页号        |
 * |        逻辑页号        |         物理页号        |
 * |        逻辑页号        |         物理页号        |
 * |        逻辑页号        |         物理页号        |
 * |        逻辑页号        |         物理页号        |
 **|************************************************|***************************/

#include "head.h"
#include "queueLite.h"

#ifndef PAGE_MEMORY_MANAGEMENT_PAGETABLE
#define PAGE_MEMORY_MANAGEMENT_PAGETABLE

#define PAGE_CAPACITY 1024

#define PAGE_NOT_USED 0xFFFF // 物理页号最大为8192，这里使用0xFFFF作为标志，代表次页内存以释放



struct PageTable
{
    base_type *table; /* 0--65535 指向页表实体的指针 */
    base_type size; /* 已使用的最大页表项数，页表项的个数最多1024 */
    struct QueueLite *queue; // 存放未使用的页号
};

// 建立一个页表
struct PageTable* creatPageTbale();

// 销毁一个页表
void destroyPageTable(struct PageTable **pt);

// 获取页表指针
base_type* getPageTablePtr(const struct PageTable *pt);

// 获取页表项数量
base_type getPageTableSize(const struct PageTable *pt);

// 写下一个页表项,返回写的页表号
base_type writePageTable(struct PageTable *pt, base_type value);

// 修改页表
void modifyPageTable(struct PageTable *pt,base_type index,base_type value);

// 获取页表的某一项值
base_type getValueInPageTable(struct PageTable *pt, base_type index);

// 回收一个页表项
void recyclePageElem(struct PageTable *pt, base_type index);

#endif // PAGE_MEMORY_MANAGEMENT_PAGETABLE

