/*
 * libmodbus C++ Example
 * TCP Client Example - 测试所有 Modbus API
 */

#include <modbus/modbus.hpp>
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

void print_separator() {
    std::cout << "\n" << std::string(60, '-') << std::endl;
}

int main() {
    try {
        std::cout << "================================" << std::endl;
        std::cout << "libmodbus TCP Client Test" << std::endl;
        std::cout << "Version: " << modbus::version() << std::endl;
        std::cout << "================================" << std::endl;
        
        // 创建 TCP 客户端
        modbus::ModbusTCP client("127.0.0.1", 1502);
        client.set_slave(1);
        client.set_debug(true);
        
        // 连接到服务器
        std::cout << "\nConnecting to 127.0.0.1:1502..." << std::endl;
        client.connect();
        std::cout << ">>> Connected successfully! <<<\n" << std::endl;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // ===== 测试 1: 读取 Coils (位输出) =====
        print_separator();
        std::cout << "TEST 1: Read Coils (FC 0x01)" << std::endl;
        print_separator();
        auto coils = client.read_coils(0, 10);
        std::cout << "Read " << coils.size() << " coils:" << std::endl;
        for (size_t i = 0; i < coils.size(); ++i) {
            std::cout << "  Coil[" << i << "] = " << (coils[i] ? "ON" : "OFF") << std::endl;
        }
        
        // ===== 测试 2: 读取 Discrete Inputs (位输入) =====
        print_separator();
        std::cout << "TEST 2: Read Discrete Inputs (FC 0x02)" << std::endl;
        print_separator();
        auto inputs = client.read_discrete_inputs(0, 10);
        std::cout << "Read " << inputs.size() << " discrete inputs:" << std::endl;
        for (size_t i = 0; i < inputs.size(); ++i) {
            std::cout << "  Input[" << i << "] = " << (inputs[i] ? "ON" : "OFF") << std::endl;
        }
        
        // ===== 测试 3: 读取 Holding Registers (保持寄存器) =====
        print_separator();
        std::cout << "TEST 3: Read Holding Registers (FC 0x03)" << std::endl;
        print_separator();
        auto holding_regs = client.read_holding_registers(0, 10);
        std::cout << "Read " << holding_regs.size() << " holding registers:" << std::endl;
        for (size_t i = 0; i < holding_regs.size(); ++i) {
            std::cout << "  Register[" << i << "] = " << holding_regs[i]
                      << " (0x" << std::hex << std::setw(4) << std::setfill('0')
                      << holding_regs[i] << std::dec << ")" << std::endl;
        }
        
        // ===== 测试 4: 读取 Input Registers (输入寄存器) =====
        print_separator();
        std::cout << "TEST 4: Read Input Registers (FC 0x04)" << std::endl;
        print_separator();
        auto input_regs = client.read_input_registers(0, 10);
        std::cout << "Read " << input_regs.size() << " input registers:" << std::endl;
        for (size_t i = 0; i < input_regs.size(); ++i) {
            std::cout << "  InputReg[" << i << "] = " << input_regs[i] << std::endl;
        }
        
        // ===== 测试 5: 写入单个 Coil =====
        print_separator();
        std::cout << "TEST 5: Write Single Coil (FC 0x05)" << std::endl;
        print_separator();
        std::cout << "Writing coil 5 = ON..." << std::endl;
        client.write_coil(5, true);
        std::cout << "Write successful!" << std::endl;
        auto verify_coil = client.read_coils(5, 1);
        std::cout << "Verify: Coil[5] = " << (verify_coil[0] ? "ON" : "OFF") << std::endl;
        
        // ===== 测试 6: 写入单个寄存器 =====
        print_separator();
        std::cout << "TEST 6: Write Single Register (FC 0x06)" << std::endl;
        print_separator();
        std::cout << "Writing register 5 = 12345..." << std::endl;
        client.write_register(5, 12345);
        std::cout << "Write successful!" << std::endl;
        auto verify_reg = client.read_holding_registers(5, 1);
        std::cout << "Verify: Register[5] = " << verify_reg[0] << std::endl;
        
        // ===== 测试 7: 写入多个 Coils =====
        print_separator();
        std::cout << "TEST 7: Write Multiple Coils (FC 0x0F)" << std::endl;
        print_separator();
        std::vector<uint8_t> coil_values = {1, 0, 1, 1, 0};  // 1=ON, 0=OFF
        std::cout << "Writing 5 coils starting at address 10..." << std::endl;
        client.write_coils(10, coil_values);
        std::cout << "Write successful!" << std::endl;
        auto verify_coils = client.read_coils(10, 5);
        std::cout << "Verify: ";
        for (size_t i = 0; i < verify_coils.size(); ++i) {
            std::cout << "Coil[" << (10 + i) << "]=" << (verify_coils[i] ? "ON" : "OFF") << " ";
        }
        std::cout << std::endl;
        
        // ===== 测试 8: 写入多个寄存器 =====
        print_separator();
        std::cout << "TEST 8: Write Multiple Registers (FC 0x10)" << std::endl;
        print_separator();
        std::vector<uint16_t> reg_values = {1111, 2222, 3333, 4444, 5555};
        std::cout << "Writing 5 registers starting at address 20..." << std::endl;
        client.write_registers(20, reg_values);
        std::cout << "Write successful!" << std::endl;
        auto verify_regs = client.read_holding_registers(20, 5);
        std::cout << "Verify: ";
        for (size_t i = 0; i < verify_regs.size(); ++i) {
            std::cout << verify_regs[i] << " ";
        }
        std::cout << std::endl;
        
        // ===== 测试 9: 读写多个寄存器 =====
        print_separator();
        std::cout << "TEST 9: Read/Write Multiple Registers (FC 0x17)" << std::endl;
        print_separator();
        std::vector<uint16_t> write_vals = {9999, 8888, 7777};
        std::cout << "Write 3 registers to address 30, then read 5 from address 28..." << std::endl;
        auto rw_result = client.write_and_read_registers(30, write_vals, 28, 5);
        std::cout << "Read result: ";
        for (auto val : rw_result) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
        
        // ===== 测试总结 =====
        print_separator();
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "ALL TESTS COMPLETED SUCCESSFULLY!" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        
        std::cout << "\nClosing connection..." << std::endl;
        
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
