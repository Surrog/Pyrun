cmake_minimum_required (VERSION 3.6)

project (pyrun)

set (CMAKE_CXX_STANDARD 17)

set (SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
set (JSONCPP_DIR "${PROJECT_SOURCE_DIR}/jsoncpp")

set (SOURCES
		${SOURCE_DIR}/main.cpp
		${SOURCE_DIR}/configuration.hpp
		${SOURCE_DIR}/configuration.cpp
		${SOURCE_DIR}/aprocess.hpp
		${SOURCE_DIR}/linux_process.hpp
		${SOURCE_DIR}/windows_process.hpp
		${SOURCE_DIR}/environ.hpp
		${SOURCE_DIR}/linux_environ.hpp
		${SOURCE_DIR}/windows_environ.hpp
		${SOURCE_DIR}/python_lnk.hpp
		${SOURCE_DIR}/python_lnk.cpp
	
		${JSONCPP_DIR}/dist/jsoncpp.cpp
		${JSONCPP_DIR}/dist/json/json.h
		${JSONCPP_DIR}/dist/json/json-forwards.h
		)


add_executable(pyrun ${SOURCES})

if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.1) #added to support old travis gcc-8
	target_link_libraries(pyrun stdc++fs)
endif()

if (UNIX)
	target_link_libraries(pyrun dl)
endif()

target_include_directories(pyrun PRIVATE "${PROJECT_SOURCE_DIR}/jsoncpp/dist")
target_include_directories(pyrun PRIVATE "${SOURCE_DIR}")

target_compile_definitions(pyrun PRIVATE SOLUTION_DIR="${PROJECT_SOURCE_DIR}")