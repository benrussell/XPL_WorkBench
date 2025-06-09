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





set( IMGUI_ROOT "third_party/imgui-docking"
        )

set( INC_IMGUI
        ${IMGUI_ROOT}
        ${IMGUI_ROOT}/backends/
        third_party/imgui-filebrowser-master

        )



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

)


#we're not ready for this.
#trying to link with gz_core/gz_api lib results in weird
#missing symbols from the host app. solve another day.
set( INC_GZ_CORE ../gz_core/gz_api )
include_directories( ${INC_GZ_CORE} )


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

        )

ENDIF( APPLE )




IF(CMAKE_SYSTEM_NAME STREQUAL Linux)
SET( LIBS_LIN
        GLU
        glut
        glfw
        GL
        )
ENDIF(CMAKE_SYSTEM_NAME STREQUAL Linux)



SUBDIRS(
        #xp_sdk_shim
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
        #xp_sdk_shim
        )



set( SHIM_FOL "./xp_sdk_shim/" )




# These shim files are compiled as part of the main exe because linker optimization will drop the symbols from the
# output binary which causes plugin loading to fail.
# There is a linker flag to explore to work around this.
set( XP_SHIM_SRC
        ${SHIM_FOL}xp_core.cpp ${SHIM_FOL}xp_core.h
        ${SHIM_FOL}xp_dref.cpp ${SHIM_FOL}xp_dref.h
        ${SHIM_FOL}xp_sdk_shim.h

        ${SHIM_FOL}shim_XPLMDataAccess.cpp ${SHIM_FOL}shim_XPLMDataAccess.h
        ${SHIM_FOL}shim_XPLMDisplay.cpp ${SHIM_FOL}shim_XPLMDisplay.h
        ${SHIM_FOL}shim_XPLMGraphics.cpp ${SHIM_FOL}shim_XPLMGraphics.h
        ${SHIM_FOL}shim_XPLMPlanes.cpp ${SHIM_FOL}shim_XPLMPlanes.h
        ${SHIM_FOL}shim_XPLMProcessing.cpp ${SHIM_FOL}shim_XPLMProcessing.h
        ${SHIM_FOL}shim_XPLMUtilities.cpp ${SHIM_FOL}shim_XPLMUtilities.h

        src/Plugin.cpp src/Plugin.h
        src/XPHost.cpp src/XPHost.h
        src/WinContent.cpp src/WinContent.h
        src/AvionicsHost.h
        src/DrawCallbackHost.h
        src/GuiAvionicsDevice.h

        third_party/timer/src/Timer.cpp
        third_party/timer/src/Timer.h

        #stb image uses a weird macro to generate symbols
#        third_party/stb_image/stb_image.h

        src/StaticDefs.cpp
        src/GuiDatarefs.h
        src/xpCmdCustom.h
        src/GuiPlugins.cpp src/GuiPlugins.h
        src/GuiRecentProjects.cpp src/GuiRecentProjects.h
        src/GuiTextures.cpp src/GuiTextures.h
        src/GuiTextureInspector.h

        src/GuiPluginMessages.cpp src/GuiPluginMessages.h
        src/GuiMemory.cpp src/GuiMemory.h

        src/GuiShaderTest.h 
        src/imgui_common.h 
        src/FBO.h
        src/main.cpp
        )





add_executable( ${PROJECT_NAME}
        
        ${XP_SHIM_SRC}
        )


target_link_libraries( ${PROJECT_NAME}  ${USE_LIBS} )