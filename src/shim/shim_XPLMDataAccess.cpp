//
// Created by br on 1/8/24.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shim_XPLMDataAccess.h"
#include "../XPHost.h"




[[maybe_unused]] void XPLMSetDatavf( XPLMDataRef *dref_h, float* new_values, int offset, int count ){

	if( dref_h ){
		auto dr = reinterpret_cast<xp_dref*>(dref_h);
		dr->setFloat( new_values[0] );

	}

}


[[maybe_unused]] void XPLMSetDatavi( XPLMDataRef *dref_h, int* new_values, int offset, int count ){

	if( dref_h ){
		auto dr = reinterpret_cast<xp_dref*>(dref_h);
		dr->setFloat( new_values[0] ); //FIXME

	}

}




[[maybe_unused]] void XPLMSetDataf( XPLMDataRef *dref_h, float new_value ){
//    std::cout << "!XPLMSetDataf\n"; //FIXME

	if( dref_h ){
		auto dr = reinterpret_cast<xp_dref*>(dref_h);
		dr->setFloat( new_value );

	}


}



[[maybe_unused]] void XPLMGetDatavi( XPLMDataRef* dref_h, int* buffer, int offset, int count ){
//	std::cout << "XPLMGetDatavi ret fixed values\n";

    //XP11 sample values: 0,0,0,0

    // prob only used for view matrix at this stage
    buffer[0] = 0; //left
    buffer[1] = 0; //bottom
    buffer[2] = 0; //right
    buffer[3] = 0; //top


	std::cout << "XPLMGetDatavi: hard coded.\n";

}




[[maybe_unused]] void XPLMGetDatavf( XPLMDataRef* dref_h, float* buffer, int offset, int count ){

//	std::cout << "XPLMGetDatavf\n";
//	std::cout << " dref_h " << dref_h << "\n";
//	std::cout << " buffer " << buffer << "\n";
//	std::cout << " offset " << offset << "\n";
//	std::cout << " count  " << count << "\n";

    if( dref_h ){

        auto dr_base = reinterpret_cast<xp_dref*>(dref_h);
        switch( dr_base->drefType ){
            case xp_dref_type::dref_ModelViewMatrix:
//				std::cout << " model view matrix\n";
                glGetFloatv( GL_MODELVIEW_MATRIX, buffer );

                //XP11
                // [1.,-0.,-0.000001,0.,0.,1.,0.,0.,0.000001,-0.,1.,0.,0.360008,-1.58,12.599999,1.]

                break;

            case xp_dref_type::dref_ProjectionMatrix:
//				std::cout << " projection matrix\n";
                glGetFloatv( GL_PROJECTION_MATRIX, buffer );

                // [0.001563,0.,0.,0.,0.,0.002778,0.,0.,0.,0.,-1.,0.,-1.,-1.,-0.,1.]

                break;

            default:
                std::cout << " unexpected: " << dr_base->drefName << "\n";
                break;
        }

    }


	std::cout << "XPLMGetDatavf: bad handle.\n";

}





[[maybe_unused]] float XPLMGetDataf( XPLMDataRef* dref_h ){

    if( dref_h ){
        auto dr = reinterpret_cast<xp_dref*>(dref_h);

        float ret=0.f;

        switch( dr->drefType ){
        	case dref_FrameRatePeriod:
            	ret = 1.f / XPHost::fps;
                break;

            default:
				ret = dr->getFloat();
				break;
        }

//        std::cout << "XPLMGetDataf: returning:" << ret << "\n";
        return ret;

    }

    //could not find a dref
	std::cout << "XPLMGetDataf: bad handle.\n";
    return 0.f;
}








[[maybe_unused]] int XPLMGetDatai( XPLMDataRef* dref_h ){
//    std::cout << "XPLMGetDatai\n";

	if( dref_h ){
		auto dr = reinterpret_cast<xp_dref*>(dref_h);

		int ret=0;

		switch( dr->drefType ){
//			case dref_FrameRatePeriod:
//			{
//				auto dr_frp = reinterpret_cast<xp_dref_frp *>(dr);
//				ret = dr_frp->getFloat();
//			}
//				break;
			default:
				ret = dr->getInt();
				break;
		}

//        std::cout << "XPLMGetDataf: returning:" << ret << "\n";
		return ret;

	}

	std::cout << "XPLMGetDatai: bad handle.\n";
	return 0;
}



[[maybe_unused]] void XPLMSetDatai( XPLMDataRef* dref_h, int new_value ){
//    std::cout << "XPLMSetDatai\n";

	if( dref_h ){
		auto dr = reinterpret_cast<xp_dref*>(dref_h);
		dr->setInt( new_value );

	}

};



[[maybe_unused]] XPLMDataRef* XPLMFindDataRef( const char* dref_name ){
	std::cout << "XPLMFindDataRef(" << dref_name << ")\n";

    const std::string search_name = std::string(dref_name);
    xp_dref *dr = dref_factory::findDref( std::string(dref_name) );

	if ( dr ) {
		dr->m_vecPluginConsumers.push_back(global_target_plugin);
	}

    return dr;
}



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
						 void *               inWriteRefcon)
	{
		std::cout<<"wxb/ XPLMRegisterDataAccessor [" << inDataName << "]\n";
		std::string custom_dref_name = std::string(inDataName);
		dref_factory::saveDref(custom_dref_name, "accessor");

		//throw std::runtime_error("custom drefs not supported. :(");

	};


void XPLMUnregisterDataAccessor(){
	std::cout<<"wxb/ XPLMUnregisterDataAccessor - NOOP!\n"; //FIXME:
};






int XPLMIsDataRefGood( void* dref ) {
	std::cout<<"wxb/ XPLMIsDatarefGood - doesnt track dref ownership\n"; //FIXME:
	return 1;
}



/*
Unknown 0
Int 1
Float 2
Double 4
FloatArray 8
IntArray 16
Data 32
*/

int XPLMGetDataRefTypes( XPLMDataRef *dref_h ) {
	std::cout<<"wxb/ XPLMGetDataRefTypes - broken\n";

	if( dref_h ) {
		auto dr = reinterpret_cast<xp_dref*>(dref_h);

		switch( dr->drefType ) {
			case xp_dref_type::dref_Generic:
				return 2;
				break;

			default:
				return 0;
		}
	}

	return 2;
}


void XPLMSetDatad( XPLMDataRef *dref_h, double new_value ) {
	std::cout<<"wxb/ XPLMSetDatad - NOOP!\n"; //FIXME
}

void XPLMSetDatab() {
	std::cout<<"wxb/ XPLMSetDatab - NOOP!\n"; //FIXME
}


double XPLMGetDatad(  XPLMDataRef *dref_h ) {
	std::cout<<"wxb/ XPLMGetDatad - broken!\n"; //FIXME

	if( dref_h ){
		auto dr = reinterpret_cast<xp_dref*>(dref_h);

		int ret=0;

		switch( dr->drefType ){
			//			case dref_FrameRatePeriod:
			//			{
			//				auto dr_frp = reinterpret_cast<xp_dref_frp *>(dr);
			//				ret = dr_frp->getFloat();
			//			}
			//				break;
			default:
				ret = 0.0; //dr->getDouble();
				break;
		}

		//        std::cout << "XPLMGetDataf: returning:" << ret << "\n";
		return ret;

	}

	std::cout << "XPLMGetDatad: bad handle.\n";
	return 0.0;

}

void XPLMGetDatab() {
	std::cout<<"wxb/ XPLMGetDatab - NOOP!\n";
}



