project(Maloader)
cmake_minimum_required(VERSION 2.8)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE DEBUG CACHE STRING 
	    "Choose the type of build, options are: None Debug Release RelWithDebInfo." 
	    FORCE)
endif(NOT CMAKE_BUILD_TYPE)

#FIXME find out how to be able to compile for 32bit on 64bit

set(CMAKE_C_FLAGS "-Wall -Werror")
set(CMAKE_C_FLAGS_DEBUG "-O0 -ggdb -Wall -Werror")
set(CMAKE_C_FLAGS_RELEASE "-O0 -ggdb -Wall -Werror")
set(CMAKE_CXX_FLAGS "-ggdb -Wall -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -Wall -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "-O0 -ggdb -Wall -Werror")

include_directories(.)
include_directories(include)

add_executable(
	ld-mac
	log.cc
        ld-mac.cc
        mach-o.cc
	fat.h
	fat.cc
	include/mach/machine.h
	)

target_link_libraries(
	ld-mac
	dl
	)

add_executable(
	extract
	extract.cc
	fat.cc
	)

add_executable(
        macho2elf
	macho2elf.cc
	mach-o.cc 
	fat.cc 
	log.cc
	)

