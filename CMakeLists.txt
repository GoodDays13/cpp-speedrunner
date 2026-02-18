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

include(FetchContent)
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.0
)
if(SDL3_STATIC)
    set(SDL_STATIC ON)
    set(SDL_SHARED OFF)
else()
    set(SDL_STATIC OFF)
    set(SDL_SHARED ON)
endif()
FetchContent_MakeAvailable(SDL3)

target_link_libraries(speedrunner PRIVATE SDL3::SDL3)

target_compile_definitions(speedrunner PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")

install(TARGETS speedrunner
    DESTINATION .
)
install(DIRECTORY assets/
    DESTINATION assets
)

if (WIN32 AND NOT SDL3_STATIC)
    install(TARGETS
        SDL3-shared
        RUNTIME DESTINATION .
    )
endif()
