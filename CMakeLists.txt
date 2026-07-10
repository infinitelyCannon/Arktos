cmake_minimum_required(VERSION 3.14)
project(Arktos VERSION 0.1 LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	set(CMAKE_CXX_EXTENTIONS OFF)

	# Let's nicely support folders in IDEs
  	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

option(INCLUDE_EXAMPLE "Option to build as a standalone executable" OFF)

add_subdirectory(src)

if(INCLUDE_EXAMPLE)
	add_subdirectory(app)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    include(CTest)
    add_subdirectory(tests)
endif()