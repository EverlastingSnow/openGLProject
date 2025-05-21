cmake_minimum_required(VERSION 3.10)
project(Project)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加编译选项
add_compile_options(-Wall -Wextra -g)

# 定义源码和头文件目录
set(SRC_DIR "src")
set(INCLUDE_DIR "include")
set(LIB_DIR "lib")


# 添加头文件目录
include_directories(${INCLUDE_DIR})

# 查找源文件
file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.h")

# 添加库目录
link_directories(${LIB_DIR})

# 需要链接的库
set(LIBS glad glfw3dll freeglut opengl32 glu32)

# 添加可执行文件
add_executable(main ${SOURCES})



# 链接库
target_link_libraries(main ${LIBS})
