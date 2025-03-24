cmake_minimum_required(VERSION 3.16)
project(docparser VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 安全编译选项
add_compile_options(-fstack-protector-strong -D_FORTITY_SOURCE=1)
add_link_options(-z noexecstack -pie -fPIC)

# 设置安装路径
include(GNUInstallDirs)

# 查找依赖包
find_package(PkgConfig REQUIRED)
pkg_check_modules(DEPS REQUIRED
    poppler-cpp
    libzip
    pugixml
    freetype2
    libxml-2.0
    uuid
    tinyxml2
)

# 添加子目录
add_subdirectory(src) 

# 添加测试选项，默认不构建
option(BUILD_TESTS "Build test applications" OFF)

# 有条件地添加测试目录
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
