# xlsxio 和 libxls 源码路径（本地）
set(XLSXIO_SRC_DIR ${CMAKE_SOURCE_DIR}/3rdparty/libs/fileext/excel/xlsxio)
set(LIBXLS_SRC_DIR ${CMAKE_SOURCE_DIR}/3rdparty/libs/fileext/excel/libxls)

# 查找 xlsxio 依赖（expat + minizip + zlib）
pkg_check_modules(EXPAT REQUIRED expat)
pkg_check_modules(MINIZIP REQUIRED minizip)
pkg_check_modules(ZLIB REQUIRED zlib)

# 创建 xlsxio_read 静态库（本地源码）
add_library(xlsxio_read STATIC
    ${XLSXIO_SRC_DIR}/xlsxio_read.c
    ${XLSXIO_SRC_DIR}/xlsxio_read_sharedstrings.c
)
target_compile_definitions(xlsxio_read
    PRIVATE
        USE_MINIZIP
)
target_include_directories(xlsxio_read
    PUBLIC
        ${XLSXIO_SRC_DIR}
    PRIVATE
        ${EXPAT_INCLUDE_DIRS}
        ${MINIZIP_INCLUDE_DIRS}
        ${ZLIB_INCLUDE_DIRS}
)
target_link_libraries(xlsxio_read
    PRIVATE
        ${EXPAT_LIBRARIES}
        ${MINIZIP_LIBRARIES}
        ${ZLIB_LIBRARIES}
)
set_target_properties(xlsxio_read PROPERTIES POSITION_INDEPENDENT_CODE ON)

# 生成 libxls 需要的 config.h
set(LIBXLS_CONFIG_DIR ${CMAKE_CURRENT_BINARY_DIR}/libxls_config)
file(WRITE ${LIBXLS_CONFIG_DIR}/config.h
    "#define LIBXLS_MAJOR_VERSION 1\n"
    "#define LIBXLS_MINOR_VERSION 6\n"
    "#define LIBXLS_MICRO_VERSION 3\n"
    "#define PACKAGE_VERSION \"1.6.3\"\n"
    "#define HAVE_ICONV 1\n"
    "#define ICONV_CONST \n"
    "#define HAVE_STRDUP 1\n"
    "#define HAVE_WCHAR_H 1\n"
)

# 创建 libxls 静态库（本地源码）
add_library(xls STATIC
    ${LIBXLS_SRC_DIR}/xls.c
    ${LIBXLS_SRC_DIR}/ole.c
    ${LIBXLS_SRC_DIR}/xlstool.c
    ${LIBXLS_SRC_DIR}/endian.c
    ${LIBXLS_SRC_DIR}/locale.c
)
target_include_directories(xls
    PUBLIC
        ${LIBXLS_SRC_DIR}/include
    PRIVATE
        ${LIBXLS_CONFIG_DIR}
)
target_compile_definitions(xls
    PRIVATE
        _XOPEN_SOURCE=700
)
set_target_properties(xls PROPERTIES POSITION_INDEPENDENT_CODE ON)

# 收集源文件
file(GLOB_RECURSE SRC_FILES_MAIN
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(GLOB_RECURSE SRC_FILES_3RDPARTY_LIBS
    "${CMAKE_SOURCE_DIR}/3rdparty/libs/*.cpp"
    "${CMAKE_SOURCE_DIR}/3rdparty/libs/*.hpp"
    "${CMAKE_SOURCE_DIR}/3rdparty/libs/*.h"
)

file(GLOB_RECURSE SRC_FILES_3RDPARTY_UTILS
    "${CMAKE_SOURCE_DIR}/3rdparty/utils/*.cpp"
    "${CMAKE_SOURCE_DIR}/3rdparty/utils/*.cc"
    "${CMAKE_SOURCE_DIR}/3rdparty/utils/*.h"
)

# 创建源文件列表
set(SRC_FILES
    ${SRC_FILES_MAIN}
    ${SRC_FILES_3RDPARTY_LIBS}
    ${SRC_FILES_3RDPARTY_UTILS}
)

# 移除重复的源文件
list(REMOVE_DUPLICATES SRC_FILES)

# 打印源文件列表，用于调试
message(STATUS "Source files:")
foreach(SOURCE_FILE ${SRC_FILES})
    message(STATUS "  ${SOURCE_FILE}")
endforeach()

# 创建库目标
add_library(docparser SHARED
    ${SRC_FILES}
)

# 设置库的版本信息
set_target_properties(docparser PROPERTIES
    VERSION 1.0
    SOVERSION 1
)

# 设置目标属性
target_include_directories(docparser
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/docparser>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/3rdparty/libs
        ${CMAKE_SOURCE_DIR}/3rdparty/utils/libofd
        ${DEPS_INCLUDE_DIRS}
)

target_link_libraries(docparser
    PRIVATE
        ${DEPS_LIBRARIES}
        xlsxio_read
        xls
)

# 安装目标
install(TARGETS docparser
    EXPORT docparser-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 安装头文件
install(FILES
    docparser.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/docparser
)

# 生成并安装 pkg-config 文件
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/docparser.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/docparser.pc
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/docparser.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)
