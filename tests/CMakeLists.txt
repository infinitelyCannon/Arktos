add_executable(MoveStateTest MoveStateTest.cpp)
target_compile_features(MoveStateTest PRIVATE cxx_std_20)
target_link_libraries(MoveStateTest PRIVATE Arktos)

set(MOVESTATE_NUM_OF_GAMES 1 CACHE STRING "MoveState: Number of game files to test against.")
set(MOVESTATE_GAME_DIR "" CACHE STRING "Directory of game files to read.")

add_test(NAME MoveStateTest COMMAND MoveStateTest ${MOVESTATE_NUM_OF_GAMES} "${MOVESTATE_GAME_DIR}")