cmake_minimum_required(VERSION 3.13)
project(docparser VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 在顶层CMakeLists.txt中添加
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

# 安全编译选项
add_compile_options(-fstack-protector-strong -D_FORTITY_SOURCE=1 -fvisibility=hidden)
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
