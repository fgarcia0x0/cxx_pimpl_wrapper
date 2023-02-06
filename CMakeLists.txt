cmake_minimum_required(VERSION 3.18)
project(pimpl_wrapper_test VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
	target_compile_options(/W4 /analyzer)	
else()
	add_compile_options(
		-Wall 
		-Wextra 
		-Wshadow 
		-Wconversion 
		-Wpedantic 
		-Werror 
		-Wno-unused-variable 
		-Wno-unused-parameter 
		#-Wno-analyzer-possible-null-dereference
		-Wno-unused-lambda-capture
		-fstack-protector-strong
	)

	if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
		#add_compile_options(-fsanitize=address)
		#add_link_options(-fsanitize=address)
	elseif(${CMAKE_BUILD_TYPE} MATCHES "Release")
		add_compile_definitions(_FORTIFY_SOURCE=3)
	endif()
endif()

set(SRCS 
	include/pimpl_wrapper.hpp 
	test/pimpl_device_test.h 
	test/pimpl_device_test.cpp 
	test/pimpl_wrapper_test.cpp
)

add_executable(pimpl_wrapper_test ${SRCS})

target_include_directories(pimpl_wrapper_test PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_include_directories(pimpl_wrapper_test PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/test>
)

set_target_properties(pimpl_wrapper_test PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)