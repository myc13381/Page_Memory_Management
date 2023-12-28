include_directories(${PROJECT_SOURCE_DIR}/src/head)
add_library(mlib "head.c" "pageTable.c" "queueLite.c" "processControlBlock.c" "memory.c" "process.c" "system.c")