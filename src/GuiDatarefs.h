//
// Created by Ben on 5/8/2024.
//

#ifndef GLFW_XP_HOST_GUIDATAREFS_H
#define GLFW_XP_HOST_GUIDATAREFS_H
#include <GL/glew.h>

#include "XPHost.h"

#include <string>
#include <vector>

#include "imgui.h"

#include "shim/shim_XPLMDataAccess.h"


class GuiDatarefs {
public:

	bool win_open = true;

	void draw(){
		if( ! win_open ){
			return;
		}

		ImGui::SetNextWindowPos(ImVec2(0,390), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(320,360), ImGuiCond_FirstUseEver );

		const std::string devDesc = "Datarefs";
		ImGui::Begin(devDesc.c_str(), &win_open );

		if(dref_pool.empty()) {
			ImGui::Text("Empty datarefs list.");
		}

		size_t wid=0;
		for( auto dr: dref_pool ){

			if( ImGui::TreeNode( dr->drefName.c_str() ) ){

				float fTmp = dr->getFloat();
				std::string widget_id = "##_slider_" + std::to_string(wid);
				ImGui::SliderFloat(widget_id.c_str(), &fTmp, -360, 360);

				std::string widget_id_txt = "##_text_" + std::to_string(wid);
				ImGui::InputFloat(widget_id_txt.c_str(), &fTmp);

				dr->setFloat(fTmp);

				ImGui::TreePop();
			}





			++wid;
		}


		ImGui::End();
	}


};


#endif //GLFW_XP_HOST_GUIDATAREFS_H
