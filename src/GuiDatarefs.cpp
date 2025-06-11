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

    const std::string devDesc = "DataRefs";
    ImGui::Begin(devDesc.c_str(), &win_open );

    static char filter_buff[256]{};
    ImGui::InputText("filter", filter_buff, 1024);
    size_t filter_len = strlen(filter_buff);

    if(XPHost::m_dref_pool.empty()) {
        ImGui::Text("Empty DataRefs list.");
    }



//    auto #include <cstring>

    auto lam_contains = [](const char* haystack, const char* needle) {
        // sanity checks
        if (!haystack || !needle)
            return false;
        // empty needle ⇒ consider it “found”
        if (*needle == '\0')
            return true;
        return std::strstr(haystack, needle) != nullptr;
    };





    char caLabel[512]{}; //gui label

    size_t wid=0;
    for( auto dr: XPHost::m_dref_pool ) {

        if ( lam_contains(dr->drefName.c_str(), filter_buff) )
        {
            snprintf(caLabel, 512, "%s  %s",
                             dr->drefName.c_str(), dr->typeName().c_str()
                    );

            if (ImGui::TreeNode(caLabel)) {
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

    }


    ImGui::End();
}
