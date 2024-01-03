include_directories(${PROJECT_SOURCE_DIR}/kernel/head)
# 设置库文件输出路径
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
add_library(mlibd SHARED "head.c" "pageTable.c" "queueLite.c" "processControlBlock.c" "memory.c" "process.c" "system.c")
add_library(mliba "head.c" "pageTable.c" "queueLite.c" "processControlBlock.c" "memory.c" "process.c" "system.c")
set_target_properties(mlibd PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin) # 设置动态链接库输出路径