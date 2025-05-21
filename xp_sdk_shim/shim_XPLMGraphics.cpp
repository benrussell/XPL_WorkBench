//
// Created by br on 1/8/24.
//

#include <GL/glew.h>

#include "shim_XPLMGraphics.h"

#include "../src/XPHost.h"


[[maybe_unused]] void XPLMBindTexture2d( int textureNum, int textureUnit ){
//	std::cout << "XPLMBindTeture2d\n";
    glBindTexture( textureUnit, textureNum );
}



[[maybe_unused]] void XPLMGenerateTextureNumbers(GLuint *textures, int count ){
	std::cout << "XPLMGenerateTextureNumbers..\n";
    glGenTextures( count, textures );

    for( auto x=0; x<count; ++x ){
		target_plugin->m_vecTextures.push_back( textures[x] );
	}

}





[[maybe_unused]] void XPLMSetGraphicsState(
        int                  inEnableFog,
        int                  inNumberTexUnits,
        int                  inEnableLighting,
        int                  inEnableAlphaTesting,
        int                  inEnableAlphaBlending,
        int                  inEnableDepthTesting,
        int                  inEnableDepthWriting
){
//	std::cout << "!XPLMSetGraphicsState\n";

    if( inEnableFog ){
        glEnable( GL_FOG );
    }else{
        glDisable( GL_FOG );
    }

    if( inNumberTexUnits ){ //FIXME: loop and call n times..
        glEnable( GL_TEXTURE );
        glEnable( GL_TEXTURE0 );
        glEnable( GL_TEXTURE1 );
    }else{
        glDisable( GL_TEXTURE );
        glDisable( GL_TEXTURE0 );
        glDisable( GL_TEXTURE1 );
    }

    if( inEnableLighting ){
        glEnable( GL_LIGHTING );
    }else{
        glDisable( GL_LIGHTING );
    }

    if( inEnableAlphaTesting ){
        glEnable( GL_ALPHA_TEST );
    }else{
        glDisable( GL_ALPHA_TEST );
    }

    if( inEnableAlphaBlending ){
        glEnable( GL_BLEND_DST_ALPHA ); //FIXME: verify
    }else{
        glDisable( GL_BLEND_DST_ALPHA );
    }

    if( inEnableDepthTesting ){
        glEnable( GL_DEPTH_TEST );
    }else{
        glDisable( GL_DEPTH_TEST );
    }

    if( inEnableDepthWriting ){
        glEnable( GL_DEPTH_WRITEMASK ); //FIXME: verify
    }else{
        glDisable( GL_DEPTH_WRITEMASK );
    }

}
