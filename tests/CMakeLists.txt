enable_testing()
include(AddCxxtest)

add_definitions(-DPROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
        -DPROJECT_BINARY_DIR="${CMAKE_BINARY_DIR}")
include_directories("${CMAKE_BINARY_DIR}/src")
link_libraries(atomspace atom_service_client)
IF(CXXTEST_FOUND)
    add_subdirectory(manager)
    ADD_CXXTEST(AtomServiceUtilsUTest)

ENDIF(CXXTEST_FOUND)