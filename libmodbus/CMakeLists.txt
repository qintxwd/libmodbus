cmake_minimum_required(VERSION 3.12)

project("modbus" LANGUAGES C CXX)

set(LIBMODBUS_VERSION ${CMAKE_PROJECT_VERSION})
set(LIBMODBUS_VERSION_MAJOR ${CMAKE_PROJECT_VERSION_MAJOR})
set(LIBMODBUS_VERSION_MINOR ${CMAKE_PROJECT_VERSION_MINOR})
set(LIBMODBUS_VERSION_MICRO ${CMAKE_PROJECT_VERSION_PATCH})

message(STATUS "libmodbus version: ${LIBMODBUS_VERSION}")

# 设置标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 构建选项
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_CPP_WRAPPER "Build C++ wrapper (requires C++11)" ON)

# 平台特定设置
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_Release} -O3")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_Release} -O3")
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    if(MSVC)
        add_compile_options(/utf-8)
    endif()
endif()

# 检查头文件和函数
include(CheckIncludeFile)
include(CheckFunctionExists)
include(CheckSymbolExists)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    check_include_file(winsock2.h HAVE_WINSOCK2_H)
    check_include_file(errno.h HAVE_ERRNO_H)
    check_include_file(fcntl.h HAVE_FCNTL_H)
    check_include_file(limits.h HAVE_LIMITS_H)
    check_include_file(time.h HAVE_TIME_H)
    set(HAVE_SELECT 1)
    set(HAVE_SOCKET 1)
    set(HAVE_STRERROR 1)
else()
    check_include_file(arpa/inet.h HAVE_ARPA_INET_H)
    check_include_file(byteswap.h HAVE_BYTESWAP_H)
    check_include_file(errno.h HAVE_ERRNO_H)
    check_include_file(fcntl.h HAVE_FCNTL_H)
    check_include_file(limits.h HAVE_LIMITS_H)
    check_include_file(linux/serial.h HAVE_LINUX_SERIAL_H)
    check_include_file(netdb.h HAVE_NETDB_H)
    check_include_file(netinet/in.h HAVE_NETINET_IN_H)
    check_include_file(netinet/ip.h HAVE_NETINET_IP_H)
    check_include_file(netinet/tcp.h HAVE_NETINET_TCP_H)
    check_include_file(sys/ioctl.h HAVE_SYS_IOCTL_H)
    check_include_file(sys/socket.h HAVE_SYS_SOCKET_H)
    check_include_file(sys/time.h HAVE_SYS_TIME_H)
    check_include_file(sys/types.h HAVE_SYS_TYPES_H)
    check_include_file(termios.h HAVE_TERMIOS_H)
    check_include_file(time.h HAVE_TIME_H)
    check_include_file(unistd.h HAVE_UNISTD_H)
    
    check_function_exists(accept4 HAVE_ACCEPT4)
    check_function_exists(gai_strerror HAVE_GAI_STRERROR)
    check_function_exists(getaddrinfo HAVE_GETADDRINFO)
    check_function_exists(gettimeofday HAVE_GETTIMEOFDAY)
    check_function_exists(inet_ntop HAVE_INET_NTOP)
    check_function_exists(inet_pton HAVE_INET_PTON)
    check_function_exists(select HAVE_SELECT)
    check_function_exists(socket HAVE_SOCKET)
    check_function_exists(strerror HAVE_STRERROR)
    check_function_exists(strlcpy HAVE_STRLCPY)
    
    check_symbol_exists(TIOCM_RTS "sys/ioctl.h" HAVE_DECL_TIOCM_RTS)
    check_symbol_exists(TIOCSRS485 "sys/ioctl.h" HAVE_DECL_TIOCSRS485)
endif()

# 生成配置头文件
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
    @ONLY
)

# 生成版本头文件到 include 目录
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/modbus/modbus-version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/modbus/modbus-version.h"
    @ONLY
)

# C 库源文件
set(MODBUS_C_SOURCES
    src/modbus.c
    src/modbus-data.c
    src/modbus-rtu.c
    src/modbus-tcp.c
)

# C++ 实现文件
set(MODBUS_CPP_SOURCES
    src/modbus-cpp.cpp
)

# 公共 C++ 头文件（仅暴露这两个头文件）
set(MODBUS_PUBLIC_HEADERS
    include/modbus/modbus.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/include/modbus/modbus-version.h
)

# 私有 C 头文件（不安装，仅内部使用）
set(MODBUS_PRIVATE_HEADERS
    src/modbus-core.h
    src/modbus-tcp.h
    src/modbus-rtu.h
    src/modbus-private.h
    src/modbus-rtu-private.h
    src/modbus-tcp-private.h
)

# 平台特定库
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(PLATFORM_LIBS ws2_32)
else()
    set(PLATFORM_LIBS)
endif()

# 创建 modbus 库（C + C++）
add_library(${PROJECT_NAME} 
    ${MODBUS_C_SOURCES}
    ${MODBUS_CPP_SOURCES}
    ${MODBUS_PUBLIC_HEADERS}
    ${MODBUS_PRIVATE_HEADERS}
)

# 设置包含目录
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}
)

# 链接库
target_link_libraries(${PROJECT_NAME} PUBLIC ${PLATFORM_LIBS})

# 库属性
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set_property(TARGET ${PROJECT_NAME} PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${LIBMODBUS_VERSION}
    SOVERSION ${LIBMODBUS_VERSION_MAJOR}
    OUTPUT_NAME "modbus"
    POSITION_INDEPENDENT_CODE ON
)

# 编译器警告
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE 
        -Wall -Wmissing-declarations
        $<$<COMPILE_LANGUAGE:C>:-Wmissing-prototypes>
    )
endif()

# 构建示例
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# 安装规则
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# 安装库
install(TARGETS ${PROJECT_NAME}
    EXPORT modbusTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# 安装头文件（仅安装公开的 C++ 头文件）
install(FILES ${MODBUS_PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/modbus
)

# 生成并安装 CMake 配置文件
set(MODBUS_CMAKE_CONFIG_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/modbus")

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/modbusConfigVersion.cmake"
    VERSION ${LIBMODBUS_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modbusConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/modbusConfig.cmake"
    INSTALL_DESTINATION ${MODBUS_CMAKE_CONFIG_DIR}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/modbusConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/modbusConfigVersion.cmake"
    DESTINATION ${MODBUS_CMAKE_CONFIG_DIR}
)

install(EXPORT modbusTargets
    FILE modbusTargets.cmake
    NAMESPACE modbus::
    DESTINATION ${MODBUS_CMAKE_CONFIG_DIR}
)
