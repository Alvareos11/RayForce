cmake_minimum_required(VERSION 3.15...4.0)
project(RayForce)

# 0. FIX PARA RAYLIB Y POLÍTICAS ANTIGUAS
# Esto soluciona el error: "Compatibility with CMake < 3.5 has been removed"
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
if(POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW) # Manejo de versiones en project()
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Flags de compilación
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive /W0")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -Wno-error")
endif()

# ---------------------------------------------------------
# 1. AUTO-INSTALACIÓN DE RAYLIB (FetchContent)
# ---------------------------------------------------------
find_package(raylib QUIET)

if(raylib_FOUND)
    target_link_libraries(physics_demo PRIVATE raylib)
else()
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
# 2. CONFIGURACIÓN DE PHYSX (OMNIVERSE 5.5 / VCPKG / MANUAL)
# ---------------------------------------------------------

# Fallback para PhysX estándar o manual
find_package(physx CONFIG QUIET)
if(physx_FOUND)
    message(STATUS "PhysX Estándar detectado.")
    target_link_libraries(RayForceLib PRIVATE 
        physx::PhysX 
        physx::PhysXCommon 
        physx::PhysXFoundation 
        physx::PhysXExtensions
        $<$<PLATFORM_ID:Linux>:pthread m dl>
    )
else()
    message(STATUS "Buscando PhysX de forma manual...")
    if(NOT PHYSX_PATH AND DEFINED ENV{PHYSX_PATH})
        set(PHYSX_PATH "$ENV{PHYSX_PATH}")
    endif()
    if(NOT PHYSX_PATH)
        if(WIN32)
            set(PHYSX_PATH "C:/vcpkg/packages/physx_x64-windows")
        else()
            set(PHYSX_PATH "/usr/local/physx") # Ruta genérica para Linux/Mac
        endif()
    endif()
    
    set(PHYSX_INCLUDE_DIR "${PHYSX_PATH}/include/physx")

    if(NOT WIN32)
        # Configuración para Linux (WSL)
        set(P_SUFFIX "_static_64.a")
        set(P_PREFIX "lib")

        # ORDEN CRÍTICO: De más dependiente a menos dependiente
        set(PHYSX_LIBRARIES 
            "${PHYSX_PATH}/lib/${P_PREFIX}PhysXExtensions${P_SUFFIX}"
            "${PHYSX_PATH}/lib/${P_PREFIX}PhysXCooking${P_SUFFIX}"
            "${PHYSX_PATH}/lib/${P_PREFIX}PhysX${P_SUFFIX}" 
            "${PHYSX_PATH}/lib/${P_PREFIX}PhysXCommon${P_SUFFIX}"
            "${PHYSX_PATH}/lib/${P_PREFIX}PhysXFoundation${P_SUFFIX}"
            "${PHYSX_PATH}/lib/${P_PREFIX}PhysXPvdSDK${P_SUFFIX}"
            # No incluimos PhysXTask porque ya está dentro de Foundation/Common en Linux
        )
    else()
        # Configuración para Windows
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
# 3. RECOLECCIÓN DE FUENTES
# ---------------------------------------------------------
file(GLOB_RECURSE ENGINE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/RayForce/src/*.cpp")

# Obtener directorios para los includes de forma recursiva (Brute Force)
file(GLOB_RECURSE ALL_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/RayForce/src/*.h")
set(MOTOR_DIRS "")
foreach(_header ${ALL_HEADERS})
    get_filename_component(_dir ${_header} DIRECTORY)
    list(APPEND MOTOR_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES MOTOR_DIRS)

# ---------------------------------------------------------
# 4. LIBRERÍA CORE
# ---------------------------------------------------------
add_library(RayForceLib STATIC ${ENGINE_SOURCES})
target_link_libraries(RayForceLib PUBLIC raylib ${PHYSX_LIBRARIES})

target_include_directories(RayForceLib PUBLIC 
    ${MOTOR_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}/RayForce"
    "${CMAKE_CURRENT_SOURCE_DIR}/RayForce/src"
    "${CMAKE_CURRENT_SOURCE_DIR}"             # Para #include "resources/..."
    "${CMAKE_CURRENT_SOURCE_DIR}/resources"   # Para #include "materials.list"
    "${PHYSX_INCLUDE_DIR}"                     # Para includes de PhysX
)

# ---------------------------------------------------------
# 5. EJEMPLOS
# ---------------------------------------------------------
file(GLOB EXAMPLE_DIRS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/examples" "examples/*")

set(RESOURCE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/resources")
set(RESOURCE_DST "$<TARGET_FILE_DIR:physics_demo>/resources")

foreach(DIR_NAME ${EXAMPLE_DIRS})
    set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/examples/${DIR_NAME}")
    if(IS_DIRECTORY ${DIR})
        string(REPLACE " " "_" SAFE_NAME ${DIR_NAME})
        file(GLOB_RECURSE EXAMPLE_SOURCES "${DIR}/*.cpp")
        
        if(EXAMPLE_SOURCES)
            add_executable(${SAFE_NAME} ${EXAMPLE_SOURCES})
            
            set(SYSTEM_LIBS "")
            if(NOT MSVC)
                set(SYSTEM_LIBS m pthread dl rt GL X11)
            endif()
    
            target_link_libraries(${SAFE_NAME} PRIVATE RayForceLib ${SYSTEM_LIBS})
        endif()
    endif()
endforeach()

add_custom_command(TARGET physics_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${RESOURCE_SRC}"
        "${RESOURCE_DST}"
    COMMENT "Copiando recursos a la carpeta de salida..."
)