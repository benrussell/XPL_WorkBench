//
// Created by br on 11/06/25.
//

#include "GuiDatarefs.h"



void GuiDatarefs::draw(){
    if( ! win_open ){
        return;
    }

    ImGui::SetNextWindowPos(ImVec2(0,390), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(320,360), ImGuiCond_FirstUseEver );

    const std::string devDesc = "Datarefs";
    ImGui::Begin(devDesc.c_str(), &win_open );

    if(XPHost::m_dref_pool.empty()) {
        ImGui::Text("Empty datarefs list.");
    }

    size_t wid=0;
    for( auto dr: XPHost::m_dref_pool ){

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
