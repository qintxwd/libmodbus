# A groovy modbus library

![Build Status](https://github.com/stephane/libmodbus/actions/workflows/build.yml/badge.svg)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/8058/badge.svg)](https://scan.coverity.com/projects/libmodbus)

> **注意**: 这是 libmodbus 的 CMake 构建版本，已配置支持 Windows 和 Linux 跨平台编译。

## 概述

libmodbus is a free software library to send/receive data with a device which
respects the Modbus protocol. This library can use a serial port or an Ethernet
connection.

The functions included in the library have been derived from the Modicon Modbus
Protocol Reference Guide which can be obtained from [www.modbus.org](http://www.modbus.org).

The license of libmodbus is *LGPL v2.1 or later*.

The official website is [www.libmodbus.org](http://www.libmodbus.org). The
website contains the latest version of the documentation.

The library is written in C and designed to run on Linux, Mac OS X, FreeBSD, Embox,
QNX and Windows.

You can use the library on MCUs with Embox RTOS.

## CMake 构建 (推荐)

本项目已配置 CMake 构建系统，支持跨平台编译。

### Windows 快速开始

```powershell
# 配置 (x64 Release)
cmake --preset x64-release

# 编译
cmake --build out/build/x64-release --config Release

# 生成的库位于
# out/build/x64-release/libmodbus/Release/modbus.dll
```

### Linux 快速开始

```bash
# 配置
cmake --preset linux-release

# 编译
cmake --build out/build/linux-release

# 生成的库位于
# out/build/linux-release/libmodbus/libmodbus.so
```

### 详细文档

- **[BUILD.md](BUILD.md)** - 完整的构建说明和选项
- **[EXAMPLES.md](EXAMPLES.md)** - API 使用示例和代码
- **[CMAKE_SUMMARY.md](CMAKE_SUMMARY.md)** - CMake 配置总结

## 传统构建方法 (Autotools)

如果你更喜欢使用 autotools，可以继续使用传统方法：

## Installation

You will only need to install automake, autoconf, libtool and a C compiler (gcc
or clang) to compile the library and asciidoc and xmlto to generate the
documentation (optional).

To install, just run the usual dance, `./configure && make install`. Run
`./autogen.sh` first to generate the `configure` script if required.

You can change installation directory with prefix option, eg. `./configure
--prefix=/usr/local/`. You have to check that the installation library path is
properly set up on your system (*/etc/ld.so.conf.d*) and library cache is up to
date (run `ldconfig` as root if required).

The library provides a *libmodbus.pc* file to use with `pkg-config` to ease your
program compilation and linking.

If you want to compile with Microsoft Visual Studio, you should follow the
instructions in `./src/win32/README.md`.

To compile under Windows, install [MinGW](http://www.mingw.org/) and MSYS then
select the common packages (gcc, automake, libtool, etc). The directory
*./src/win32/* contains a Visual C project.

To compile under OS X with [homebrew](http://mxcl.github.com/homebrew/), you
will need to install the following dependencies first: `brew install autoconf
automake libtool`.

To build under Embox, you have to use its build system.

## Testing

Some tests are provided in *tests* directory, you can freely edit the source
code to fit your needs (it's Free Software :).

See *tests/README* for a description of each program.

For a quick test of libmodbus, you can run the following programs in two shells:

1. ./unit-test-server
2. ./unit-test-client

By default, all TCP unit tests will be executed (see --help for options).

It's also possible to run the unit tests with `make check`.

## To report a bug or to contribute

See [CONTRIBUTING](CONTRIBUTING.md) document.

## Documentation

You can serve the local documentation with:

```shell
pip install mkdocs-material
mkdocs serve
```
