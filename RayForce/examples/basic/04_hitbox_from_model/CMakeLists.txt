# Automatización: Toma el nombre de la carpeta física donde está guardado (ej: 00_basic_window)
get_filename_component(EXE_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

# Busca todos los archivos .cpp locales de esta carpeta específica
file(GLOB EXE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

if(EXE_SOURCES)
    # 1. Definir el ejecutable del ejemplo
    add_executable(${EXE_NAME} ${EXE_SOURCES})

    # 2. PETICIÓN: Forzar que el archivo compilado se guarde DENTRO de esta misma carpeta
    set_target_properties(${EXE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )

    # Librerías necesarias de sistema según la plataforma
    set(SYSTEM_LIBS "")
    if(NOT MSVC)
        set(SYSTEM_LIBS m pthread dl rt GL X11)
    endif()

    # 3. PETICIÓN: Vincular con tu librería central
    # Al enlazar con RayForceLib, CMake genera una dependencia directa. Si el código del motor
    # cambia, compilar este ejemplo obligará a CMake a compilar la librería primero.
    target_link_libraries(${EXE_NAME} PRIVATE RayForceLib ${SYSTEM_LIBS})

    # 4. PETICIÓN: Clonar los recursos globales en la carpeta local de este ejemplo
    add_custom_command(TARGET ${EXE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/resources" 
            "$<TARGET_FILE_DIR:${EXE_NAME}>/resources"
        COMMENT "Clonando recursos globales en la carpeta local del ejemplo: ${EXE_NAME}"
    )

    # 5. Copiar DLLs necesarios (raylib, PhysX, etc.) al directorio del ejecutable
    if(WIN32)
        set(PHYSX_DLLS "")

        # Copiar raylib DLL si existe
        get_target_property(RAYLIB_LOC raylib IMPORTED_LOCATION_RELEASE)
        if(NOT RAYLIB_LOC)
            get_target_property(RAYLIB_LOC raylib IMPORTED_LOCATION)
        endif()
        if(RAYLIB_LOC)
            get_filename_component(RAYLIB_DIR "${RAYLIB_LOC}" DIRECTORY)
            file(GLOB RAYLIB_DLLS "${RAYLIB_DIR}/*.dll")
            foreach(DLL ${RAYLIB_DLLS})
                add_custom_command(TARGET ${EXE_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${DLL}"
                        "$<TARGET_FILE_DIR:${EXE_NAME}>/"
                    COMMENT "Copiando DLL: ${DLL}"
                )
            endforeach()
        endif()

        # Copiar PhysX DLLs si están disponibles
        if(PHYSX_PATH)
            set(PHYSX_DLL_DIR "${PHYSX_PATH}/bin")
            if(EXISTS "${PHYSX_DLL_DIR}")
                file(GLOB PHYSX_DLLS "${PHYSX_DLL_DIR}/*.dll")
            endif()
        endif()

        if(NOT PHYSX_DLLS)
            find_package(physx CONFIG QUIET)
            if(physx_FOUND)
                get_target_property(PHYSX_LOC physx::PhysX IMPORTED_LOCATION_RELEASE)
                if(NOT PHYSX_LOC)
                    get_target_property(PHYSX_LOC physx::PhysX IMPORTED_LOCATION)
                endif()
                if(PHYSX_LOC)
                    get_filename_component(PHYSX_DIR "${PHYSX_LOC}" DIRECTORY)
                    set(PHYSX_DLL_DIR "${PHYSX_DIR}")
                    if(EXISTS "${PHYSX_DIR}/../bin")
                        set(PHYSX_DLL_DIR "${PHYSX_DIR}/../bin")
                    elseif(EXISTS "${PHYSX_DIR}/bin")
                        set(PHYSX_DLL_DIR "${PHYSX_DIR}/bin")
                    endif()
                    if(EXISTS "${PHYSX_DLL_DIR}")
                        file(GLOB PHYSX_DLLS "${PHYSX_DLL_DIR}/*.dll")
                    endif()
                endif()
            endif()
        endif()

        if(NOT PHYSX_DLLS AND WIN32 AND EXISTS "C:/vcpkg/packages/physx_x64-windows/bin")
            file(GLOB PHYSX_DLLS "C:/vcpkg/packages/physx_x64-windows/bin/*.dll")
        endif()

        foreach(DLL ${PHYSX_DLLS})
            add_custom_command(TARGET ${EXE_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${DLL}"
                    "$<TARGET_FILE_DIR:${EXE_NAME}>/"
                COMMENT "Copiando PhysX DLL: ${DLL}"
            )
        endforeach()
    endif()
endif()