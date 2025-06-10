//
// Created by br on 2/8/24.
//
#include <GL/glew.h>

#include "shim_XPLMProcessing.h"


#include "../src/XPHost.h"



[[maybe_unused]] XPLMFlightLoopID XPLMCreateFlightLoop( XPLMCreateFlightLoop_t* loop_params ){

	// printf(" loop_params ptr: %p\n", loop_params);

    // std::cout << "XPLMCreateFlightLoop\n";
	// printf(" sz: %i\n", loop_params->structSize);
	// printf(" phase: %i\n", loop_params->phase);
	// printf(" cbf: %p\n", loop_params->callbackFunc);
	// printf(" refcon: %p\n", loop_params->refcon);
	if( global_target_plugin ){
		return (void*)global_target_plugin->register_flcb( *loop_params );
	}else{
		std::cout<<" XPLMCreateFlightLoop failed; bad plugin ptr\n";
	}

	return nullptr;
}


[[maybe_unused]] void XPLMDestroyFlightLoop(){
    std::cout << "XPLMDestroyFlightLoop - NOOP!\n";
}

[[maybe_unused]] void XPLMRegisterFlightLoop(){
    std::cout << "XPLMRegisterFlightLoop - NOOP!\n";
}





[[maybe_unused]] void XPLMScheduleFlightLoop(
        XPLMFlightLoopID     inFlightLoopID,
        float                inInterval,
        int                  inRelativeToNow){
    // std::cout << "XPLMScheduleFlightLoop\n";
//    std::cout << " " << inFlightLoopID << "\n";
//    std::cout << " " << inInterval << "\n";
//    std::cout << " " << inRelativeToNow << "\n";

	if( global_target_plugin ){
		global_target_plugin->flcb_set( inFlightLoopID, inInterval, inRelativeToNow );

	}else{
		std::cout << "  XPLMScheduleFlightLoop failed: bad global_target_plugin ptrn\n";
	}

}







[[maybe_unused]] float XPLMGetElapsedTime(){
	// This is probably slightly different from X-Plane behaviour.
	// The X-Plane timer has "feature" where it stalls during various operations.

    return XPHost::m_timer.getElapsedTimeInSec();
}



