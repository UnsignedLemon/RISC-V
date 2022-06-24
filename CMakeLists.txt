cmake_minimum_required(VERSION 3.16)
project (RISCV)

set(CMAKE_CXX_STANDARD 11)
add_executable(code
	main.cpp
	data/data.cpp
	reg/reg.cpp
	units/units.cpp
)
