//
// Created by Ben on 27/9/2023.
//

#include "shim_XPLMPlugin.h"
#include "shim_XPLMDataAccess.h"


#include <cstring>





#include "../src/Plugin.h"

#include "../src/XPHost.h"




[[maybe_unused]] void XPLMEnableFeature( const char* feature_name ){
	if( strcmp(feature_name, "XPLM_USE_NATIVE_PATHS") != 0 ){
		// mute the native paths request as thats all we support
		std::cout << "xwb/ XPLMEnableFeature: NO OP: " << feature_name << "\n";
	}
}



[[maybe_unused]] int XPLMGetMyID( void ){
	std::cout << "xwb/ XPLMGetMyID: [" <<
			global_target_plugin->m_plugin_id <<
			"] " <<
			global_target_plugin->m_pluginSig <<
			".\n";
	return global_target_plugin->m_plugin_id;
}

[[maybe_unused]] void XPLMGetPluginInfo( 
    int pluginId,
    char* outName,
    char* outFilePath,
    char* outSignature,
    char* outDescription
){

	int targetId = pluginId - 1;
	if ( targetId >= XPHost::m_vecPlugins.size() ) {
		std::cerr << "XPLMGetPluginInfo: targetId is out of bounds.\n";
		return;
	}
	auto plugin = XPHost::m_vecPlugins[targetId];

	if( outName ){
		snprintf(outName,256,
			"%s",
			plugin->m_pluginName.c_str()
		);
	}

	if( outFilePath ){
		snprintf(outFilePath,256,
			"%s",
			plugin->m_pluginFilename.c_str()
		);
	}

	if( outSignature ){
		snprintf(outSignature,256,
			"%s",
			plugin->m_pluginSig.c_str()
		);
	}

	if( outDescription ){
		snprintf(outDescription,256,
			"%s",
			plugin->m_pluginDesc.c_str()
		);
	}

}
