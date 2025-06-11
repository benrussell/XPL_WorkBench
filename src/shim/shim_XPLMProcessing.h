//
// Created by br on 2/8/24.
//

#ifndef GLFW_XP_HOST_SHIM_XPLMPROCESSING_H
#define GLFW_XP_HOST_SHIM_XPLMPROCESSING_H


#include <GLFW/glfw3.h>
#include <iostream>


#include <vector>
#include <cstring>





enum XPLMFlightLoopPhaseType{
    xplm_FlightLoop_Phase_BeforeFlightModel,
    xplm_FlightLoop_Phase_AfterFlightModel
};

typedef float (* XPLMFlightLoop_f)(
        float                inElapsedSinceLastCall,
        float                inElapsedTimeSinceLastFlightLoop,
        int                  inCounter,
        void *               inRefcon);

typedef struct {
    int                       structSize;
    XPLMFlightLoopPhaseType   phase;
    XPLMFlightLoop_f          callbackFunc;
    void *                    refcon;
} XPLMCreateFlightLoop_t;

typedef void * XPLMFlightLoopID;





extern "C" {


[[maybe_unused]] float XPLMGetElapsedTime();

[[maybe_unused]] XPLMFlightLoopID XPLMCreateFlightLoop( XPLMCreateFlightLoop_t* loop_params );

[[maybe_unused]] void XPLMDestroyFlightLoop();

[[maybe_unused]] void XPLMRegisterFlightLoop();

[[maybe_unused]] void XPLMScheduleFlightLoop(
        XPLMFlightLoopID     inFlightLoopID,
        float                inInterval,
        int                  inRelativeToNow);


}


#endif //GLFW_XP_HOST_SHIM_XPLMPROCESSING_H
