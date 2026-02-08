cmake_minimum_required(VERSION 3.17)
project(XPL_WorkBench)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_definitions(-DGL_SILENCE_DEPRECATION)




# Linux
IF(CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(CMAKE_CXX_FLAGS "-O0 -rdynamic -ldl")

ENDIF(CMAKE_SYSTEM_NAME STREQUAL Linux)



set( FXPLM_INC "../FXPLM/XPLM" )

set( IMGUI_ROOT "third_party/imgui-docking"
        )

set( INC_IMGUI
        ${IMGUI_ROOT}
        ${IMGUI_ROOT}/backends/
        third_party/imgui-filebrowser-master

        )


set( INC_STBTT third_party/stb_truetype )

set( INC_GLFW "../glfw/include" )
set( INC_JSON third_party/json-develop/include)

INCLUDE_DIRECTORIES(
        #/opt/homebrew/Cellar/glfw/3.3.8/include
        ${INC_GLFW}
        ${OPENGL_INCLUDE_DIRS}
        ${GLUT_INCLUDE_DIRS}

        ${INC_IMGUI}
        ${INC_JSON}

        #../gz_core/gz_api

        third_party/glew-2.2.0/include
        third_party/stb_image

        ${INC_STBTT}
        ${FXPLM_INC}

)


#we're not ready for this.
#trying to link with gz_core/gz_api lib results in weird
#missing symbols from the host app. solve another day.
set( INC_GZ_CORE ../gz_core/gz_api )
include_directories( ${INC_GZ_CORE} )

set( LNK_FXPLM_FRAMEWORK "../../FXPLM/cmake-build-debug/" )

set( LNK_GLFW "../glfw/build_mac/src" )
set( LNK_GLEW "third_party/glew-2.2.0/lib" )

LINK_DIRECTORIES(
        #/opt/homebrew/Cellar/glfw/3.3.8/lib
        ${LNK_GLFW}
        ${LNK_GLEW}
)


add_definitions( -DGZ_USE_XP_SDK=0 )

IF( APPLE )
    add_definitions( -DAPL=1 )

    set(CMAKE_OSX_DEPLOYMENT_TARGET "14.0")

    find_library(SECURITY_FRAMEWORK Security)
    find_library(SYSTEMCONFIGURATION_FRAMEWORK SystemConfiguration)
    find_library(IOKIT_FRAMEWORK IOKit)
    find_library(COREFOUNDATION_LIBRARY CoreFoundation)
    find_library(COREGRAPHICS_LIBRARY CoreGraphics)
    find_library(APPKIT_LIBRARY AppKit)
    find_library(QUARTZCORE_LIBRARY QuartzCore)

    set( FXPLM_DYLIB XPLM )

    SET( LIBS_MAC
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARY}
        glfw3

        ${SECURITY_FRAMEWORK}
        ${SYSTEMCONFIGURATION_FRAMEWORK}
        ${IOKIT_FRAMEWORK}
        ${COREFOUNDATION_LIBRARY}
        ${COREGRAPHICS_LIBRARY}
        ${APPKIT_LIBRARY}
        ${QUARTZCORE_LIBRARY}
        "-framework ${FXPLM_DYLIB}"
        "-F${LNK_FXPLM_FRAMEWORK}"
        )

ENDIF( APPLE )




IF(CMAKE_SYSTEM_NAME STREQUAL Linux)

    add_definitions( -DLIN=1 )

    LINK_DIRECTORIES(
            "../FXPLM/cmake-build-debug/"
    )
SET( LIBS_LIN
        GLU
        glut
        glfw
        GL
        XPLM
        )
ENDIF(CMAKE_SYSTEM_NAME STREQUAL Linux)



SUBDIRS(
        #shim
        imgui_lib
        gz_core
)




SET( USE_LIBS
        ${LIBS_MAC}
        ${LIBS_LIN}
        # These are for Linux, might throw mac errors?

        GLEW
        #gz_api
        gzcore

        imgui_lib
        #shim
        )



set( SHIM_FOL "src/shim/" )




# These shim files are compiled as part of the main exe because linker optimization will drop the symbols from the
# output binary which causes plugin loading to fail.
# There is a linker flag to explore to work around this.
set( XP_SHIM_SRC

        src/main.cpp

        src/HostApp.cpp src/HostApp.h
        src/WinContent.cpp src/WinContent.h

        third_party/timer/src/Timer.cpp
        third_party/timer/src/Timer.h

        #stb image uses a weird macro to generate symbols
#        third_party/stb_image/stb_image.h

        src/StaticDefs.cpp
        src/GuiDatarefs.cpp        src/GuiDatarefs.h

        src/GuiPlugins.cpp          src/GuiPlugins.h
        src/GuiRecentProjects.cpp       src/GuiRecentProjects.h
        src/GuiTextures.cpp         src/GuiTextures.h
        src/GuiTextureInspector.h

        src/GuiPluginMessages.cpp   src/GuiPluginMessages.h
        src/GuiMemory.cpp           src/GuiMemory.h

        src/CommandsTxtParse.cpp    src/CommandsTxtParse.h
        src/DataRefsTxtParse.cpp    src/DataRefsTxtParse.h

        src/GuiShaderTest.h

        src/imgui_common.h
        #src/dep_FBO.h
        src/FXPLM.cpp
        src/FXPLM.h
        src/GuiAbout.cpp
        src/GuiAbout.h
        src/GuiPluginLoader.cpp
        src/GuiPluginLoader.h
        src/GuiXPLMLog.cpp
        src/GuiXPLMLog.h
        src/GuiMessageBox.cpp
        src/GuiMessageBox.h

        src/GuiWorldView.cpp src/GuiWorldView.h
        src/GuiGraph.cpp src/GuiGraph.h
        )





add_executable( ${PROJECT_NAME}
        
        ${XP_SHIM_SRC}
        )


IF( APPLE )
    set_target_properties(${PROJECT_NAME} PROPERTIES
            MACOSX_RPATH ON
            BUILD_WITH_INSTALL_RPATH ON
            INSTALL_RPATH "@executable_path/../../../Resources/plugins"
    )


    install(TARGETS ${PROJECT_NAME}
            #RENAME "XWB" # this does nothing :-(
            DESTINATION "/Users/br/XP12_Libs/XWB.app/Contents/MacOS"
            )


ENDIF( APPLE )


target_link_libraries( ${PROJECT_NAME}  ${USE_LIBS} )