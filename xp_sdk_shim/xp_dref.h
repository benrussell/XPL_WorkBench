//
// Created by Ben on 27/9/2023.
//

#ifndef GLFW_XP_HOST_XP_DREF_H
#define GLFW_XP_HOST_XP_DREF_H

#include <string>
#include <utility>
#include <iostream>

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
	xp_dref( std::string name, xp_dref_type type ){
//		std::cout << "xp_dref() constructor ********\n";
		drefName = std::move(name);
		drefType = type;
	}

	std::string drefName;
	xp_dref_type drefType;

	float m_valFloat;
	int m_valInt;
	double m_valDouble;

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






//
//
//class xp_dref_frp: public xp_dref{
//public:
//	xp_dref_frp( std::string name, xp_dref_type type ):
//	xp_dref(name, type){}
//
//	float getFloat() override{
//		return 0.12345f;
//	}
//};



#endif //GLFW_XP_HOST_XP_DREF_H
