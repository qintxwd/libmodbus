/*
 * libmodbus C++ Example
 * RTU Client Example
 */

#include <modbus/modbus.hpp>
#include <iostream>

int main() {
    try {
        std::cout << "libmodbus RTU Client Example" << std::endl;
        
        // 创建 RTU 客户端
        // Windows: "COM1", "COM2", etc.
        // Linux: "/dev/ttyUSB0", "/dev/ttyS0", etc.
#ifdef _WIN32
        const std::string device = "COM1";
#else
        const std::string device = "/dev/ttyUSB0";
#endif
        
        std::cout << "Opening serial port: " << device << std::endl;
        modbus::ModbusRTU client(device, 9600, 'N', 8, 1);
        
        // 设置从站地址
        client.set_slave(1);
        
        // 连接
        std::cout << "Connecting..." << std::endl;
        client.connect();
        std::cout << "Connected!" << std::endl;
        
        // 读取保持寄存器
        std::cout << "\nReading 10 holding registers from address 0..." << std::endl;
        auto registers = client.read_holding_registers(0, 10);
        
        std::cout << "Read " << registers.size() << " registers:" << std::endl;
        for (size_t i = 0; i < registers.size(); ++i) {
            std::cout << "  Register[" << i << "] = " << registers[i] << std::endl;
        }
        
        // 写入寄存器
        std::cout << "\nWriting value 1234 to register 0..." << std::endl;
        client.write_register(0, 1234);
        std::cout << "Write successful!" << std::endl;
        
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
