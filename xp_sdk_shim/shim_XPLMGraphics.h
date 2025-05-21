//
// Created by br on 1/8/24.
//

#ifndef GLFW_XP_HOST_SHIM_XPLMGRAPHICS_H
#define GLFW_XP_HOST_SHIM_XPLMGRAPHICS_H

#include <GLFW/glfw3.h>
#include <iostream>


extern "C" {

[[maybe_unused]] void XPLMBindTexture2d( int textureNum, int textureUnit );


[[maybe_unused]] void XPLMGenerateTextureNumbers(GLuint *textures, int count );



[[maybe_unused]] void XPLMSetGraphicsState(
        int                  inEnableFog,
        int                  inNumberTexUnits,
        int                  inEnableLighting,
        int                  inEnableAlphaTesting,
        int                  inEnableAlphaBlending,
        int                  inEnableDepthTesting,
        int                  inEnableDepthWriting
);




}

#endif //GLFW_XP_HOST_SHIM_XPLMGRAPHICS_H
