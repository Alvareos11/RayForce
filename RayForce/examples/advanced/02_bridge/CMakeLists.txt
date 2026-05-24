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
            "${CMAKE_CURRENT_SOURCE_DIR}/resources"
        COMMENT "Clonando recursos globales en la carpeta local del ejemplo: ${EXE_NAME}"
    )
endif()