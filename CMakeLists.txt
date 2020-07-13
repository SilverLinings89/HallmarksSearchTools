cmake_minimum_required(VERSION 3.0)
project(CancerDataAnalysis)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -o3")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build/bin)
FIND_PACKAGE(Boost)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    ADD_DEFINITIONS( "-DHAS_BOOST" )
ENDIF()

ADD_LIBRARY(Core src/WordHistogram.cpp src/Constants.h src/Hallmark.cpp src/MatrixManager.cpp src/Paper.cpp src/Timer.cpp)

add_executable(Main main/main.cpp)

target_link_libraries(Main
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY} -lboost_system
        Core
)

enable_testing()
set (gtest_force_shared_crt ON CACHE BOOL "MSVC defaults to shared CRT" FORCE)
add_subdirectory(third_party/googletest)
target_compile_definitions(gtest
    PUBLIC
        GTEST_LANG_CXX20
        GTEST_HAS_TR1_TUPLE=0
)

add_executable(tests)
target_sources(tests
    PRIVATE
        unit_tests/PaperTests.cpp
)
set_target_properties(tests PROPERTIES
    COMPILE_FLAGS "${cxx_strict}"
)

target_link_libraries(tests
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY} -lboost_system
        gtest
        gtest_main
        Core
)

add_test(tests tests)