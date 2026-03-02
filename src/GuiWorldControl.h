//
// Created by br on 2/03/26.
//

#ifndef GUIWORLDCONTROL_H
#define GUIWORLDCONTROL_H


#include <XPLMDataAccess.h>

#include "imgui.h"
#include "xp_dref.hpp"



class GuiWorldControl {
public:

	bool m_bDraw=false;


	XPLMDataRef m_dr_view_x;
	XPLMDataRef m_dr_view_y;
	XPLMDataRef m_dr_view_z;

	XPLMDataRef m_dr_view_pitch;
	XPLMDataRef m_dr_view_roll;
	XPLMDataRef m_dr_view_heading;

	XPLMDataRef m_dr_view_fov;




	// GuiWorldControl() {
	// 	m_dr_view_x = XPLMFindDataRef("sim/foo/bar_x");
	// 	m_dr_view_y = XPLMFindDataRef("sim/foo/bar_y");
	// 	m_dr_view_z = XPLMFindDataRef("sim/foo/bar_z");
	// }


	void draw( double dt ) {
		// if( ! m_dr_view_x ) {
		// 	m_dr_view_x = XPLMFindDataRef("sim/graphics/view/view_x");
		// 	m_dr_view_y = XPLMFindDataRef("sim/graphics/view/view_y");
		// 	m_dr_view_z = XPLMFindDataRef("sim/graphics/view/view_z");
		// }


		if (m_bDraw) {
			ImGui::SetNextWindowSize(ImVec2(640, 600), ImGuiCond_FirstUseEver);
			//ImGui::Begin("World View", &m_bDraw, ImGuiWindowFlags_MenuBar);
			ImGui::Begin("World Control", &m_bDraw);

			//ImGui::Text("rat: %0.2f,%0.2f", 1.f, 2.f );
			float fTmp;

			fTmp = m_dr_view_x->getFloat();
			ImGui::SliderFloat("x", &fTmp, -10, 10);
			m_dr_view_x->setFloat(fTmp);

			fTmp = m_dr_view_y->getFloat();
			ImGui::SliderFloat("y", &fTmp, -10, 10);
			m_dr_view_y->setFloat(fTmp);

			fTmp = m_dr_view_z->getFloat();
			ImGui::SliderFloat("z", &fTmp, -10, 10);
			m_dr_view_z->setFloat(fTmp);

			ImGui::End();
		}//m_bDraw?
	}
};



#endif //GUIWORLDCONTROL_H
