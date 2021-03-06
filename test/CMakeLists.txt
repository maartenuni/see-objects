
option(BUILD_UNIT_TESTS
    "Whether or not to build the unit-test requires CUnit"
    ON
    )

if (BUILD_UNIT_TESTS)
    #find_package(${SEE_OBJ_LIB})
    #option(CUnit_DIR CACHE PATH)
    #find_package(CUnit)
    set(CUNIT_HINT_PREFIX "C:/Program Files (x86)/CUnit")
    set(CUNIT_LIB_DIR_HINT "")
    
    # Set the right hint for Win32 vs Win64
    if ("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)") # Win64 build
        set (CUNIT_LIB_DIR_HINT "${CUNIT_HINT_PREFIX}/Lib/Debug-x64")
    else() # win32 build
        set (CUNIT_LIB_DIR_HINT "${CUNIT_HINT_PREFIX}/Lib/Debug-x68")
    endif()
    
    find_library(LIB_CUINIT cunit
        HINTS ${CUNIT_LIB_DIR_HINT}
        )
    #Find the headers on windows
    if (MSVC)
        find_path(HEADER_DIR_CUNIT CUnit/CUnit.h
            HINTS "${CUNIT_HINT_PREFIX}/Include"
            )
    endif()
        

    set(UNIT_TEST unit-test)
    set(UNIT_TEST_SOURCES
        unit_test.c
        dynamic_array_test.c
        error_test.c
        meta_test.c
        msgbuffer_test.c
        random_test.c
        see_object_test.c
        serial_test.c
        stack_test.c
        utilities_tests.c
        time_test.c
        )
    set(UNIT_TEST_HEADERS suites.h test_macros.h)

    add_executable(${UNIT_TEST} ${UNIT_TEST_SOURCES} ${UNIT_TEST_HEADERS})

    set_property(TARGET ${SEE_OBJ_LIB} PROPERTY C_STANDARD 99)
    set_property(TARGET ${UNIT_TEST} PROPERTY C_STANDARD_REQUIRED ON)

    target_link_libraries(${UNIT_TEST} PRIVATE ${LIB_CUINIT})
    target_link_libraries(${UNIT_TEST} PRIVATE ${SEE_OBJ_LIB})
    target_include_directories(${UNIT_TEST} PRIVATE "${CMAKE_BINARY_DIR}/src")
    if (MSVC)
    else()
        target_link_libraries(${UNIT_TEST} PRIVATE m)
    endif()
    
    if (MSVC)
        # Specify the include directory from which the include files are found
        target_include_directories(${UNIT_TEST} PRIVATE ${HEADER_DIR_CUNIT})
        # Set this unit test as debug target
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            PROPERTY
                VS_STARTUP_PROJECT ${UNIT_TEST}
            )
    endif()
    
    set_target_properties(
            ${UNIT_TEST}
        PROPERTIES
            LINKER_LANGUAGE C
        )
    
    # Set the desired output directories
    set_target_properties(
            ${UNIT_TEST}
        PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
            C_STANDARD 11
        )
    # Add compiler warnings
    if (NOT MSVC)
        target_compile_options(${UNIT_TEST}
            PRIVATE -W -Wall -Wextra -pedantic -Werror
            )
    else()
        target_compile_options(
            ${UNIT_TEST} PRIVATE "/W4"
            )
    endif()
endif()

