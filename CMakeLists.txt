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

# 首先尝试查找除libmagic之外的所有依赖
pkg_check_modules(DEPS_WITHOUT_MAGIC REQUIRED
    poppler-cpp
    libzip
    pugixml
    freetype2
    libxml-2.0
    uuid
    tinyxml2
)

# 单独处理libmagic依赖，兼容新旧环境
set(LIBMAGIC_FOUND FALSE)
set(LIBMAGIC_LIBRARIES "")
set(LIBMAGIC_INCLUDE_DIRS "")

# 首先尝试使用pkg-config查找libmagic（新环境）
pkg_check_modules(LIBMAGIC_PKG libmagic)
if(LIBMAGIC_PKG_FOUND)
    set(LIBMAGIC_FOUND TRUE)
    set(LIBMAGIC_LIBRARIES ${LIBMAGIC_PKG_LIBRARIES})
    set(LIBMAGIC_INCLUDE_DIRS ${LIBMAGIC_PKG_INCLUDE_DIRS})
    message(STATUS "Found libmagic via pkg-config: ${LIBMAGIC_PKG_VERSION}")
else()
    # 回退到传统方法（旧环境）
    find_path(LIBMAGIC_INCLUDE_DIR magic.h)
    find_library(LIBMAGIC_LIBRARY NAMES magic)
    
    if(LIBMAGIC_INCLUDE_DIR AND LIBMAGIC_LIBRARY)
        set(LIBMAGIC_FOUND TRUE)
        set(LIBMAGIC_LIBRARIES ${LIBMAGIC_LIBRARY})
        set(LIBMAGIC_INCLUDE_DIRS ${LIBMAGIC_INCLUDE_DIR})
        message(STATUS "Found libmagic via traditional method: ${LIBMAGIC_LIBRARY}")
    endif()
endif()

if(NOT LIBMAGIC_FOUND)
    message(FATAL_ERROR "libmagic not found. Please install libmagic-dev package.")
endif()

# 合并所有依赖
set(DEPS_LIBRARIES ${DEPS_WITHOUT_MAGIC_LIBRARIES} ${LIBMAGIC_LIBRARIES})
set(DEPS_INCLUDE_DIRS ${DEPS_WITHOUT_MAGIC_INCLUDE_DIRS} ${LIBMAGIC_INCLUDE_DIRS})

# 添加子目录
add_subdirectory(src) 

# 添加测试选项，默认构建
option(BUILD_TESTS "Build test applications" ON)

# 有条件地添加测试目录
if(BUILD_TESTS)
    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        add_subdirectory(tests)
    endif()
endif()
