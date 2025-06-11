//
// Created by br on 1/8/24.
//


#include <iostream>
#include <vector>
#include <string>
#include <cstring>


#include <GL/glew.h>

#include "../src/WinContent.h"
#include "../src/AvionicsHost.h"
#include "../src/DrawCallbackHost.h"


#include "shim_XPLMDisplay.h"

#include "../src/XPHost.h"

extern std::vector<WinBox*> window_pool;

//extern XPLMCreateAvionics_t* fuck;
extern AvionicsHost* dev;



[[maybe_unused]] XPLMWindowID XPLMCreateWindowEx( XPLMCreateWindow_t *params ){
    std::cout << "XPLMCreateWindowEx\n";
    std::cout << " params: " << (size_t)params << "\n";

    XPLMCreateWindow_t *params_copy; //only used if incoming size is good.

    const size_t need_size = sizeof(XPLMCreateWindow_t);
    if( need_size != params->structSize ){
        std::cout << " params struct size mismatch/unknown\n";
        std::cout << " sizeof():" << need_size << " / p->size:" << params->structSize << "\n";
        return nullptr;
    }else{
        std::cout << " params size is good. taking a copy of the struct..\n";
        params_copy = new XPLMCreateWindow_t();
        memcpy( params_copy, params, need_size );
    }

	std::cout<< "  l,t,r,b:" << params->left <<","<< params->top <<","<< params->right <<","<< params->bottom << "\n";

    int width = params->right - params->left;
    int height = params->top - params->bottom;

    std::cout << " w:" << width << " h:" << height << "\n";
    std::cout << " drawWindowFunc: " << (size_t)params->drawWindowFunc << "\n";//
    std::cout << " refcon: " << (size_t)params->refcon << "\n";//


    // WinBox will crash when trying to draw.
#if 0
    auto b = new WinBox(width,height);

    //auto win_h = glfwCreateWindow(width, height, "XP Host Default Caption", NULL, NULL);
    glfwSetWindowUserPointer( b->m_winh, params_copy );
//    glfwSetWindowPos(b->m_winh, params->left, params->bottom + height);
	glfwSetWindowPos(b->m_winh, params->left, params->top*2);
    glfwSetWindowSize(b->m_winh, width, height);
	glfwHideWindow(b->m_winh);

    window_pool.push_back( b );

#endif

    return (XPLMWindowID)nullptr;

}



[[maybe_unused]] void XPLMDestroyWindow( XPLMWindowID inWindowID ){
    std::cout << "!XPLMDestroyWindow\n";
	auto b = (WinBox*)inWindowID;
	//b->m_winh
	delete b;
}




[[maybe_unused]] void XPLMGetWindowGeometry( XPLMWindowID inWindowID, int *left, int *top, int *right, int *bottom ){
//	std::cout << "XPLMGetWindowGeometry..\n";
	auto b = (WinBox*)inWindowID;
	int width,height;
	glfwGetWindowSize(b->m_winh, &width, &height);

	*left = 0;
	*bottom = 0;
	*right = width;
	*top = height;

	//std::cout<< "  l,t,r,b:" << *left <<","<< *top <<","<< *right <<","<< *bottom << "\n";

}

[[maybe_unused]] int XPLMGetWindowIsVisible( XPLMWindowID inWindowID ){
	std::cout << "XPLMGetWindowIsVisible\n";
	auto b = (WinBox*)inWindowID;
    return glfwGetWindowAttrib(b->m_winh, GLFW_VISIBLE);;
}

[[maybe_unused]] void XPLMSetWindowIsVisible( XPLMWindowID inWindowID, int inIsVisible ){
    std::cout << "XPLMSetWindowIsVisible..\n";
	auto b = (WinBox*)inWindowID;


	if( inIsVisible ){
        glfwShowWindow(b->m_winh);
    }else{
        glfwHideWindow(b->m_winh);
    }

}

[[maybe_unused]] int XPLMHasKeyboardFocus( XPLMWindowID inWindowID ){
	auto b = (WinBox*)inWindowID;
    return glfwGetWindowAttrib(b->m_winh, GLFW_FOCUSED);
}

[[maybe_unused]] int XPLMIsWindowInFront(){
//	std::cout << "!XPLMIsWindowInFront\n"; //FIXME
	return 1;
}




[[maybe_unused]] void XPLMSetWindowGeometry( XPLMWindowID inWindowID, int left, int top, int right, int bottom ){
	std::cout << "XPLMSetWindowGeometry\n";
//	std::cout << " l:" << left << "\n";
//	std::cout << " t:" << top << "\n";
//	std::cout << " r:" << right << "\n";
//	std::cout << " b:" << bottom << "\n";

	auto b = (WinBox*)inWindowID;

    int width = right - left;
    int height = top - bottom;

    glfwSetWindowPos(b->m_winh, left, bottom+height);
    glfwSetWindowSize(b->m_winh, width, height);

}

[[maybe_unused]] void XPLMSetWindowPositioningMode(){
    std::cout << "!XPLMSetWindowPositioningMode\n";
}

[[maybe_unused]] void XPLMSetWindowResizingLimits(XPLMWindowID         inWindowID,
												  int                  inMinWidthBoxels,
												  int                  inMinHeightBoxels,
												  int                  inMaxWidthBoxels,
												  int                  inMaxHeightBoxels){
    std::cout << "!XPLMSetWindowResizingLimits\n";
    std::cout<<"  window_id:" << (size_t)inWindowID <<"\n";
    std::cout<<"   min width: " << inMinWidthBoxels << "\n";
	std::cout<<"   min height: " << inMinHeightBoxels << "\n";
	std::cout<<"   max width: " << inMaxWidthBoxels << "\n";
	std::cout<<"   max height: " << inMaxHeightBoxels << "\n";



}

[[maybe_unused]] void XPLMSetWindowTitle( XPLMWindowID inWindowID, const char* new_title ){

	auto b = (WinBox*)inWindowID;

    glfwSetWindowTitle( b->m_winh, new_title );
}

[[maybe_unused]] void XPLMTakeKeyboardFocus( XPLMWindowID inWindowID ){
//	glfwFocusWindow(inWindowID);
}






void       XPLMAvionicsNeedsDrawing(
		XPLMAvionicsID       inHandle){
	std::cout<< "!XPLMAvionicsNeedsDrawing: " << (size_t)inHandle << "\n";
}


void       XPLMDestroyAvionics(
		XPLMAvionicsID       inHandle){
	std::cout<< "!XPLMDestroyAvionics: " << (size_t)inHandle << "\n";
}


[[maybe_unused]] void XPLMCreateAvionicsEx( void* params ){
    std::cout << "!XPLMCreateAvionicsEx\n";
    std::cout<< "  global_target_plugin: " << std::to_string((size_t)global_target_plugin) << "\n";

    auto p = (XPLMCreateAvionics_t*)params;
	std::cout<<" avionics device id: " << p->deviceID << "\n";
	std::cout<<" avionics device name: " << p->deviceName << "\n";
	std::cout<<" avionics drawing call: " << std::to_string( (size_t)p->drawCallback ) << "\n";
	std::cout<<" avionics refcon: " << std::to_string( (size_t)p->refcon ) << "\n";
	std::cout<<" av: width,height: " << p->screenWidth << ", " << p->screenWidth << "\n";
	std::cout<<" av: offset x,y: " << p->screenOffsetX << ", " << p->screenOffsetY << "\n";

	if( global_target_plugin ){
		std::cout<<"  saving deets.\n";
		auto av_host = new AvionicsHost{p};
		global_target_plugin->m_vecAvionicsHost.push_back( av_host );

		auto gui_av = new GuiAvionicsDevice();
		gui_av->m_dev = av_host;
		gui_av->m_plugin = global_target_plugin;
		global_target_plugin->m_vecGuiAv.push_back( gui_av );


	}else{
		throw std::runtime_error("target plugin is nullptr");
	}


}




[[maybe_unused]] void XPLMGetScreenBoundsGlobal(
		int *                outLeft,    /* Can be NULL */
		int *                outTop,    /* Can be NULL */
		int *                outRight,    /* Can be NULL */
		int *                outBottom /* Can be NULL */
){
//	std::cout << "!XPLMGetScreenBoudsGlobal\n";

	if( outLeft ){
		*outLeft = 0; //FIXME: verify values
	}

	if( outTop ){
		*outTop = 768;
	}

	if( outRight ){
		*outRight = 1024;
	}

	if( outBottom ){
		*outBottom = 0;
	}

}







[[maybe_unused]] int XPLMRegisterDrawCallback( 
 		XPLMDrawCallback_f   inCallback,
		XPLMDrawingPhase     inPhase,
		int                  inWantsBefore,
		void *               inRefcon){

			//create a draw callback wrapper instance
			DrawCallbackHost* dcbh = 
				new DrawCallbackHost(
					inCallback,
					inPhase,
					inWantsBefore,
					inRefcon
				);

			global_target_plugin->m_vecDrawCallbackHost.push_back( dcbh );

			return 1;
		}



[[maybe_unused]] int        XPLMUnregisterDrawCallback(
					XPLMDrawCallback_f   inCallback,
					XPLMDrawingPhase     inPhase,
					int                  inWantsBefore,
					void *               inRefcon){
					std::cout << "xwb/ XPLMUnregisterDrawCallback - NOOP\n";
					//ret 1 if the cb was found, ret 0 if it was not.
					return 0;
					};







[[maybe_unused]] void XPLMGetMouseLocationGlobal(){
	std::cout << "xwb/ XPLMGetMouseLocationGlobal - NO OP.\n";
}

//DISPLAY




