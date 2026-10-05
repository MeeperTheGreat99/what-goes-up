cmake_minimum_required(VERSION 4.2.2)
project(wgu)

file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/source/*.cpp
    ${CMAKE_SOURCE_DIR}/source/*.c
    ${CMAKE_SOURCE_DIR}/lib/msdfgen/include/ext/*.cpp
)

add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)
target_link_options(${PROJECT_NAME} PRIVATE -static-libstdc++)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/SDL/include
    ${CMAKE_SOURCE_DIR}/lib/glm/include
    ${CMAKE_SOURCE_DIR}/lib/glad/include
    ${CMAKE_SOURCE_DIR}/lib/stb/include
    ${CMAKE_SOURCE_DIR}/lib/assimp/include
    ${CMAKE_SOURCE_DIR}/lib/freetype/include
    ${CMAKE_SOURCE_DIR}/lib/msdfgen/include
    ${CMAKE_SOURCE_DIR}/lib/bullet/include
    ${CMAKE_SOURCE_DIR}/lib/openal/include
)
target_link_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/SDL
    ${CMAKE_SOURCE_DIR}/lib/assimp
    ${CMAKE_SOURCE_DIR}/lib/freetype
    ${CMAKE_SOURCE_DIR}/lib/msdfgen
    ${CMAKE_SOURCE_DIR}/lib/bullet
    ${CMAKE_SOURCE_DIR}/lib/openal
)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3 opengl32 assimp freetype msdfgen-core LinearMath BulletCollision BulletDynamics OpenAL32)