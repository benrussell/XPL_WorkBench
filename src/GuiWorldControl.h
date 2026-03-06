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


	XPLMDataRef m_dr_light_x;
	XPLMDataRef m_dr_light_y;
	XPLMDataRef m_dr_light_z;

	XPLMDataRef m_dr_light_r;
	XPLMDataRef m_dr_light_g;
	XPLMDataRef m_dr_light_b;
	XPLMDataRef m_dr_light_a;



	void dref_bind() {
		std::cout << "world control dref_bind()\n";
		m_dr_light_x = XPLMFindDataRef("art/light/pos_x");
		m_dr_light_y = XPLMFindDataRef("art/light/pos_y");
		m_dr_light_z = XPLMFindDataRef("art/light/pos_z");

		m_dr_light_r = XPLMFindDataRef("art/light/col_r");
		m_dr_light_g = XPLMFindDataRef("art/light/col_g");
		m_dr_light_b = XPLMFindDataRef("art/light/col_b");
		m_dr_light_a = XPLMFindDataRef("art/light/col_a");



		//these view location and orientation drefs ARE used during the GL setup for 3D callback
		m_dr_view_x = XPLMFindDataRef("sim/graphics/view/view_x");
		m_dr_view_y = XPLMFindDataRef("sim/graphics/view/view_y");
		m_dr_view_z = XPLMFindDataRef("sim/graphics/view/view_z");

		m_dr_view_pitch = XPLMFindDataRef("sim/graphics/view/view_pitch");
		m_dr_view_roll = XPLMFindDataRef("sim/graphics/view/view_roll");
		m_dr_view_heading = XPLMFindDataRef("sim/graphics/view/view_heading");

	}


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


			fTmp = m_dr_view_pitch->getFloat();
			ImGui::SliderFloat("p", &fTmp, -90, 90);
			m_dr_view_pitch->setFloat(fTmp);

			fTmp = m_dr_view_roll->getFloat();
			ImGui::SliderFloat("r", &fTmp, -180, 180);
			m_dr_view_roll->setFloat(fTmp);

			fTmp = m_dr_view_heading->getFloat();
			ImGui::SliderFloat("h", &fTmp, -180, 180);
			m_dr_view_heading->setFloat(fTmp);

			{
				float myColor[4] = {
					m_dr_light_x->getFloat() / 10.f,
					m_dr_light_y->getFloat() / 10.f,
					m_dr_light_z->getFloat() / 10.f,
					1.f
				};
				// 2. Render the widget
				ImGui::ColorEdit4("pos", myColor);

				m_dr_light_x->setFloat(myColor[0] * 10.f);
				m_dr_light_y->setFloat(myColor[1] * 10.f);
				m_dr_light_z->setFloat(myColor[2] * 10.f);

			}


			float myColor[4] = {
				m_dr_light_r->getFloat(),
				m_dr_light_g->getFloat(),
				m_dr_light_b->getFloat(),
				m_dr_light_a->getFloat()
				};
			// 2. Render the widget
			ImGui::ColorEdit4("light", myColor);

			m_dr_light_r->setFloat(myColor[0]);
			m_dr_light_g->setFloat(myColor[1]);
			m_dr_light_b->setFloat(myColor[2]);
			m_dr_light_a->setFloat(myColor[3]);







			ImGui::End();
		}//m_bDraw?
	}
};



#endif //GUIWORLDCONTROL_H
