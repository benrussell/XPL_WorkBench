
#include "shim_XPLMPlanes.h"

#include <iostream>


//#include "../XPHost.h"


[[maybe_unused]] void XPLMGetNthAircraftModel(
    int inIndex,
    char* outFileName,
    char* outPath
){
    std::cout << "xwb/ XPLMGetNthAircraftModel: " << inIndex << " \n";


    if( ! outFileName ){
        std::cout << "xwb/   outFileName is nullptr\n";
    }else{
        snprintf( outFileName, 256, "Cessna_172.acf" );
    }
    

    if( ! outPath ){
        std::cout << "xwb/   outPath is nullptr\n";
    }else{
        snprintf( outPath, 512, "Aircraft/Laminar Research/Cessna 172 SP" );
    }
    
}

