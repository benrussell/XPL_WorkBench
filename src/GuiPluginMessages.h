/*
 * XPL_WorkBench - An X-Plane Plugin host.
 * Copyright (C) 2024-2026 Ben Russell - br@x-plugins.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 */

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
#include <XPLMPlugin.h>


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

        if ( ! m_bDraw ) { return; }

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

            {
                // drop down list of message types?
                const char* items[] = {
                    "Custom",
                    "PLANE_CRASHED",
                    "PLANE_LOADED",
                    "AIRPORT_LOADED",
                    "SCENERY_LOADED",
                    "AIRPLANE_COUNT_CHANGED",
                    "PLANE_UNLOADED",
                    "WILL_WRITE_PREFS",
                    "LIVERY_LOADED",
                    "ENTERED_VR",
                    "EXITING_VR",
                    "RELEASE_PLANES",
                    "FMOD_BANK_LOADED",
                    "FMOD_BANK_UNLOADING",
                    "DATAREFS_ADDED"
                };
                static int item_current = 0;

                const int item_vals[] = {
                    0, //Custom
                    XPLM_MSG_PLANE_CRASHED,
                    XPLM_MSG_PLANE_LOADED,
                    XPLM_MSG_AIRPORT_LOADED,
                    XPLM_MSG_SCENERY_LOADED,
                    XPLM_MSG_AIRPLANE_COUNT_CHANGED,
                    XPLM_MSG_PLANE_UNLOADED,
                    XPLM_MSG_WILL_WRITE_PREFS,
                    XPLM_MSG_LIVERY_LOADED,
                    XPLM_MSG_ENTERED_VR,
                    XPLM_MSG_EXITING_VR,
                    XPLM_MSG_RELEASE_PLANES,
                    XPLM_MSG_FMOD_BANK_LOADED,
                    XPLM_MSG_FMOD_BANK_UNLOADING,
                    XPLM_MSG_DATAREFS_ADDED
                };

                // Signature: (Label, &current_index, items_array, items_count)
                if(ImGui::Combo("SDK MSGS", &item_current, items, IM_ARRAYSIZE(items))) {
                    if ( item_current != 0 ) {
                        std::cout<<"xwb/ plugin msg/ sdk msg selected: " << items[item_current] <<"\n";
                        m_msg_message=item_vals[item_current];
                    }
                }

            }


            //ImGui::InputInt("message", &m_msg_message);
            ImGui::InputScalar("message", ImGuiDataType_U64, &m_msg_message, nullptr, nullptr, "%zu");
            //ImGui::InputInt("param", &m_msg_param);
            ImGui::InputScalar("param", ImGuiDataType_U64, &m_msg_param, nullptr, nullptr, "%zu");

            if( ImGui::Button("Send Message") ){
				std::cout << "xwb / plugin messages / send message..\n";
				FXPLM_SendMessageToPlugin(
									 m_msg_sender_plugin_id,
									 m_msg_target_plugin_id,
									 m_msg_message,
									 (void*)m_msg_param
									 );
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
