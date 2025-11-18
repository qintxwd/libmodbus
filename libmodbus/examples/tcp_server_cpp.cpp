/*
 * libmodbus C++ Example
 * TCP Server Example - 配合 client 测试所有 API
 */

#include <modbus/modbus.hpp>
#include <iostream>
#include <csignal>
#include <atomic>

std::atomic<bool> running(true);

void signal_handler(int) {
    running = false;
}

int main() {
    std::signal(SIGINT, signal_handler);
    
    try {
        std::cout << "================================" << std::endl;
        std::cout << "libmodbus TCP Server" << std::endl;
        std::cout << "Version: " << modbus::version() << std::endl;
        std::cout << "================================" << std::endl;
        
        // 创建服务器
        modbus::ModbusTCPServer server("127.0.0.1", 1502);
        
        // 创建数据映射
        modbus::ModbusTCPServer::Mapping mapping;
        
        // 初始化测试数据
        std::cout << "\nInitializing server data..." << std::endl;
        
        // 初始化 coils (位输出)
        for (int i = 0; i < 20; ++i) {
            mapping.coil(i) = (i % 2 == 0);  // 偶数位为 true
        }
        
        // 初始化 discrete inputs (位输入)
        for (int i = 0; i < 20; ++i) {
            mapping.discrete_input(i) = (i % 3 == 0);  // 每3个为 true
        }
        
        // 初始化 holding registers (保持寄存器)
        for (int i = 0; i < 50; ++i) {
            mapping.holding_register(i) = i * 100;
        }
        
        // 初始化 input registers (输入寄存器)
        for (int i = 0; i < 50; ++i) {
            mapping.input_register(i) = i * 50 + 1000;
        }
        
        std::cout << "Server data initialized." << std::endl;
        std::cout << "  - Coils: 20 (address 0-19)" << std::endl;
        std::cout << "  - Discrete Inputs: 20 (address 0-19)" << std::endl;
        std::cout << "  - Holding Registers: 50 (address 0-49)" << std::endl;
        std::cout << "  - Input Registers: 50 (address 0-49)" << std::endl;
        
        std::cout << "\nListening on 127.0.0.1:1502..." << std::endl;
        server.listen(1);
        
        std::cout << "Waiting for client connection..." << std::endl;
        std::cout << "(Press Ctrl+C to stop)\n" << std::endl;
        
        auto client = server.accept();
        std::cout << "\n>>> Client connected! <<<\n" << std::endl;
        
        int request_count = 0;
        while (running) {
            int rc = server.receive_and_reply(*client, mapping);
            if (rc == -1) {
                std::cout << "\n>>> Client disconnected <<<" << std::endl;
                break;
            }
            
            if (rc > 0) {
                request_count++;
                std::cout << "[" << request_count << "] Request processed" << std::endl;
            }
        }
        
        std::cout << "\n================================" << std::endl;
        std::cout << "Server shutting down..." << std::endl;
        std::cout << "Total requests: " << request_count << std::endl;
        std::cout << "================================" << std::endl;
        
    } catch (const modbus::Exception& e) {
        std::cerr << "Modbus error: " << e.what() 
                  << " (code: " << e.error_code() << ")" << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
