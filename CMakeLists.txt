cmake_minimum_required(VERSION 3.28)
project(stdexec_example VERSION 0.0.1)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
find_package(stdexec CONFIG REQUIRED)
add_executable(${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_sources(${PROJECT_NAME}
PRIVATE
	main.cpp
PRIVATE
  FILE_SET cxx_modules TYPE CXX_MODULES FILES
	module.cppm
)
target_link_libraries(${PROJECT_NAME} PRIVATE STDEXEC::stdexec)

