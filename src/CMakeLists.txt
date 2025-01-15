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
