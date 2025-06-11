

set( XP_SHIM_SRC
        xp_core.cpp xp_core.h
        xp_dref.cpp xp_dref.h
        xp_sdk_shim.h

        shim_XPLMDataAccess.cpp shim_XPLMDataAccess.h
        shim_XPLMDisplay.cpp shim_XPLMDisplay.h
        shim_XPLMGraphics.cpp shim_XPLMGraphics.h
        shim_XPLMProcessing.cpp shim_XPLMProcessing.h
        shim_XPLMUtilities.cpp shim_XPLMUtilities.h

        )


SET( XP_SHIM_LIB_NAME "xp_sdk_shim" )


set(CMAKE_CXX_FLAGS "-O0 -rdynamic -ldl")


ADD_LIBRARY( ${XP_SHIM_LIB_NAME} STATIC ${XP_SHIM_SRC} )

#target_link_libraries( ${XP_SHIM_LIB_NAME}  )
