add_executable(app app.cpp)
target_compile_features(app PRIVATE cxx_std_17)
target_link_libraries(app PRIVATE Arktos)

# Copy over libary dlls after building
if(WIN32 AND ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	set_target_properties(app PROPERTIES
	VS_DEBUGGER_ENVIRONMENT $<TARGET_FILE_DIR:Arktos>)
endif()