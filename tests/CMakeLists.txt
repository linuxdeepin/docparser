find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Dtk6 REQUIRED COMPONENTS Core)

# 设置测试程序的源文件
set(TEST_SOURCES
    main.cpp
)

# 创建测试可执行文件
add_executable(docparser_test ${TEST_SOURCES})

# 链接docparser库和Qt6Core
target_link_libraries(docparser_test
    PRIVATE
        docparser
        Qt6::Core
        Dtk6::Core
)

# 设置包含目录
target_include_directories(docparser_test
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

# 安装测试程序(可选)
install(TARGETS docparser_test
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
