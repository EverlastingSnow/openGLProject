cmake_minimum_required(VERSION 3.10)
project(MyOpenGLProject)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")

# 定义目录结构
set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/output)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)

# 包含目录（直接对应Makefile中的INCLUDES）
include_directories(
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/imgui
    ${INCLUDE_DIR}/imgui/backends
    ${INCLUDE_DIR}/assimp
)

# 查找所有源文件（对应Makefile中的SOURCES）
file(GLOB_RECURSE SOURCES 
    "${SRC_DIR}/*.cpp"
    "${INCLUDE_DIR}/imgui/*.cpp"
)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

# 创建可执行文件
add_executable(main ${SOURCES})

# 链接库（直接对应Makefile中的LIBRARIES和LIBS）
target_link_libraries(main
    ${LIB_DIR}/libassimp.dll.a  # 直接指定库文件路径
    ${LIB_DIR}/glad
    ${LIB_DIR}/glfw3dll
    ${LIB_DIR}/freeglut
    opengl32
    glu32
)

# 添加clean目标
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${OUTPUT_DIR}
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/*.obj
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/*.o
    COMMENT "Cleaning all generated files"
)

# 添加run目标
add_custom_target(run
    DEPENDS main
    COMMAND ${OUTPUT_DIR}/main.exe
)