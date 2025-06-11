//
// Created by Ben on 27/9/2023.
//

#ifndef GLFW_XP_HOST_XP_DREF_H
#define GLFW_XP_HOST_XP_DREF_H

#include <string>
#include <utility>
#include <iostream>


//	XPLMGetDatavf(gModelViewMatrixRef, mModelView, 0, 16);
//	XPLMGetDatavf(gProjectionMatrixRef, mProjection, 0, 16);
//	XPLMGetDatavi(gViewportRef, mViewport, 0, 4);

//	static float mModelView[16], mProjection[16];
//	static int mViewport[4];



enum xp_dref_type{
	dref_FrameRatePeriod, //sim/time/framerate_period
	dref_ModelViewMatrix, //sim/graphics/view/modelview_matrix
	dref_ProjectionMatrix, //sim/graphics/view/projection_matrix
	dref_Viewport, //sim/graphics/view/viewport
	dref_VREnabled, //sim/graphics/VR/enabled

	dref_Generic,

};


class xp_dref {
public:
	virtual ~xp_dref() = default;

	xp_dref( std::string name, xp_dref_type type ){
//		std::cout << "xp_dref() constructor ********\n";
		drefName = std::move(name);
		drefType = type;

		m_valDouble = 0.0;
		m_valFloat = 0.0f;
		m_valInt = 0;
	}

	std::string drefName;
	xp_dref_type drefType;

	float m_valFloat;
	int m_valInt;
	double m_valDouble;

	std::string typeName() const{
		switch ( drefType ) {
			case dref_FrameRatePeriod:
				return "frp";

			case dref_ModelViewMatrix:
				return "model";

			case dref_ProjectionMatrix:
				return "projection";

			case dref_Viewport:
				return "viewport";

			case dref_VREnabled:
				return "vr_enabled";

			case dref_Generic:
				return "generic";

			default:
				return "??";
		}
	}



	virtual float getFloat(){
//		std::cout << "xp_dref->read(): " << drefName << "\n";
		return m_valFloat;
	};

	virtual void setFloat( float new_val ){
		m_valFloat = new_val;
	}


	virtual int getInt(){
//		std::cout << "xp_dref->read(): " << drefName << "\n";
		return m_valInt;
	};

	virtual void setInt( int new_val ){
		m_valInt = new_val;
	}





//	virtual void read_arr( float* buff, int offset, int count );

};



#endif //GLFW_XP_HOST_XP_DREF_H
