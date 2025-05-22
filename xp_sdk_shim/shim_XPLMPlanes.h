
#ifndef GLFW_XP_HOST_SHIM_XPLMPLANES_H
#define GLFW_XP_HOST_SHIM_XPLMPLANES_H



extern "C" {

[[maybe_unused]] void XPLMGetNthAircraftModel(
    int inIndex,
    char* outFilename, // [256]
    char* outPath // [512]
);

}

#endif