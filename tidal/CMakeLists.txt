cmake_minimum_required(VERSION 3.23)

project(tidal LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

set(BUILD_SHARED_LIBS OFF)
set(OPENSSL_USE_STATIC_LIBS TRUE)
set(ZLIB_USE_STATIC_LIBS TRUE)

find_package(Lua REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

add_subdirectory(extern/argh SYSTEM)
add_subdirectory(extern/spdlog SYSTEM)
add_subdirectory(extern/Crow SYSTEM)
add_subdirectory(extern/sol2 SYSTEM)
add_subdirectory(extern/rapidcsv SYSTEM)
add_subdirectory(extern/EzPC/SCI SYSTEM)

# --- FIX PATCH BEGIN ---
# CMakeLists.txt of SCI-utils is not standard. need repair to ensure CXX 20
if(TARGET SCI-utils)
    get_target_property(SCI_OPTS SCI-utils INTERFACE_COMPILE_OPTIONS)
    if(SCI_OPTS)
        string(REPLACE "-std=c++17" "-std=c++20" UPDATED_OPTS "${SCI_OPTS}")
        set_target_properties(SCI-utils PROPERTIES INTERFACE_COMPILE_OPTIONS "${UPDATED_OPTS}")
    endif()
endif()
# --- FIX PATCH END ---

file(GLOB ENGINE_FILES "src/engine/*.cpp" "src/engine/*.hpp")
file(GLOB SERVICE_FILES "src/service/*.cpp" "src/service/*.hpp")
file(GLOB UTILS_FILES "src/utils/*.cpp" "src/utils/*.hpp")

add_executable(
    ${PROJECT_NAME} 
        src/main.cpp 
        src/main.hpp
        ${ENGINE_FILES}
        ${SERVICE_FILES}
        ${UTILS_FILES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${LUA_INCLUDE_DIR}"
    "${CMAKE_SOURCE_DIR}/src"
)

target_link_libraries(
    ${PROJECT_NAME}
        # system install
        ${LUA_LIBRARIES}
        OpenSSL::SSL
        OpenSSL::Crypto
        ZLIB::ZLIB
        
        # local install
        argh
        rapidcsv
        spdlog::spdlog
        sol2::sol2
        Crow::Crow
        SCI-FloatingPoint
)

target_link_options(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++)
