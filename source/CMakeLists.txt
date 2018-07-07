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

add_executable (ShaderGen ${HEADERS} ${SOURCES} ${RESOURCES})

target_link_libraries(ShaderGen
    Qt5::Widgets
    Qt5::OpenGL
    ${OPENGL_LIBRARIES}
)

# Install
if (APPLE)
    set_target_properties(ShaderGen PROPERTIES MACOSX_BUNDLE TRUE)
    install(TARGETS ShaderGen BUNDLE DESTINATION ./)
elseif (WIN32)
    set_target_properties(ShaderGen PROPERTIES WIN32 TRUE)
    set_target_properties(ShaderGen PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
    install(TARGETS ShaderGen RUNTIME DESTINATION ./)
else()
    # Assume linux
    install(TARGETS ShaderGen DESTINATION bin)
endif ()

# Automatically deploy qt
get_target_property(QT5_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
get_filename_component(QT5_BIN_DIR ${QT5_QMAKE_EXECUTABLE} PATH)
if (APPLE)
    find_program(QT5_DEPLOYQT_EXECUTABLE macdeployqt HINTS "${QT5_BIN_DIR}")
    add_custom_command(TARGET ShaderGen POST_BUILD
           COMMAND ${QT5_DEPLOYQT_EXECUTABLE} $<TARGET_BUNDLE_DIR:ShaderGen>)
elseif (WIN32)
    find_program(QT5_DEPLOYQT_EXECUTABLE windeployqt HINTS "${QT5_BIN_DIR}")
    add_custom_command(TARGET ShaderGen POST_BUILD
           COMMAND ${QT5_DEPLOYQT_EXECUTABLE} $<TARGET_FILE:ShaderGen>)
endif ()



# Package builder
set(CPACK_PACKAGE_NAME "ShaderGenSetup")
set(CPACK_PACKAGE_VENDOR "mojocorp")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "GLSL ShaderGen")
set(CPACK_PACKAGE_VERSION ${SHADERGEN_VERSION})
set(CPACK_COMPONENTS_GROUPING "ALL_COMPONENTS_IN_ONE")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/License.txt")

include(CPack)
