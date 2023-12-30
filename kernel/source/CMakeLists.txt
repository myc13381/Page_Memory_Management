include_directories(${PROJECT_SOURCE_DIR}/src/head)
# 设置库文件输出路径
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
add_library(mlib "head.c" "pageTable.c" "queueLite.c" "processControlBlock.c" "memory.c" "process.c" "system.c")

