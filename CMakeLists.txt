cmake_minimum_required(VERSION 3.20)
project(stdexec_example VERSION 0.0.1)
find_package(stdexec CONFIG REQUIRED)
add_executable(${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_sources(${PROJECT_NAME}
PRIVATE
	main.cpp
)
target_link_libraries(${PROJECT_NAME} PRIVATE STDEXEC::stdexec)

