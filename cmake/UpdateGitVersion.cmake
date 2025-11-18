# UpdateGitVersion.cmake - 在构建时检查Git版本信息
# 这个脚本会在每次构建时运行，确保版本信息是最新的

message(STATUS "Checking Git version during build...")

if(NOT GIT_EXECUTABLE)
    message(STATUS "Git executable not provided")
    return()
endif()

if(NOT EXISTS ${GIT_EXECUTABLE})
    message(STATUS "Git executable not found: ${GIT_EXECUTABLE}")
    return()
endif()

# 检查是否在Git仓库中
execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --git-dir
    WORKING_DIRECTORY ${SOURCE_DIR}
    RESULT_VARIABLE GIT_REPO_CHECK
    OUTPUT_QUIET
    ERROR_QUIET
)

set(CURRENT_VERSION ${DEFAULT_VERSION})

if(GIT_REPO_CHECK EQUAL 0)
    # 获取最新的tag
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0
        WORKING_DIRECTORY ${SOURCE_DIR}
        OUTPUT_VARIABLE GIT_TAG_RAW
        RESULT_VARIABLE GIT_TAG_RESULT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    
    if(GIT_TAG_RESULT EQUAL 0 AND GIT_TAG_RAW)
        # 移除可能的前缀'v'或'V'
        string(REGEX REPLACE "^[vV]" "" GIT_TAG_CLEAN "${GIT_TAG_RAW}")
        
        # 检查是否符合版本号格式
        if(GIT_TAG_CLEAN MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)([-\\.].*)?$")
            set(CURRENT_VERSION "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}")
            message(STATUS "Build-time Git version: ${CURRENT_VERSION} (from tag: ${GIT_TAG_RAW})")
        else()
            message(STATUS "Build-time Git tag '${GIT_TAG_RAW}' does not match version format")
        endif()
    else()
        message(STATUS "Build-time: No Git tags found")
    endif()
else()
    message(STATUS "Build-time: Not in a Git repository")
endif()

# 读取之前的版本信息（如果存在）
set(PREVIOUS_VERSION "")
if(EXISTS ${VERSION_FILE})
    file(READ ${VERSION_FILE} PREVIOUS_VERSION)
    string(STRIP "${PREVIOUS_VERSION}" PREVIOUS_VERSION)
endif()

# 如果版本发生变化，输出警告
if(NOT "${CURRENT_VERSION}" STREQUAL "${PREVIOUS_VERSION}")
    message(STATUS "Version changed from '${PREVIOUS_VERSION}' to '${CURRENT_VERSION}'")
    message(WARNING "Git version has changed! You may need to reconfigure CMake to use the new version.")
    message(WARNING "Current CMake project version may be outdated.")
    message(WARNING "To update: delete build directory and reconfigure, or run: cmake --fresh .")
endif()

# 写入当前版本到文件
file(WRITE ${VERSION_FILE} "${CURRENT_VERSION}")
