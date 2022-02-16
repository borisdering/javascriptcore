cmake_minimum_required(VERSION 3.16)

project(jsc)

function(dump_cmake_variables)
    get_cmake_property(_variableNames VARIABLES)
    list (SORT _variableNames)
    foreach (_variableName ${_variableNames})
        if (ARGV0)
            unset(MATCHED)
            string(REGEX MATCH ${ARGV0} MATCHED ${_variableName})
            if (NOT MATCHED)
                continue()
            endif()
        endif()
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endfunction()

# include libjavascript core library by path 
# it seams to be a header only library,.. so,...
include_directories(/usr/include/webkitgtk-4.0/ include)

# copy test file to build folder
configure_file(test.js test.js COPYONLY)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB REQUIRED glib-2.0)
# pkg_check_modules(${CMAKE_PROJECT_NAME} IMPORTED_TARGET REQUIRED glib-2.0)

# include_directories(${GLIB_INCLUDE_DIRS})
# link_directories(${GLIB_LIBRARY_DIRS})

# add_definitions(${GLIB_CFLAGS_OTHER})

# create an executable target
add_executable(${CMAKE_PROJECT_NAME} src/main.cpp)

dump_cmake_variables(GLIB)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${GLIB_INCLUDE_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC glib-2.0 gobject-2.0 "/usr/lib/x86_64-linux-gnu/libjavascriptcoregtk-4.0.so.18")

# link actual shared library
# target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${GLIB_INCLUDE_DIRS})
# target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ${GLIB_LIBRARIES} PUBLIC "/usr/lib/x86_64-linux-gnu/libjavascriptcoregtk-4.0.so.18")
# target_link_libraries(${CMAKE_PROJECT_NAME} INTERFACE ${GLIB_LIBRARIES})

# target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC "/usr/lib/x86_64-linux-gnu/libjavascriptcoregtk-4.0.so.18")
# target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ${GLIB_LIBRARIES})


# # include glib for the libjavascriptcore library 
# find_package(PkgConfig REQUIRED)
# pkg_search_module(GLIB REQUIRED glib-2.0-dev)

# pkg_check_modules(COGCORE IMPORTED_TARGET REQUIRED cogcore glib-2.0)

# target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${GLIB_INCLUDE_DIRS})
# # target_link_libraries(${CMAKE_PROJECT_NAME} PkgConfig::GLIB)
# target_link_libraries(${CMAKE_PROJECT_NAME} INTERFACE ${GLIB_LIBRARIES})