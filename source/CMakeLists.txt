cmake_minimum_required(VERSION 3.1.0)

set(CMAKE_CXX_STANDARD 14)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)
find_package(OpenGL REQUIRED)

set(HEADERS
    globals.h
    SGFrame.h
    SGFixedGLState.h
    SGModels.h
    SGOglNotebook.h
    SGSurfaces.h
    SGShaderGenerator.h
    SGShaderTextWindow.h
    SGCanvasMouseHandler.h
    SGCanvas.h
    SGTextures.h
    SGOglFogNBPage.h
    SGOglMaterialNBPage.h
    SGOglLightNBPage.h
    SGOglTextureCoordNBPage.h
    SGOglTextureEnvNBPage.h
    QColorButton.h
    QVectorEdit.h
    QCodeEditor.h
)

set(SOURCES
    SGFrame.cpp
    SGFixedGLState.cpp
    SGModels.cpp
    SGOglNotebook.cpp
    SGSurfaces.cpp
    SGShaderGenerator.cpp
    SGCanvas.cpp
    SGCanvasMouseHandler.cpp
    SGShaderTextWindow.cpp
    SGOglFogNBPage.cpp
    SGOglMaterialNBPage.cpp
    SGTextures.cpp
    SGOglLightNBPage.cpp
    SGOglTextureCoordNBPage.cpp
    SGOglTextureEnvNBPage.cpp
    QColorButton.cpp
    QVectorEdit.cpp
    QCodeEditor.cpp
    main.cpp
)

set(RESOURCES
    textures.qrc
)

if (APPLE)
    set(MACOSX_BUNDLE_ICON_FILE "ShaderGen.icns")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION})
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.ShaderGen")
    set(RESOURCES ${RESOURCES} ShaderGen.icns)
    set_source_files_properties(ShaderGen.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
elseif (WIN32)
    set(RESOURCES ${RESOURCES} info.rc)
endif ()

add_executable (ShaderGen WIN32 MACOSX_BUNDLE ${HEADERS} ${SOURCES} ${RESOURCES})

target_link_libraries(ShaderGen
    Qt5::Widgets
    Qt5::OpenGL
    ${OPENGL_LIBRARIES}
)

install(TARGETS ShaderGen DESTINATION bin)
