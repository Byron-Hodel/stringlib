cmake_minimum_required(VERSION 3.20)
project(str)

add_compile_options(-march=native)

add_executable(str
	str.h
	main.c
)
