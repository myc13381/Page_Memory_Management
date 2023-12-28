#include "pageTable.h"

// 建立一个页表
struct PageTable* creatPageTbale()
{
    // 创建对象，分配内存
    struct PageTable *pt = (struct PageTable *)malloc(sizeof(struct PageTable));
    assert(pt != NULL);
    pt->table = (base_type *)malloc(PAGE_CAPACITY * sizeof(base_type));
    assert(pt->table != NULL);
    pt->size = 0;
    for(base_type i = 0; i < PAGE_CAPACITY; ++i)
        (pt->table)[i] = PAGE_NOT_USED;
    pt->queue = creatQueue(PAGE_CAPACITY);
    base_type arr[PAGE_CAPACITY];
    for(base_type i = 0; i < PAGE_CAPACITY; ++i)
    {
        arr[i] = i;
    }
    initQueueByArray(pt->queue, arr, PAGE_CAPACITY);
    return pt;
}

// 销毁一个页表
void destroyPageTable(struct PageTable **pt)
{
    assert(pt != NULL && *pt != NULL);
    if((*pt)->table != NULL)
    {
        free((*pt)->table);
        (*pt)->table=NULL;
    }
    if((*pt)->queue != NULL)
    {
        destroyQueue(&((*pt)->queue));
        (*pt)->queue = NULL;
    }
    (*pt)->size = 0;
    free(*pt);
    (*pt)=NULL;
}

// 获取页表指针
base_type* getPageTablePtr(const struct PageTable *pt)
{
    assert(pt != NULL);
    return pt->table;
}


// 获取页表项数量
base_type getPageTableSize(const struct PageTable *pt)
{
    assert(pt != NULL);
    return pt->size;
}


// 写下一个页表项,返回写的页表号
base_type writePageTable(struct PageTable *pt, base_type value)
{
    assert(pt != NULL);
    assert(pt->size <= PAGE_CAPACITY);
    base_type npage = queue_front(pt->queue);
    pt->table[npage] = value;
    queue_pop(pt->queue);
    ++(pt->size);
    return npage;
}


// 修改页表 用于将将页表置为未使用状态
void modifyPageTable(struct PageTable *pt,base_type index,base_type value)
{
    assert(pt != NULL);
    assert(index>=0 && pt->table[index] != PAGE_NOT_USED);
    (pt->table)[index] = value;
    return;
}

// 获取页表的某一项值
base_type getValueInPageTable(struct PageTable *pt, base_type index)
{
    assert(pt != NULL);
    assert(pt->table != NULL);
    return (pt->table)[index];
}

// 回收一个页表项
void recyclePageElem(struct PageTable *pt, base_type index)
{
    assert(pt != NULL);
    assert(index <= PAGE_CAPACITY);
    if(getValueInPageTable(pt,index) != PAGE_NOT_USED) //使用过页表项才会被回收
    {
        queue_push(pt->queue, index);
        modifyPageTable(pt, index, PAGE_NOT_USED);
    }
    --(pt->size);
}