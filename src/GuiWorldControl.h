//
// Created by br on 2/03/26.
//

#ifndef GUIWORLDCONTROL_H
#define GUIWORLDCONTROL_H


#include <FXPLM.h>
#include <XPLMDataAccess.h>

#include "imgui.h"
#include "xp_dref.hpp"



class GuiWorldControl {
public:

	bool m_bDraw=false;


	XPLMDataRef m_dr_acf_p;
	XPLMDataRef m_dr_acf_h;
	XPLMDataRef m_dr_acf_r;
	XPLMDataRef m_dr_acf_speed;

	XPLMDataRef m_dr_acf_x;
	XPLMDataRef m_dr_acf_y;
	XPLMDataRef m_dr_acf_z;


	XPLMDataRef m_dr_view_x;
	XPLMDataRef m_dr_view_y;
	XPLMDataRef m_dr_view_z;

	XPLMDataRef m_dr_view_pitch;
	XPLMDataRef m_dr_view_roll;
	XPLMDataRef m_dr_view_heading;

	XPLMDataRef m_dr_view_fov;
	XPLMDataRef m_dr_view_is_external;


	XPLMDataRef m_dr_light_x;
	XPLMDataRef m_dr_light_y;
	XPLMDataRef m_dr_light_z;

	XPLMDataRef m_dr_light_r;
	XPLMDataRef m_dr_light_g;
	XPLMDataRef m_dr_light_b;
	XPLMDataRef m_dr_light_a;



	void dref_bind() {
		std::cout << "XWB/ GuiWorldControl.dref_bind()\n";

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

		m_dr_view_fov = XPLMFindDataRef("sim/graphics/view/field_of_view_deg");
		m_dr_view_is_external = XPLMFindDataRef("sim/graphics/view/view_is_external");


		m_dr_acf_x = XPLMFindDataRef("sim/flightmodel/position/local_x2");
		m_dr_acf_y = XPLMFindDataRef("sim/flightmodel/position/local_y2");
		m_dr_acf_z = XPLMFindDataRef("sim/flightmodel/position/local_z2");

		m_dr_acf_p = XPLMFindDataRef("sim/flightmodel/position/true_phi");
		m_dr_acf_r = XPLMFindDataRef("sim/flightmodel/position/true_theta");
		m_dr_acf_h = XPLMFindDataRef("sim/flightmodel/position/true_psi");
		m_dr_acf_speed = FXPLM_DrefCreate("xwb/hacks/acf_speed");

	}


	void draw( double dt ) {


		if (m_bDraw) {
			ImGui::SetNextWindowSize(ImVec2(640, 600), ImGuiCond_FirstUseEver);
			//ImGui::Begin("World View", &m_bDraw, ImGuiWindowFlags_MenuBar);
			ImGui::Begin("World Control", &m_bDraw);



			//ImGui::Text("rat: %0.2f,%0.2f", 1.f, 2.f );
			float fTmp;


			auto tree_options = ImGuiTreeNodeFlags_DefaultOpen;
			std::string sNodeLabel = "Camera";
			if( ImGui::TreeNodeEx( sNodeLabel.c_str(), tree_options ) ) {

				fTmp = XPLMGetDataf(m_dr_view_x);
				ImGui::SliderFloat("x", &fTmp, -10, 10);
				XPLMSetDataf(m_dr_view_x, fTmp);

				fTmp = XPLMGetDataf(m_dr_view_y);
				ImGui::SliderFloat("y", &fTmp, -10, 10);
				XPLMSetDataf(m_dr_view_y, fTmp);

				fTmp = XPLMGetDataf(m_dr_view_z);
				ImGui::SliderFloat("z", &fTmp, -10, 10);
				XPLMSetDataf(m_dr_view_z, fTmp);


				fTmp = XPLMGetDataf(m_dr_view_pitch);
				ImGui::SliderFloat("p", &fTmp, -90, 90);
				XPLMSetDataf(m_dr_view_pitch, fTmp);

				fTmp = XPLMGetDataf(m_dr_view_roll);
				ImGui::SliderFloat("r", &fTmp, -180, 180);
				XPLMSetDataf(m_dr_view_roll, fTmp);

				fTmp = XPLMGetDataf(m_dr_view_heading);
				ImGui::SliderFloat("h", &fTmp, -180, 180);
				XPLMSetDataf(m_dr_view_heading, fTmp);


				fTmp = XPLMGetDataf(m_dr_view_fov);
				ImGui::SliderFloat("fov", &fTmp, 10, 180);
				XPLMSetDataf(m_dr_view_fov, fTmp);


				int iTmp = XPLMGetDatai(m_dr_view_is_external);
				ImGui::SliderInt("external", &iTmp, 0, 1);
				XPLMSetDatai(m_dr_view_is_external, iTmp);


				ImGui::TreePop();
			}


			sNodeLabel = "Light";
			if( ImGui::TreeNodeEx( sNodeLabel.c_str(), tree_options ) ) {
				{
					float myColor[4] = {
							XPLMGetDataf(m_dr_light_x) / 10.f,
							XPLMGetDataf(m_dr_light_y) / 10.f,
							XPLMGetDataf(m_dr_light_z) / 10.f,
							1.f
					};
					// 2. Render the widget
					ImGui::ColorEdit4("pos", myColor);

					XPLMSetDataf(m_dr_light_x, myColor[0] * 10.f);
					XPLMSetDataf(m_dr_light_y, myColor[1] * 10.f);
					XPLMSetDataf(m_dr_light_z, myColor[2] * 10.f);
				}

				float myColor[4] = {
						XPLMGetDataf(m_dr_light_r),
						XPLMGetDataf(m_dr_light_g),
						XPLMGetDataf(m_dr_light_b),
						XPLMGetDataf(m_dr_light_a)
				};
				// 2. Render the widget
				ImGui::ColorEdit4("light", myColor);

				XPLMSetDataf(m_dr_light_r, myColor[0]);
				XPLMSetDataf(m_dr_light_g, myColor[1]);
				XPLMSetDataf(m_dr_light_b, myColor[2]);
				XPLMSetDataf(m_dr_light_a, myColor[3]);

				ImGui::TreePop();
			}


			sNodeLabel = "Vehicle";
			if( ImGui::TreeNodeEx( sNodeLabel.c_str(), tree_options ) ) {
				// x
				fTmp = XPLMGetDataf(m_dr_acf_x);
				ImGui::SliderFloat("x", &fTmp, -90, 90);
				XPLMSetDataf(m_dr_acf_x, fTmp);

				// y
				fTmp = XPLMGetDataf(m_dr_acf_y);
				ImGui::SliderFloat("y", &fTmp, -180, 180);
				XPLMSetDataf(m_dr_acf_y, fTmp);

				// z
				fTmp = XPLMGetDataf(m_dr_acf_z);
				ImGui::SliderFloat("z", &fTmp, -90, 90);
				XPLMSetDataf(m_dr_acf_z, fTmp);

				// pitch
				fTmp = XPLMGetDataf(m_dr_acf_p);
				ImGui::SliderFloat("p", &fTmp, -90, 90);
				XPLMSetDataf(m_dr_acf_p, fTmp);

				// roll
				fTmp = XPLMGetDataf(m_dr_acf_r);
				ImGui::SliderFloat("r", &fTmp, -90, 90);
				XPLMSetDataf(m_dr_acf_r, fTmp);

				// heading
				fTmp = XPLMGetDataf(m_dr_acf_h);
				ImGui::SliderFloat("h", &fTmp, -180, 180);
				XPLMSetDataf(m_dr_acf_h, fTmp);

				// speed
				fTmp = XPLMGetDataf(m_dr_acf_speed);
				ImGui::SliderFloat("speed", &fTmp, -1, 1);
				XPLMSetDataf(m_dr_acf_speed, fTmp);

				ImGui::TreePop();
			}



			ImGui::End();
		}//m_bDraw?
	}
};



#endif //GUIWORLDCONTROL_H
