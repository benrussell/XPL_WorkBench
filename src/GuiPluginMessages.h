//
// Created by br on 7/06/25.
//

#ifndef GUIPLUGINMESSAGES_H
#define GUIPLUGINMESSAGES_H



#include <vector>
#include <string>
#include <sstream>

#include "imgui.h"

#include "HostApp.h"
#include <functional>




class GuiPluginMessages {

public:

    static bool m_bDraw;

    static size_t m_msg_sender_plugin_id;
    static size_t m_msg_target_plugin_id;
    static size_t m_msg_message;
    static size_t m_msg_param; //refcons etc


    //	if( m_bDrawRecent )
    static void draw(
        // const std::function<void(std::string)>& cbf_load_project, //lambda
        // const std::function<void(std::string)>& cbf_load_plugin //lambda
        ){

        auto lam_splitString = [](const std::string& str, char delimiter) -> std::vector<std::string>{
            std::vector<std::string> tokens;
            std::string token;
            std::istringstream tokenStream(str);

            while (std::getline(tokenStream, token, delimiter)) {
                tokens.push_back(token);
            }

            return tokens;
        };



        ImGui::Begin("Plugin Messages", &m_bDraw );
        //		if( ImGui::BeginPopupModal("Recent Projects") )
        {

            //ImGui::InputInt("sender", &m_msg_sender_plugin_id);
            ImGui::InputScalar("sender", ImGuiDataType_U64, &m_msg_sender_plugin_id, nullptr, nullptr, "%zu");
            //ImGui::InputInt("target", &m_msg_target_plugin_id);
            ImGui::InputScalar("target", ImGuiDataType_U64, &m_msg_target_plugin_id, nullptr, nullptr, "%zu");
            //FIXME: drop down list of message types?
            //ImGui::InputInt("message", &m_msg_message);
            ImGui::InputScalar("message", ImGuiDataType_U64, &m_msg_message, nullptr, nullptr, "%zu");
            //ImGui::InputInt("param", &m_msg_param);
            ImGui::InputScalar("param", ImGuiDataType_U64, &m_msg_param, nullptr, nullptr, "%zu");

            if( ImGui::Button("Send Message") ){
                //FIXME:
                // ex_XPLMSendMessageToPlugin(
                //                         m_msg_sender_plugin_id,
                //                         m_msg_target_plugin_id,
                //                         m_msg_message,
                //                         (void*)m_msg_param
                //                         );
            }

            ImGui::Separator();

            if(HostApp::m_vecPluginMessages.empty()){
                ImGui::Text("No messages.");
            }

            //FIXME: child region for scroll

            for( const auto& msg: HostApp::m_vecPluginMessages ){
                // if( fn.empty() ){
                //     continue;
                // }

                // auto tokens = lam_splitString( fn, '/' );
                // std::string fn_clean = tokens[ tokens.size() - 1 ];
                //
                // if(ImGui::Button( ("Open [" + fn_clean +  "]##_btn_" + fn).c_str() )){
                //     //m_bDraw = false;
                //     //ImGui::CloseCurrentPopup();
                //     cbf_load_plugin( fn );
                // }
                // //ImGui::SameLine();

                ImGui::TextUnformatted( msg.c_str() );
            }


        }
        ImGui::End();

    }

};




#endif //GUIPLUGINMESSAGES_H
