cmake_minimum_required(VERSION 3.15...4.0)
project(RayForce)

# 0. CONFIGURACIÓN BÁSICA Y COMPILADORES
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
if(POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW) 
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive /W0")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -Wno-error")
endif()

# ---------------------------------------------------------
# 1. DEPENDENCIA: RAYLIB (FetchContent)
# ---------------------------------------------------------
find_package(raylib QUIET)
if(NOT raylib_FOUND)
    include(FetchContent)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG        5.0
    )
    FetchContent_MakeAvailable(raylib)
endif()

# ---------------------------------------------------------
# 2. DEPENDENCIA: PHYSX
# ---------------------------------------------------------
set(PHYSX_LIBRARIES "")
find_package(physx CONFIG QUIET)
if(physx_FOUND)
    set(PHYSX_LIBRARIES physx::PhysX physx::PhysXCommon physx::PhysXFoundation physx::PhysXExtensions)
else()
    if(NOT PHYSX_PATH AND DEFINED ENV{PHYSX_PATH})
        set(PHYSX_PATH "$ENV{PHYSX_PATH}")
    endif()
    if(NOT PHYSX_PATH)
        if(WIN32)
            set(PHYSX_PATH "C:/vcpkg/packages/physx_x64-windows/")
        else()
            set(PHYSX_PATH "/usr/local/physx") 
        endif()
    endif()
  
    set(PHYSX_INCLUDE_DIR "${PHYSX_PATH}/include/physx")
    if(NOT WIN32)
        set(P_SUFFIX "_static_64.a")
        set(P_PREFIX "lib")
        set(PHYSX_LIBRARIES 
            "${PHYSX_PATH}/lib/${P_PREFIX}PhysXExtensions${P_SUFFIX}"
            "${PHYSX_PATH}/lib/${P_PREFIX}PhysXCooking${P_SUFFIX}"
            "${PHYSX_PATH}/lib/${P_PREFIX}PhysX${P_SUFFIX}" 
            "${PHYSX_PATH}/lib/${P_PREFIX}PhysXCommon${P_SUFFIX}"
            "${PHYSX_PATH}/lib/${P_PREFIX}PhysXFoundation${P_SUFFIX}"
            "${PHYSX_PATH}/lib/${P_PREFIX}PhysXPvdSDK${P_SUFFIX}"
        )
    else()
        set(P_CORE_SUFFIX "_64.lib")
        set(P_STATIC_SUFFIX "_static_64.lib")
        set(PHYSX_LIBRARIES 
            "${PHYSX_PATH}/lib/PhysX${P_CORE_SUFFIX}" 
            "${PHYSX_PATH}/lib/PhysXCommon${P_CORE_SUFFIX}"
            "${PHYSX_PATH}/lib/PhysXFoundation${P_CORE_SUFFIX}"
            "${PHYSX_PATH}/lib/PhysXCooking${P_CORE_SUFFIX}"
            "${PHYSX_PATH}/lib/PhysXExtensions${P_STATIC_SUFFIX}" 
            "${PHYSX_PATH}/lib/PhysXPvdSDK${P_STATIC_SUFFIX}"
            "${PHYSX_PATH}/lib/PhysXTask${P_STATIC_SUFFIX}"
        )
    endif()
endif()

# ---------------------------------------------------------
# 3. COMPILAR LA LIBRERÍA CENTRAL (RayForceLib)
# ---------------------------------------------------------
file(GLOB_RECURSE ENGINE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/RayForce/src/*.cpp")
file(GLOB_RECURSE ALL_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/RayForce/src/*.h")

set(MOTOR_DIRS "")
foreach(_header ${ALL_HEADERS})
    get_filename_component(_dir ${_header} DIRECTORY)
    list(APPEND MOTOR_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES MOTOR_DIRS)

# Creamos la librería estática enlazable
add_library(RayForceLib STATIC ${ENGINE_SOURCES})
target_link_libraries(RayForceLib PUBLIC raylib ${PHYSX_LIBRARIES})

target_include_directories(RayForceLib PUBLIC 
    ${MOTOR_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}/RayForce"
    "${CMAKE_CURRENT_SOURCE_DIR}/RayForce/src"
    "${CMAKE_CURRENT_SOURCE_DIR}"             
    "${CMAKE_CURRENT_SOURCE_DIR}/resources"   
)
if(PHYSX_INCLUDE_DIR)
    target_include_directories(RayForceLib PUBLIC "${PHYSX_INCLUDE_DIR}")
endif()

# Guardamos el binario .lib / .a resultante en la carpeta raíz /bin
set_target_properties(RayForceLib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)

# ---------------------------------------------------------
# 4. INCLUSIÓN AUTOMÁTICA DE SUB-CMAKES DE EJEMPLOS
# ---------------------------------------------------------
# Escanea de forma recursiva buscando archivos CMakeLists.txt dentro de la carpeta 'examples'
file(GLOB_RECURSE SUB_CMAKES "${CMAKE_CURRENT_SOURCE_DIR}/examples/*CMakeLists.txt")

foreach(SUB_CMAKE ${SUB_CMAKES})
    get_filename_component(SUB_DIR "${SUB_CMAKE}" DIRECTORY)
    # Evita que se incluya a sí mismo si accidentalmente encuentra un archivo fantasma
    if(NOT "${SUB_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
        # Calculamos la ruta relativa para estructurar correctamente la carpeta build intermedia
        file(RELATIVE_PATH REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}" "${SUB_DIR}")
        add_subdirectory("${SUB_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/${REL_PATH}")
    endif()
endforeach()