# CMakeLists.txt: Top-level CMake project file for libmodbus
# Perform global configuration and include sub-projects.
#
cmake_minimum_required(VERSION 3.8)


# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# 默认版本号（如果无法从Git获取）
set(DEFAULT_VERSION "1.0.1")

# 创建版本缓存文件路径
set(VERSION_CACHE_FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake_git_version_cache.txt")

# 检查是否需要强制重新获取Git版本
set(FORCE_GIT_CHECK FALSE)

# 检查.git/refs目录的修改时间（如果存在的话）
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git/refs")
    # 获取refs目录的修改时间
    file(GLOB_RECURSE GIT_REF_FILES "${CMAKE_CURRENT_SOURCE_DIR}/.git/refs/*")
    if(GIT_REF_FILES)
        # 如果缓存文件不存在，或者任何ref文件比缓存文件新，则强制检查
        if(NOT EXISTS ${VERSION_CACHE_FILE})
            set(FORCE_GIT_CHECK TRUE)
        else()
            foreach(ref_file ${GIT_REF_FILES})
                if(${ref_file} IS_NEWER_THAN ${VERSION_CACHE_FILE})
                    set(FORCE_GIT_CHECK TRUE)
                    message(STATUS "Git refs updated, forcing version check...")
                    break()
                endif()
            endforeach()
        endif()
    endif()
endif()

# 尝试从Git获取最新tag作为版本号
# 注意：每次构建都会重新检查Git信息，不使用缓存
find_package(Git QUIET)
set(PROJECT_VERSION_FROM_GIT "")

if(GIT_FOUND)
    message(STATUS "Git found: ${GIT_EXECUTABLE}")
    
    # 检查是否在Git仓库中 - 每次都执行，不使用缓存
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --git-dir
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE GIT_REPO_CHECK
        OUTPUT_QUIET
        ERROR_QUIET
    )
    
    if(GIT_REPO_CHECK EQUAL 0)
        message(STATUS "Git repository detected")
        
        # 获取最新的tag - 每次都执行，不使用缓存
        execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_TAG_RAW
            RESULT_VARIABLE GIT_TAG_RESULT
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        
        if(GIT_TAG_RESULT EQUAL 0 AND GIT_TAG_RAW)
            message(STATUS "Latest Git tag: ${GIT_TAG_RAW}")
            
            # 验证tag是否符合版本号格式 (例如: v1.2.3, 1.2.3, v1.2.3-beta等)
            # 移除可能的前缀'v'或'V'
            string(REGEX REPLACE "^[vV]" "" GIT_TAG_CLEAN "${GIT_TAG_RAW}")
            
            # 检查是否符合版本号格式 X.Y.Z 或 X.Y.Z-suffix
            if(GIT_TAG_CLEAN MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)([-\\.].*)?$")
                set(PROJECT_VERSION_FROM_GIT "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}")
                message(STATUS "Valid version format detected: ${PROJECT_VERSION_FROM_GIT}")
                
                # 获取详细的版本信息（包括commit信息）- 每次都执行
                execute_process(
                    COMMAND ${GIT_EXECUTABLE} describe --tags --dirty=-dirty
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    OUTPUT_VARIABLE GIT_DESCRIBE
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_QUIET
                )
                
                if(GIT_DESCRIBE)
                    message(STATUS "Git describe: ${GIT_DESCRIBE}")
                endif()
                
                # 获取当前commit hash用于强制重新检测
                execute_process(
                    COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    OUTPUT_VARIABLE GIT_COMMIT_HASH
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_QUIET
                )
                
                if(GIT_COMMIT_HASH)
                    message(STATUS "Current commit: ${GIT_COMMIT_HASH}")
                endif()
            else()
                message(STATUS "Git tag '${GIT_TAG_RAW}' does not match version format (X.Y.Z), using default version")
            endif()
        else()
            message(STATUS "No Git tags found, using default version")
        endif()
    else()
        message(STATUS "Not in a Git repository, using default version")
    endif()
else()
    message(STATUS "Git not found, using default version")
endif()

# 确定最终使用的版本号
if(PROJECT_VERSION_FROM_GIT)
    set(FINAL_VERSION ${PROJECT_VERSION_FROM_GIT})
    message(STATUS "Using Git tag version: ${FINAL_VERSION}")
else()
    set(FINAL_VERSION ${DEFAULT_VERSION})
    message(STATUS "Using default version: ${FINAL_VERSION}")
endif()

# 更新版本缓存文件
file(WRITE ${VERSION_CACHE_FILE} "${FINAL_VERSION}\n${CMAKE_CURRENT_LIST_FILE}")

project ("libmodbus" VERSION ${FINAL_VERSION} LANGUAGES C CXX)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include sub-projects.
add_subdirectory("libmodbus")
