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
// Created by Ben on 10/8/2024.
//

#include "GuiRecentProjects.h"


bool GuiRecentProjects::m_bDraw=true;





void GuiRecentProjects::draw(
        const std::function<void(std::string)>& cbf_load_project, //lambda
        const std::function<void(std::string)>& cbf_load_plugin //lambda
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



    ImGui::Begin("Recent Files", &m_bDraw );
    {

        ImGui::Text("Recent Plugins");

        if(HostApp::m_vecRecentPlugins.empty()){
            ImGui::Text("No recent plugins.");
        }

        for( const auto& fn: HostApp::m_vecRecentPlugins ){
            if( fn.empty() ){
                continue;
            }

            auto tokens = lam_splitString( fn, '/' );
            std::string fn_clean =
                    tokens[ tokens.size() - 3 ] + "/" +
                    tokens[ tokens.size() - 2 ] + "/" +
                    tokens[ tokens.size() - 1 ];

            if(ImGui::Button( ("Open [" + fn_clean +  "]##_btn_" + fn).c_str() )){
                //m_bDraw = false;
                cbf_load_plugin( fn );
            }

        }


        ImGui::Separator();

        ImGui::Text("Recent Projects");

        if(HostApp::m_vecRecentProjects.empty()){
            ImGui::Text("No recent projects.");
        }

        for( const auto& fn: HostApp::m_vecRecentProjects ){
            auto tokens = lam_splitString( fn, '/' );
            std::string fn_clean = tokens[ tokens.size() - 1 ];

            if(ImGui::Button( ("Open [" + fn_clean +  "]##_btn_" + fn).c_str() )){
                //m_bDraw = false;
                cbf_load_project( fn );
            }

        }

    }
    ImGui::End();

}