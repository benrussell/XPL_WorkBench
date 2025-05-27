//
// Created by Ben on 27/9/2023.
//

#include "xp_core.h"
#include "shim_XPLMDataAccess.h"


#include <cstring>





[[maybe_unused]] void XPLMEnableFeature( const char* feature_name ){
	if( strcmp(feature_name, "XPLM_USE_NATIVE_PATHS") != 0 ){
		// mute the native paths request as thats all we support
		std::cout << "xplwb/ XPLMEnableFeature: NO OP: " << feature_name << "\n";
	}
}


[[maybe_unused]] void XPLMGetMouseLocationGlobal(){
	std::cout << "xplwb/ XPLMGetMouseLocationGlobal - NO OP.\n";
}

//DISPLAY









[[maybe_unused]] void XPLMGetSystemPath( char* outBuff ){
//	std::cout << "!XPLMGetSystemPath..\n";
	// snprintf( outBuff, 512, "/fake/x-plane/path" ); //FIXME:
	snprintf( outBuff, 512, "/media/br/MintyData/br/X-Plane 12" ); //FIXME:
}







[[maybe_unused]] int XPLMGetMyID( void ){
	return 42; //FIXME: return a useful ID
}

[[maybe_unused]] void XPLMGetPluginInfo( 
    int pluginId,
    char* outName,
    char* outFilePath,
    char* outSignature,
    char* outDescription
){

	//FIXME: lookup the plugin info struct with pluginId

	if( outName ){
		//set
	}

	if( outFilePath ){
		//set
		//snprintf(outFilePath,256,"/home/br/Dev/wasm/wasm_host_xpl/build/xpl/libwasm_xpl_plugin.so");
		
		printf("xplwb/ dyn resolved xpl filename! - yay!\n");
		snprintf(outFilePath,256,
			"%s",
			global_target_plugin->m_pluginFilename.c_str()
		);


	}

	if( outSignature ){
		//set
	}

	if( outDescription ){
		//set
	}

}
