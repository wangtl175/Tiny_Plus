# CMakeList.txt: Tiny_Plus 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required(VERSION 3.17)

project("Tiny_Plus")

set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/include/syntax)
include_directories(${PROJECT_SOURCE_DIR}/include/lex)
include_directories(${PROJECT_SOURCE_DIR}/include)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/lex SOURCE_LEX)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/syntax SOURCE_SYNTAX)
aux_source_directory(${PROJECT_SOURCE_DIR}/src SOURCE)

# 将源代码添加到此项目的可执行文件。
add_executable(Tiny_Plus ${SOURCE_LEX} ${SOURCE_SYNTAX} ${SOURCE})
# add_executable (Tiny_Plus   ${SOURCE})

