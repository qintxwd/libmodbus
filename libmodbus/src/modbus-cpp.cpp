/*
 * libmodbus C++ Implementation
 * Copyright © 2025
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <modbus/modbus.hpp>
#include "modbus-core.h"
#include "modbus-tcp.h"
#include "modbus-rtu.h"
#include <modbus/modbus-version.h>
#include <cstring>
#include <cerrno>

namespace modbus {

// 前向声明的实现类
class ModbusImpl {
public:
    modbus_t* ctx = nullptr;
    
    explicit ModbusImpl(modbus_t* c) : ctx(c) {}
    
    ~ModbusImpl() {
        if (ctx) {
            modbus_close(ctx);
            modbus_free(ctx);
        }
    }
};

class MappingImpl {
public:
    modbus_mapping_t* mapping = nullptr;
    
    explicit MappingImpl(modbus_mapping_t* m) : mapping(m) {}
    
    ~MappingImpl() {
        if (mapping) {
            modbus_mapping_free(mapping);
        }
    }
};

// Exception 实现
Exception::Exception(const std::string& message)
    : std::runtime_error(message), error_code_(errno) {}

Exception::Exception(const std::string& message, int error_code)
    : std::runtime_error(message), error_code_(error_code) {}

int Exception::error_code() const noexcept {
    return error_code_;
}

// Modbus 基类实现
Modbus::Modbus(std::unique_ptr<ModbusImpl> impl) : impl_(std::move(impl)) {
    if (!impl_ || !impl_->ctx) {
        throw Exception("创建 Modbus 上下文失败");
    }
}

Modbus::~Modbus() = default;

Modbus::Modbus(Modbus&& other) noexcept = default;
Modbus& Modbus::operator=(Modbus&& other) noexcept = default;

void Modbus::connect() {
    if (modbus_connect(impl_->ctx) == -1) {
        throw Exception("连接失败: " + std::string(modbus_strerror(errno)));
    }
}

void Modbus::close() {
    if (impl_ && impl_->ctx) {
        modbus_close(impl_->ctx);
    }
}

void Modbus::set_slave(int slave) {
    if (modbus_set_slave(impl_->ctx, slave) == -1) {
        throw Exception("设置从站地址失败");
    }
}

void Modbus::set_debug(bool on) {
    modbus_set_debug(impl_->ctx, on ? TRUE : FALSE);
}

void Modbus::set_response_timeout(uint32_t sec, uint32_t usec) {
    if (modbus_set_response_timeout(impl_->ctx, sec, usec) == -1) {
        throw Exception("设置响应超时失败");
    }
}

std::vector<uint8_t> Modbus::read_coils(int addr, int nb) {
    std::vector<uint8_t> dest(nb);
    int rc = modbus_read_bits(impl_->ctx, addr, nb, dest.data());
    if (rc == -1) {
        throw Exception("读取线圈失败: " + std::string(modbus_strerror(errno)));
    }
    dest.resize(rc);
    return dest;
}

std::vector<uint8_t> Modbus::read_discrete_inputs(int addr, int nb) {
    std::vector<uint8_t> dest(nb);
    int rc = modbus_read_input_bits(impl_->ctx, addr, nb, dest.data());
    if (rc == -1) {
        throw Exception("读取离散输入失败: " + std::string(modbus_strerror(errno)));
    }
    dest.resize(rc);
    return dest;
}

std::vector<uint16_t> Modbus::read_holding_registers(int addr, int nb) {
    std::vector<uint16_t> dest(nb);
    int rc = modbus_read_registers(impl_->ctx, addr, nb, dest.data());
    if (rc == -1) {
        throw Exception("读取保持寄存器失败: " + std::string(modbus_strerror(errno)));
    }
    dest.resize(rc);
    return dest;
}

std::vector<uint16_t> Modbus::read_input_registers(int addr, int nb) {
    std::vector<uint16_t> dest(nb);
    int rc = modbus_read_input_registers(impl_->ctx, addr, nb, dest.data());
    if (rc == -1) {
        throw Exception("读取输入寄存器失败: " + std::string(modbus_strerror(errno)));
    }
    dest.resize(rc);
    return dest;
}

void Modbus::write_coil(int addr, bool status) {
    if (modbus_write_bit(impl_->ctx, addr, status ? TRUE : FALSE) == -1) {
        throw Exception("写入线圈失败: " + std::string(modbus_strerror(errno)));
    }
}

void Modbus::write_register(int addr, uint16_t value) {
    if (modbus_write_register(impl_->ctx, addr, value) == -1) {
        throw Exception("写入寄存器失败: " + std::string(modbus_strerror(errno)));
    }
}

void Modbus::write_coils(int addr, const std::vector<uint8_t>& src) {
    if (modbus_write_bits(impl_->ctx, addr, src.size(), src.data()) == -1) {
        throw Exception("写入多个线圈失败: " + std::string(modbus_strerror(errno)));
    }
}

void Modbus::write_registers(int addr, const std::vector<uint16_t>& src) {
    if (modbus_write_registers(impl_->ctx, addr, src.size(), src.data()) == -1) {
        throw Exception("写入多个寄存器失败: " + std::string(modbus_strerror(errno)));
    }
}

std::vector<uint16_t> Modbus::write_and_read_registers(
    int write_addr, const std::vector<uint16_t>& src,
    int read_addr, int read_nb) {
    
    std::vector<uint16_t> dest(read_nb);
    int rc = modbus_write_and_read_registers(impl_->ctx,
        write_addr, src.size(), src.data(),
        read_addr, read_nb, dest.data());
    
    if (rc == -1) {
        throw Exception("读写寄存器失败: " + std::string(modbus_strerror(errno)));
    }
    dest.resize(rc);
    return dest;
}

// ModbusTCP 实现
ModbusTCP::ModbusTCP(const std::string& ip, int port)
    : Modbus(std::make_unique<ModbusImpl>(modbus_new_tcp(ip.c_str(), port))) {}

// ModbusRTU 实现
ModbusRTU::ModbusRTU(const std::string& device, int baud,
                     char parity, int data_bit, int stop_bit)
    : Modbus(std::make_unique<ModbusImpl>(
        modbus_new_rtu(device.c_str(), baud, parity, data_bit, stop_bit))) {}

void ModbusRTU::set_serial_mode(int mode) {
    if (modbus_rtu_set_serial_mode(impl_->ctx, mode) == -1) {
        throw Exception("设置串口模式失败");
    }
}

void ModbusRTU::set_rts(int mode) {
    if (modbus_rtu_set_rts(impl_->ctx, mode) == -1) {
        throw Exception("设置 RTS 模式失败");
    }
}

// ModbusTCPServer 实现
class ServerImpl : public ModbusImpl {
public:
    int socket = -1;
    
    explicit ServerImpl(modbus_t* c) : ModbusImpl(c) {}
    
    ~ServerImpl() {
        if (socket != -1) {
#ifdef _WIN32
            closesocket(socket);
#else
            ::close(socket);
#endif
        }
    }
};

ModbusTCPServer::ModbusTCPServer(const std::string& ip, int port)
    : impl_(std::make_unique<ServerImpl>(modbus_new_tcp(ip.c_str(), port))) {
    if (!impl_ || !impl_->ctx) {
        throw Exception("创建 TCP 服务器上下文失败");
    }
}

ModbusTCPServer::~ModbusTCPServer() = default;

void ModbusTCPServer::listen(int nb_connection) {
    ServerImpl* server = static_cast<ServerImpl*>(impl_.get());
    server->socket = modbus_tcp_listen(server->ctx, nb_connection);
    if (server->socket == -1) {
        throw Exception("监听失败: " + std::string(modbus_strerror(errno)));
    }
}

std::unique_ptr<Modbus> ModbusTCPServer::accept() {
    ServerImpl* server = static_cast<ServerImpl*>(impl_.get());
    if (server->socket == -1) {
        throw Exception("服务器未监听，无法接受连接");
    }
    if (modbus_tcp_accept(server->ctx, &server->socket) == -1) {
        throw Exception("接受连接失败: " + std::string(modbus_strerror(errno)));
    }
    int client_socket = modbus_get_socket(server->ctx);
    
    modbus_t* client_ctx = modbus_new_tcp("0.0.0.0", DEFAULT_TCP_PORT);
    if (!client_ctx) {
        throw Exception("创建客户端上下文失败");
    }
    modbus_set_socket(client_ctx, client_socket);
    
    return std::unique_ptr<Modbus>(new Modbus(std::make_unique<ModbusImpl>(client_ctx)));
}

// Mapping 实现
ModbusTCPServer::Mapping::Mapping(int nb_bits, int nb_input_bits,
                                  int nb_registers, int nb_input_registers)
    : impl_(std::make_unique<MappingImpl>(
        modbus_mapping_new(nb_bits, nb_input_bits, nb_registers, nb_input_registers))) {
    if (!impl_ || !impl_->mapping) {
        throw Exception("创建数据映射失败: " + std::string(modbus_strerror(errno)));
    }
}

ModbusTCPServer::Mapping::~Mapping() = default;

uint8_t& ModbusTCPServer::Mapping::coil(int addr) {
    return impl_->mapping->tab_bits[addr];
}

uint8_t& ModbusTCPServer::Mapping::discrete_input(int addr) {
    return impl_->mapping->tab_input_bits[addr];
}

uint16_t& ModbusTCPServer::Mapping::holding_register(int addr) {
    return impl_->mapping->tab_registers[addr];
}

uint16_t& ModbusTCPServer::Mapping::input_register(int addr) {
    return impl_->mapping->tab_input_registers[addr];
}

int ModbusTCPServer::receive_and_reply(Modbus& client, Mapping& mapping) {
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    int rc = modbus_receive(client.impl_->ctx, query);
    
    if (rc > 0) {
        modbus_reply(client.impl_->ctx, query, rc, mapping.impl_->mapping);
    } else if (rc == -1) {
        return -1; // 连接关闭
    }
    return rc;
}

// 版本信息函数
std::string version() {
    return LIBMODBUS_VERSION_STRING;
}

int version_major() {
    return LIBMODBUS_VERSION_MAJOR;
}

int version_minor() {
    return LIBMODBUS_VERSION_MINOR;
}

int version_micro() {
    return LIBMODBUS_VERSION_MICRO;
}

} // namespace modbus
