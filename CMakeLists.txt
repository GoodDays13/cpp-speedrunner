cmake_minimum_required(VERSION 3.10)

project(Speedrunner CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(SDL3_STATIC "Static link SDL3" OFF)

set(SHADER_SOURCES
    ${CMAKE_SOURCE_DIR}/assets/shaders/vertex.vert
    ${CMAKE_SOURCE_DIR}/assets/shaders/fragment.frag
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
