

set( GZ_CORE ../../gz_core/gz_api )

set( GZ_CORE_SRC
        #./${GZ_CORE}/gz_gfx.h
        #./${GZ_CORE}/gz_gfx.cpp
        ./${GZ_CORE}/gz_shader.h
        ./${GZ_CORE}/gz_shader.cpp
        ./${GZ_CORE}/gz_log.h
        ./${GZ_CORE}/gz_log.cpp
        ./${GZ_CORE}/gz_clock.h
        ./${GZ_CORE}/gz_clock.cpp
        )


SET( LIB_GZ_CORE_NAME "gzcore" )


ADD_LIBRARY( ${LIB_GZ_CORE_NAME} STATIC ${GZ_CORE_SRC} )

#target_link_libraries( ${XP_SHIM_LIB_NAME}  )
