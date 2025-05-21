//
// Created by Ben on 27/9/2023.
//

#include "xp_core.h"
#include "shim_XPLMDataAccess.h"


#include <cstring>





[[maybe_unused]] void XPLMEnableFeature( const char* feature_name ){
	if( strcmp(feature_name, "XPLM_USE_NATIVE_PATHS") != 0 ){
		// mute the native paths request as thats all we support
		std::cout << "!XPLMEnableFeature: " << feature_name << "\n";
	}
}


[[maybe_unused]] void XPLMGetMouseLocationGlobal(){

}

//DISPLAY


[[maybe_unused]] void XPLMGetSystemPath( char* outBuff ){
//	std::cout << "!XPLMGetSystemPath..\n";
	snprintf( outBuff, 512, "./" );
}
