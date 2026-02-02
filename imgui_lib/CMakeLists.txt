

set( INC_IMGUI "../third_party/imgui-docking" )


set( XP_SHIM_SRC

        # Dear IMGUI
        ./${INC_IMGUI}/imconfig.h
        ./${INC_IMGUI}/imgui.cpp
        ./${INC_IMGUI}/imgui.h
        ./${INC_IMGUI}/imgui_demo.cpp
        ./${INC_IMGUI}/imgui_draw.cpp
        ./${INC_IMGUI}/imgui_internal.h
        ./${INC_IMGUI}/imgui_tables.cpp
        ./${INC_IMGUI}/imgui_widgets.cpp
        ./${INC_IMGUI}/imstb_rectpack.h
        ./${INC_IMGUI}/imstb_textedit.h
        ./${INC_IMGUI}/imstb_truetype.h

        ./${INC_IMGUI}/misc/cpp/imgui_stdlib.cpp
        ./${INC_IMGUI}/misc/cpp/imgui_stdlib.h

        ./${INC_IMGUI}/backends/imgui_impl_glfw.cpp
        ./${INC_IMGUI}/backends/imgui_impl_opengl2.cpp

        )


SET( XP_SHIM_LIB_NAME "imgui_lib" )


ADD_LIBRARY( ${XP_SHIM_LIB_NAME} STATIC ${XP_SHIM_SRC} )

#target_link_libraries( ${XP_SHIM_LIB_NAME}  )
