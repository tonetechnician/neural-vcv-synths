cmake_minimum_required(VERSION 3.15...3.21)

execute_process(
    COMMAND bash "-c" "jq -r .slug ${CMAKE_SOURCE_DIR}/plugin.json" 
    OUTPUT_VARIABLE SLUG
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND bash "-c" "jq -r .version ${CMAKE_SOURCE_DIR}/plugin.json" 
    OUTPUT_VARIABLE VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
project(${SLUG})

message(STATUS "RACK_DIR: $ENV{RACK_DIR}/include")

SET(LIBTORCH_DIR "" CACHE STRING "Path to libtorch")
set (CMAKE_CXX_STANDARD 14)
message(STATUS "LIBTORCH DIR: ${LIBTORCH_DIR}")
include(${LIBTORCH_DIR}/share/cmake/Torch/TorchConfig.cmake)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

set(DIST_PATH "${PROJECT_SOURCE_DIR}/cmake-dist/${SLUG}")

message(STATUS "SLUG ${SLUG}")
message(STATUS "VERSION: ${VERSION}")
message(STATUS "OUTPUT PATH: ${DIST_PATH}")

# set(CMAKE_MACOSX_RPATH 1)
# set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH /tmp/Rack2 )

include_directories($ENV{RACK_DIR}/dep/include)
include_directories($ENV{RACK_DIR}/include)
link_directories($ENV{RACK_DIR})

add_subdirectory(src)

file(GLOB SOURCES
    src/*.h
    src/*.cpp
    src/*.c
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${DIST_PATH}")
add_library(plugin SHARED ${SOURCES})

set_target_properties(plugin PROPERTIES PREFIX "")
set_property(TARGET plugin PROPERTY CXX_STANDARD 14)

target_link_libraries(plugin Rack "${TORCH_LIBRARIES}")

message(STATUS $<TARGET_OBJECTS:plugin>)

if(UNIX)
    set(CMAKE_BUILD_RPATH "/tmp/Rack2/")
    set_target_properties(plugin PROPERTIES COMPILE_FLAGS "-fno-gnu-unique")
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-rpath=/tmp/Rack2 -static-libstdc++ -static-libgcc")
endif()

if (APPLE)
add_custom_command(TARGET plugin POST_BUILD COMMAND
                        install_name_tool -change libRack.dylib /tmp/Rack2/libRack.dylib $<TARGET_FILE:plugin>
                        DEPENDS $<TARGET_FILE:plugin>
                    )

add_custom_command(TARGET plugin POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${PROJECT_SOURCE_DIR}/res ${DIST_PATH}/res
                        DEPENDS $<TARGET_FILE:plugin>    
                    )

add_custom_command(TARGET plugin POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                        ${PROJECT_SOURCE_DIR}/plugin.json ${DIST_PATH}/plugin.json
                        DEPENDS $<TARGET_FILE:plugin>
                    )

add_custom_command(TARGET plugin POST_BUILD COMMAND
                    rm -f ${PROJECT_SOURCE_DIR}/cmake-dist/${SLUG}-${VERSION}-mac.vcvplugin
                        DEPENDS $<TARGET_FILE:plugin>
                    )
                    
add_custom_command(TARGET plugin POST_BUILD COMMAND
                    cd ${PROJECT_SOURCE_DIR}/cmake-dist && tar -c ${SLUG} | zstd -19 -o ${SLUG}-${VERSION}-mac.vcvplugin
                        DEPENDS $<TARGET_FILE:plugin>
                    )

add_custom_command(TARGET plugin POST_BUILD COMMAND
                        cp ${PROJECT_SOURCE_DIR}/cmake-dist/*.vcvplugin ~/Documents/Rack2/plugins/
                    )
endif()

if(UNIX)
add_custom_command(TARGET plugin POST_BUILD COMMAND
                        echo "Handling post build linux"
                    )

add_custom_command(TARGET plugin POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${PROJECT_SOURCE_DIR}/res ${DIST_PATH}/res
                        DEPENDS $<TARGET_FILE:plugin>    
                    )
add_custom_command(TARGET plugin POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${PROJECT_SOURCE_DIR}/res ${DIST_PATH}/res
                        DEPENDS $<TARGET_FILE:plugin>    
                    )

add_custom_command(TARGET plugin POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                        ${PROJECT_SOURCE_DIR}/plugin.json ${DIST_PATH}/plugin.json
                        DEPENDS $<TARGET_FILE:plugin>
                    )

add_custom_command(TARGET plugin POST_BUILD COMMAND
                    rm -f ${PROJECT_SOURCE_DIR}/cmake-dist/${SLUG}-${VERSION}-mac.vcvplugin
                        DEPENDS $<TARGET_FILE:plugin>
                    )
                    
add_custom_command(TARGET plugin POST_BUILD COMMAND
                    cd ${PROJECT_SOURCE_DIR}/cmake-dist && tar -c ${SLUG} | zstd -19 -o ${SLUG}-${VERSION}-lin.vcvplugin
                        DEPENDS $<TARGET_FILE:plugin>
                    )

add_custom_command(TARGET plugin POST_BUILD COMMAND
                    cp ${PROJECT_SOURCE_DIR}/cmake-dist/*.vcvplugin ~/.Rack2/plugins/
                    )
endif()