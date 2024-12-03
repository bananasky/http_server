Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.4.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

set(TEST_FILES
        ResponseTest.cpp
        RequestTest.cpp
)

add_executable(unit_tests ${TEST_FILES})
target_link_libraries(unit_tests PRIVATE http_server_core Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(unit_tests)