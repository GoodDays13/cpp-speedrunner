cmake_minimum_required(VERSION 3.21)

project(Speedrunner CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(SDL3_STATIC "Static link SDL3" OFF)

set(SHADER_SOURCES
    ${CMAKE_SOURCE_DIR}/src/shaders/vertex.vert
    ${CMAKE_SOURCE_DIR}/src/shaders/fragment.frag
    ${CMAKE_SOURCE_DIR}/src/shaders/fullscreen.vert
    ${CMAKE_SOURCE_DIR}/src/shaders/boxblur.frag
    ${CMAKE_SOURCE_DIR}/src/shaders/conway.frag
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
        src/EndScreen.cpp
        src/GameObject.cpp
        src/Player.cpp
        src/LevelLoader.cpp
        src/JsonReader.cpp
        src/Text.cpp
)

add_dependencies(speedrunner embed_shaders)

target_include_directories(speedrunner PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_include_directories(speedrunner PRIVATE "${CMAKE_BINARY_DIR}/generated")

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

if(SDL3_STATIC)
    target_link_libraries(speedrunner SDL3_image::SDL3_image-static SDL3::SDL3-static)
else()
    target_link_libraries(speedrunner PRIVATE SDL3_image::SDL3_image SDL3::SDL3)
endif()

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install") 

install(TARGETS speedrunner
    DESTINATION .
)
install(DIRECTORY assets/
    DESTINATION assets
)

if (WIN32)
    install(IMPORTED_RUNTIME_ARTIFACTS 
        SDL3::SDL3-shared
        SDL3_image::SDL3_image-shared
    )
endif()
