//
// Created by br on 2/8/24.
//
#include <GL/glew.h>

#include "shim_XPLMProcessing.h"


#include "../src/XPHost.h"



[[maybe_unused]] XPLMFlightLoopID XPLMCreateFlightLoop( XPLMCreateFlightLoop_t loop_params ){
    std::cout << "XPLMCreateFlightLoop\n";
//    std::cout << " " << loop_params.structSize << "\n";
//    std::cout << " " << loop_params.phase << "\n";
//    std::cout << " " << loop_params.callbackFunc << "\n";
//    std::cout << " " << loop_params.refcon << "\n";
	if( target_plugin ){
		return (void*)target_plugin->register_flcb( loop_params );
	}else{
		std::cout<<" XPLMCreateFlightLoop failed; bad plugin ptr\n";
	}

	return nullptr;
}


[[maybe_unused]] void XPLMDestroyFlightLoop(){
    std::cout << "!XPLMDestroyFlightLoop\n";
}

[[maybe_unused]] void XPLMRegisterFlightLoop(){
    std::cout << "!XPLMRegisterFlightLoop\n";
}





[[maybe_unused]] void XPLMScheduleFlightLoop( //FIXME
        XPLMFlightLoopID     inFlightLoopID,
        float                inInterval,
        int                  inRelativeToNow){
    std::cout << "XPLMScheduleFlightLoop\n";
//    std::cout << " " << inFlightLoopID << "\n";
//    std::cout << " " << inInterval << "\n";
//    std::cout << " " << inRelativeToNow << "\n";

	if( target_plugin ){
		//FIXME: this is going to fail?
		target_plugin->flcb_set( inFlightLoopID, inInterval, inRelativeToNow );

	}else{
		std::cout << "  XPLMScheduleFlightLoop failed: bad target_plugin ptrn\n";
	}


	std::cout<<"  XPLMScheduleFlightLoop: ret\n";

}







[[maybe_unused]] float XPLMGetElapsedTime(){
//	std::cout << "!XPLMGetElapsedTime..\n";

    static float hack = 0.1f;
    hack += 0.01f;

    return hack; //FIXME: Use Timer.h
}



