//
// Created by Ben on 27/9/2023.
//

#ifndef GLFW_XP_HOST_XP_CORE_H
#define GLFW_XP_HOST_XP_CORE_H





extern "C" {

[[maybe_unused]] void XPLMEnableFeature( const char* feature_name );

[[maybe_unused]] int XPLMGetMyID( void );

[[maybe_unused]] void XPLMGetPluginInfo( 
    int pluginId,
    char* outName,
    char* outFilePath,
    char* outSignature,
    char* outDescription
);





} //extern "C"





#endif //GLFW_XP_HOST_XP_CORE_H
