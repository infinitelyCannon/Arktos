# Setting this to have windows export all symbols by default and not have to manually "declspec(dllexport)" everything
if(WIN32)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif(WIN32)

configure_file("${PROJECT_SOURCE_DIR}/cmake/Version.h.in" "${PROJECT_SOURCE_DIR}/inc/Version.h")

# Note that including headers are optional and do not affect add_library,
# but they will not show up in IDEs unless they're added this way.
file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${Arktos_SOURCE_DIR}/inc/*.h")

set(PLATFORM_FILES)

if(WIN32)
	set(PLATFORM_FILES "win/UTF8CodePage.cpp" "${Arktos_SOURCE_DIR}/inc/win/UTF8CodePage.h")
endif()

add_library(Arktos STATIC Arktos.cpp ${PLATFORM_FILES} ${HEADER_LIST} "Board.cpp"
        BitBoard.cpp)

target_include_directories(Arktos PUBLIC ../inc)

set_property(TARGET Arktos PROPERTY CXX_STANDARD 17)