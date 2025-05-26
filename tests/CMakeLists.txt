set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Core Test)
find_package(Dtk6 REQUIRED COMPONENTS Core)

# Enable testing
enable_testing()

# 设置测试程序的源文件
set(TEST_SOURCES
    main.cpp
)

# 设置自动化测试的源文件
set(AUTOTEST_SOURCES
    autotest.cpp
)

# 创建测试可执行文件
add_executable(docparser_test ${TEST_SOURCES})

# 创建自动化测试可执行文件
add_executable(docparser_autotest ${AUTOTEST_SOURCES})

# 链接docparser库和Qt6Core
target_link_libraries(docparser_test
    PRIVATE
        docparser
        Qt6::Core
        Dtk6::Core
)

# 链接自动化测试所需的库
target_link_libraries(docparser_autotest
    PRIVATE
        docparser
        Qt6::Core
        Qt6::Test
)

# 设置包含目录
target_include_directories(docparser_test
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_include_directories(docparser_autotest
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

# 添加测试到CTest
add_test(
    NAME DocParserAutoTest
    COMMAND docparser_autotest
)

# 设置测试属性
set_tests_properties(DocParserAutoTest PROPERTIES
    TIMEOUT 300  # 5 minutes timeout for all tests
)

# 安装测试程序(可选)
install(TARGETS docparser_test docparser_autotest
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
