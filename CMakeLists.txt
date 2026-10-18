cmake_minimum_required(VERSION 3.20)

# ----------------------------------------------------------------------
# 1. ตรวจสอบและตั้งค่า vcpkg toolchain
# ----------------------------------------------------------------------
set(_detect_vcpkg_toolchain FALSE)
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE OR CMAKE_TOOLCHAIN_FILE STREQUAL "")
  set(_detect_vcpkg_toolchain TRUE)
elseif(NOT EXISTS "${CMAKE_TOOLCHAIN_FILE}")
  message(STATUS "Cached/explicit toolchain file does not exist: ${CMAKE_TOOLCHAIN_FILE}")
  set(_detect_vcpkg_toolchain TRUE)
endif()

if(_detect_vcpkg_toolchain)
  set(_vcpkg_toolchain_candidates)

  if(DEFINED ENV{VCPKG_ROOT} AND NOT "$ENV{VCPKG_ROOT}" STREQUAL "")
    file(TO_CMAKE_PATH
      "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      _vcpkg_from_env
    )
    list(APPEND _vcpkg_toolchain_candidates "${_vcpkg_from_env}")
  endif()

  if(CMAKE_HOST_WIN32 AND DEFINED ENV{USERPROFILE})
    file(TO_CMAKE_PATH
      "$ENV{USERPROFILE}/vcpkg/scripts/buildsystems/vcpkg.cmake"
      _vcpkg_from_home
    )
    list(APPEND _vcpkg_toolchain_candidates "${_vcpkg_from_home}")
  elseif(DEFINED ENV{HOME})
    file(TO_CMAKE_PATH
      "$ENV{HOME}/vcpkg/scripts/buildsystems/vcpkg.cmake"
      _vcpkg_from_home
    )
    list(APPEND _vcpkg_toolchain_candidates "${_vcpkg_from_home}")
  endif()

  unset(_found_vcpkg_toolchain)
  foreach(_candidate IN LISTS _vcpkg_toolchain_candidates)
    if(EXISTS "${_candidate}")
      set(_found_vcpkg_toolchain "${_candidate}")
      break()
    endif()
  endforeach()

  if(_found_vcpkg_toolchain)
    set(CMAKE_TOOLCHAIN_FILE "${_found_vcpkg_toolchain}"
        CACHE FILEPATH "vcpkg toolchain file" FORCE)
    message(STATUS "Using vcpkg toolchain: ${CMAKE_TOOLCHAIN_FILE}")
  else()
    message(FATAL_ERROR
      "Could not find the vcpkg toolchain file.\n"
      "Install vcpkg in the recommended location, or set VCPKG_ROOT to your vcpkg folder."
    )
  endif()
endif()

# ----------------------------------------------------------------------
# 2. ตั้งค่าโปรเจกต์
# ----------------------------------------------------------------------
project(OpenGLStarter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ดึง stb_image.h จาก GitHub อัตโนมัติ (วิธีที่ 2)
include(FetchContent)
FetchContent_Declare(
  stb
  URL https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
  DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(stb)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glew CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# สร้าง Header สำหรับ Path ของ Shader
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated")
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ProjectPaths.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/generated/ProjectPaths.h
  @ONLY
)

# รวมไฟล์ Source Code (ไม่ใส่ stb_image.cpp เพราะไม่มีไฟล์จริง)
add_executable(OpenGLStarter
  src/Assignment2_67050019.cpp
  src/Libs/Mesh.cpp
  src/Libs/Shader.cpp
  src/Libs/Window.cpp
)

add_executable(Assignment2_67050019
  src/Assignment2_67050019.cpp
  src/Libs/Mesh.cpp
  src/Libs/Shader.cpp
  src/Libs/Window.cpp
)

add_executable(generate_office
  tools/generate_office.cpp
)


target_include_directories(OpenGLStarter PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Libs
  ${CMAKE_CURRENT_BINARY_DIR}/generated
  ${stb_SOURCE_DIR}
)

target_include_directories(Assignment2_67050019 PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Libs
  ${CMAKE_CURRENT_BINARY_DIR}/generated
  ${stb_SOURCE_DIR}
)

target_link_libraries(OpenGLStarter PRIVATE
  OpenGL::GL
  glfw
  GLEW::GLEW
  glm::glm
)

target_link_libraries(Assignment2_67050019 PRIVATE
  OpenGL::GL
  glfw
  GLEW::GLEW
  glm::glm
)

if(APPLE)
  target_compile_definitions(OpenGLStarter PRIVATE GL_SILENCE_DEPRECATION)
  target_compile_definitions(Assignment2_67050019 PRIVATE GL_SILENCE_DEPRECATION)
endif()

# คัดลอกโฟลเดอร์ Assets ไปยัง Build Output
add_custom_target(copy_models ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Models
    $<TARGET_FILE_DIR:OpenGLStarter>/Models)
add_dependencies(OpenGLStarter copy_models)

add_custom_target(copy_textures ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Textures
    $<TARGET_FILE_DIR:OpenGLStarter>/Textures)
add_dependencies(OpenGLStarter copy_textures)
add_dependencies(Assignment2_67050019 copy_models copy_textures)