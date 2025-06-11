//
// Created by br on 1/8/24.
//

#ifndef GLFW_XP_HOST_SHIM_XPLMDATAACCESS_H
#define GLFW_XP_HOST_SHIM_XPLMDATAACCESS_H

#include <vector>

#include "xp_dref.h"


#include "../Plugin.h"
class Plugin;
extern Plugin* global_target_plugin;


//alias for SDK compat
typedef xp_dref XPLMDataRef;

extern std::vector<xp_dref*> dref_pool;










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

	int XPLMGetDataRefTypes( XPLMDataRef *dref_h );

	double XPLMGetDatad( XPLMDataRef *dref_h );
	void XPLMSetDatad( XPLMDataRef *dref_h, double new_value );

	void XPLMGetDatab();
	void XPLMSetDatab();



[[maybe_unused]] XPLMDataRef* XPLMFindDataRef( const char* dref_name );



	//these are all placeholder type defs
	typedef void* XPLMGetDatai_f;
	typedef void* XPLMSetDatai_f;

	typedef void* XPLMGetDatavi_f;
	typedef void* XPLMSetDatavi_f;


	typedef void* XPLMGetDataf_f;
	typedef void* XPLMSetDataf_f;

	typedef void* XPLMGetDatavf_f;
	typedef void* XPLMSetDatavf_f;

	typedef void* XPLMGetDatad_f;
	typedef void* XPLMSetDatad_f;

	typedef void* XPLMGetDatab_f;
	typedef void* XPLMSetDatab_f;


	typedef int XPLMDataTypeID;

	void XPLMRegisterDataAccessor(
							 const char *         inDataName,
							 XPLMDataTypeID       inDataType,
							 int                  inIsWritable,
							 XPLMGetDatai_f       inReadInt,
							 XPLMSetDatai_f       inWriteInt,
							 XPLMGetDataf_f       inReadFloat,
							 XPLMSetDataf_f       inWriteFloat,
							 XPLMGetDatad_f       inReadDouble,
							 XPLMSetDatad_f       inWriteDouble,
							 XPLMGetDatavi_f      inReadIntArray,
							 XPLMSetDatavi_f      inWriteIntArray,
							 XPLMGetDatavf_f      inReadFloatArray,
							 XPLMSetDatavf_f      inWriteFloatArray,
							 XPLMGetDatab_f       inReadData,
							 XPLMSetDatab_f       inWriteData,
							 void *               inReadRefcon,
							 void *               inWriteRefcon);

void XPLMUnregisterDataAccessor();



} //extern "C"



#endif //GLFW_XP_HOST_SHIM_XPLMDATAACCESS_H
