//
// Created by br on 1/8/24.
//

#ifndef GLFW_XP_HOST_SHIM_XPLMDATAACCESS_H
#define GLFW_XP_HOST_SHIM_XPLMDATAACCESS_H

#include <vector>

#include "xp_dref.h"


#include "../src/Plugin.h"
class Plugin;
extern Plugin* global_target_plugin;


//alias for SDK compat
typedef xp_dref XPLMDataRef;

extern std::vector<xp_dref*> dref_pool;




class drp{
public:
	static xp_dref* saveDref( const std::string& name ){

		std::cout<<"drp::saveDref: " << name << "\n";


//		bool need = true;
		for( auto dr: dref_pool ){
			if( dr->drefName == name ){
				//need=false;
				std::cout << "  dref found: ret existing.\n";
				return dr;
			}
		}

//		if(need){
			auto drTmp = new xp_dref( name, xp_dref_type::dref_Generic );
			dref_pool.push_back(drTmp);
			std::cout << "  dref created: ret new.\n";
			return drTmp;
//		}

	}; //saveDref



	static void init(){

//	XPLMGetDatavf(gModelViewMatrixRef, mModelView, 0, 16);
//	XPLMGetDatavf(gProjectionMatrixRef, mProjection, 0, 16);
//	XPLMGetDatavi(gViewportRef, mViewport, 0, 4);

//	static float mModelView[16], mProjection[16];
//	static int mViewport[4];


//		auto dr_frp = new xp_dref_frp( "sim/time/framerate_period", xp_dref_type::dref_FrameRatePeriod );
//		dref_pool.push_back(dr_frp);

		auto dr_mvm = new xp_dref( "sim/graphics/view/modelview_matrix", xp_dref_type::dref_ModelViewMatrix );
		dref_pool.push_back(dr_mvm);

		auto dr_pm = new xp_dref( "sim/graphics/view/projection_matrix", xp_dref_type::dref_ProjectionMatrix );
		dref_pool.push_back(dr_pm);

		auto dr_vp = new xp_dref( "sim/graphics/view/viewport", xp_dref_type::dref_Viewport );
		dref_pool.push_back(dr_vp);

		auto dr_vr = new xp_dref( "sim/graphics/VR/enabled", xp_dref_type::dref_VREnabled );
		dref_pool.push_back(dr_vr);

	};


};










extern "C" {


[[maybe_unused]] void XPLMSetDataf( XPLMDataRef *dref_h, float new_value );

[[maybe_unused]] void XPLMSetDatavf( XPLMDataRef *dref_h, float* new_values, int offset, int count );
[[maybe_unused]] void XPLMSetDatavi( XPLMDataRef *dref_h, int* new_values, int offset, int count );


//[[maybe_unused]] float XPLMGetDataf();
[[maybe_unused]] float XPLMGetDataf( XPLMDataRef* dref_h );

[[maybe_unused]] int XPLMGetDatai( XPLMDataRef* dref_h );

[[maybe_unused]] void XPLMSetDatai( XPLMDataRef* dref_h, int val );


[[maybe_unused]] void XPLMGetDatavf( XPLMDataRef* dref_h, float* buffer, int offset, int count );

[[maybe_unused]] void XPLMGetDatavi( XPLMDataRef* dref_h, int* buffer, int offset, int count );


	int XPLMIsDataRefGood( void* dref );

	void XPLMGetDataRefTypes();

	double XPLMGetDatad( XPLMDataRef *dref_h );
	void XPLMSetDatad( XPLMDataRef *dref_h, double new_value );

	void XPLMGetDatab();
	void XPLMSetDatab();



[[maybe_unused]] XPLMDataRef* XPLMFindDataRef( const char* dref_name );



void XPLMRegisterDataAccessor();
void XPLMUnregisterDataAccessor();



} //extern "C"



#endif //GLFW_XP_HOST_SHIM_XPLMDATAACCESS_H
