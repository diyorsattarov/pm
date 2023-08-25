cmake_minimum_required(VERSION 3.25.1)

project(projectum_test)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/test/bin)

add_executable(test_main
	test_beast.cpp
)

find_package(nlohmann_json)

target_link_libraries(test_main PRIVATE gtest spdlog nlohmann_json::nlohmann_json)

add_custom_target(clean_test
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/test/bin

    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/test/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/test/CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/test/Makefile
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/test/cmake_install.cmake
	COMMENT "Cleaning build files"
)


