cmake_minimum_required(VERSION 3.10)

project(Speedrunner CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(SDL3_STATIC "Static link SDL3" OFF)

set(SHADER_SOURCES
    ${CMAKE_SOURCE_DIR}/assets/shaders/vertex.vert
    ${CMAKE_SOURCE_DIR}/assets/shaders/fragment.frag
    ${CMAKE_SOURCE_DIR}/assets/shaders/fullscreen.vert
    ${CMAKE_SOURCE_DIR}/assets/shaders/boxblur.frag
    ${CMAKE_SOURCE_DIR}/assets/shaders/conway.frag
)

set(GENERATED_HEADERS "")
foreach(shader ${SHADER_SOURCES})
    get_filename_component(fname ${shader} NAME) # e.g. vert
    set(spv ${CMAKE_BINARY_DIR}/generated/${fname}.spv)
    set(out ${CMAKE_BINARY_DIR}/generated/${fname}.h)

    add_custom_command(
        OUTPUT ${spv}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/generated
        COMMAND glslc ${shader} -o ${spv}
        DEPENDS ${shader}
        COMMENT "Compiling ${shader} → ${spv}"
        VERBATIM
    )

    add_custom_command(
        OUTPUT ${out}
        COMMAND xxd -i -n ${fname}_spv ${spv} > ${out}
        DEPENDS ${spv}
        COMMENT "Embedding ${spv} → ${out}"
        VERBATIM
    )

    list(APPEND GENERATED_HEADERS ${out})
endforeach()

add_custom_target(embed_shaders ALL DEPENDS ${GENERATED_HEADERS})

add_executable(speedrunner
        src/main.cpp
        src/Game.cpp
        src/Video.cpp
        src/TitleScreen.cpp
        src/PlatformerScene.cpp
        src/GameObject.cpp
        src/Player.cpp
        src/LevelLoader.cpp
        src/JsonReader.cpp
)

add_dependencies(speedrunner embed_shaders)

target_include_directories(speedrunner PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_include_directories(speedrunner PRIVATE "${CMAKE_BINARY_DIR}/generated")

find_package(SDL3 REQUIRED)

if(SDL3_STATIC)
	target_link_libraries(speedrunner ${SDL3_STATIC_LIBRARIES})
else()
	target_link_libraries(speedrunner ${SDL3_LIBRARIES})
endif()

set(DIST_DIR ${CMAKE_BINARY_DIR}/out)
set(ASSET_DIRS
    assets/levels
)

file(MAKE_DIRECTORY ${DIST_DIR})

add_custom_command(
    OUTPUT ${DIST_DIR}/speedrunner${CMAKE_EXECUTABLE_SUFFIX}
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:speedrunner> ${DIST_DIR}/speedrunner${CMAKE_EXECUTABLE_SUFFIX}
    COMMENT "Copying executable to ${DIST_DIR}"
    DEPENDS $<TARGET_FILE:speedrunner>
    VERBATIM
)

foreach (dir ${ASSET_DIRS})
    add_custom_command(
        OUTPUT ${DIST_DIR}/${dir}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${DIST_DIR}/${dir}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/${dir} ${DIST_DIR}/${dir}
        COMMENT "Copying ${dir} to ${DIST_DIR}/${dir}"
        DEPENDS ${CMAKE_SOURCE_DIR}/${dir}
        VERBATIM
    )
    list(APPEND ASSET_OUTPUTS ${DIST_DIR}/${dir})
endforeach()

add_custom_target(dist ALL DEPENDS ${DIST_DIR}/speedrunner${CMAKE_EXECUTABLE_SUFFIX} ${ASSET_OUTPUTS})
