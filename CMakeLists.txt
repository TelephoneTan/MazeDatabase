# CMakeList.txt: MazeDatabase 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# 指定C标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED true)

# 指定C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# MSVC ====================================================================================================

# 为 MSVC 添加C++编译选项
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /source-charset:utf-8 /execution-charset:utf-8")

# 为 MSVC 添加C编译选项
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /source-charset:utf-8 /execution-charset:utf-8")

# =========================================================================================================

project ("MazeDatabase")

# 将源代码添加到此项目的可执行文件。
add_executable (MazeDatabase "MazeDatabase.cpp" "User.cpp" "User.h" "Record.h" "Record.cpp" Database.cpp Database.h Log.cpp Log.h)

# gcc on Linux =============================================================================================

## 链接额外的静态库和动态库
## 依赖关系（如果有）要满足：前面的库依赖后面的库
## 末尾要恢复动态链接
#target_link_libraries(MazeDatabase PRIVATE -Wl,-Bdynamic pthread stdc++ sqlite3 -Wl,-Bstatic m -Wl,-Bdynamic)
#
## 强制所有库链接使用静态库
## target_link_libraries(MazeDatabase PRIVATE -static)

# MSVC =====================================================================================================

# 在 Windows 中，静态库链接需要提供静态库.lib文件，动态库链接需要提供导入库.lib文件，使用动态链接库的程序在运行时必须能找到相应的.dll文件

# 链接时寻找SQLite库（.lib文件）的路径，视具体环境而定
# 设置此路径之后，只能保证链接成功，不能保证程序正常运行
# 一般在 Windows 中，程序运行时仍需要将SQLite动态库的DLL文件存放在系统指定目录或程序的同一目录下，除非使用静态链接库
target_link_directories(MazeDatabase PRIVATE "D:\\sqlite\\")

# 链接SQLite库（不显式指定动态/静态，由对应的.lib文件的类型决定是静态链接还是动态链接）
target_link_libraries(MazeDatabase sqlite3)

# ==========================================================================================================

# include路径，视平台与环境而定，一般在 Linux 中可不加此句
target_include_directories(MazeDatabase PRIVATE "D:\\sqlite\\")

# TODO: 如有需要，请添加测试并安装目标。
