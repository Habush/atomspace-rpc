enable_testing()
include(AddCxxtest)

add_definitions(-DPROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
        -DPROJECT_BINARY_DIR="${CMAKE_BINARY_DIR}")


IF(CXXTEST_FOUND)
    add_subdirectory(manager)

ENDIF(CXXTEST_FOUND)