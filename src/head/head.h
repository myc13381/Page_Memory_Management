/*
 * 包含libc的头文件,定义一些类型的名称
*/

#ifndef PAGE_MEMORY_MANAGEMENT_HEAD
#define PAGE_MEMORY_MANAGEMENT_HEAD

#include <stddef.h> /* size_t */
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>


typedef enum { TYPE_FALSE, TYPE_TRUE } bool_type; // bool 类型

typedef uint16_t base_type; // 定义数量类型，例如页表项的数量，栈的长度
typedef uint32_t address_type;  // 定义地址类型

#define SIZE_OF_PAGE 4 * 1024 // Page的大小
#define MAX_PID 20 // 进程的最大数量

// 定义单位方便测试 (Unit:byte)
#define _KB (1 * 1024)
#define _MB (1 * 1024 * 1024)


address_type MAX(address_type left, address_type right);

address_type MIN(address_type left, address_type right);



#endif // PAGE_MEMORY_MANAGEMENT_HEAD

