//
// Created by Ben on 14/3/2026.
//

#ifndef XPL_WORKBENCH_WORLDVIEW_H
#define XPL_WORKBENCH_WORLDVIEW_H

#include "glue_FBO.hpp" //FIXME: copy to gz_core

#include <XPLMDataAccess.h>

class WorldView {
public:
	gz_fbo* m_fboCanvas;

	bool m_bDrawTriangle = false;


	XPLMDataRef m_dr_LightX;
	XPLMDataRef m_dr_LightY;
	XPLMDataRef m_dr_LightZ;

	XPLMDataRef m_dr_LightR;
	XPLMDataRef m_dr_LightG;
	XPLMDataRef m_dr_LightB;
	XPLMDataRef m_dr_LightA;


	XPLMDataRef m_dr_view_x;
	XPLMDataRef m_dr_view_y;
	XPLMDataRef m_dr_view_z;

	XPLMDataRef m_dr_view_fov;

	XPLMDataRef m_dr_view_pitch;
	XPLMDataRef m_dr_view_roll;
	XPLMDataRef m_dr_view_heading;



	WorldView( int w, int h );
	~WorldView();

	void render_world( void* target_fbo, const float fov, const bool dbg_tri, const double dt );

	void draw_triangle_box( double dt );

};


#endif //XPL_WORKBENCH_WORLDVIEW_H
