# modbus C++ 示例

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# TCP 客户端示例
add_executable(tcp_client_cpp tcp_client_cpp.cpp)
target_link_libraries(tcp_client_cpp modbus)
target_include_directories(tcp_client_cpp PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_BINARY_DIR}/../include
)

# TCP 服务器示例
add_executable(tcp_server_cpp tcp_server_cpp.cpp)
target_link_libraries(tcp_server_cpp modbus)
target_include_directories(tcp_server_cpp PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_BINARY_DIR}/../include
)

# RTU 客户端示例
add_executable(rtu_client_cpp rtu_client_cpp.cpp)
target_link_libraries(rtu_client_cpp modbus)
target_include_directories(rtu_client_cpp PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_BINARY_DIR}/../include
)

# 在 Windows 上，将 modbus.dll 复制到示例程序旁边
if(WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET tcp_client_cpp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:modbus>
        $<TARGET_FILE_DIR:tcp_client_cpp>
        COMMENT "Copying modbus.dll to tcp_client_cpp directory"
    )
    
    add_custom_command(TARGET tcp_server_cpp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:modbus>
        $<TARGET_FILE_DIR:tcp_server_cpp>
        COMMENT "Copying modbus.dll to tcp_server_cpp directory"
    )
    
    add_custom_command(TARGET rtu_client_cpp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:modbus>
        $<TARGET_FILE_DIR:rtu_client_cpp>
        COMMENT "Copying modbus.dll to rtu_client_cpp directory"
    )
endif()
