/*
 * 运行程序
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

#ifndef UNIT_TEST

int main()
{
    start();
    return 0;
}

#endif