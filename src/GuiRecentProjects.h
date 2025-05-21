//
// Created by Ben on 10/8/2024.
//

#ifndef XDBG_GUIRECENTPROJECTS_H
#define XDBG_GUIRECENTPROJECTS_H


#include <vector>
#include <string>
#include <sstream>

#include "imgui.h"

#include "XPHost.h"
#include <functional>

class GuiRecentProjects {
public:

	static bool m_bDraw;


//	if( m_bDrawRecent )
	static void draw( const std::function<void(std::string)>& cbf_load_project )
	{

		auto lam_splitString = [](const std::string& str, char delimiter) -> std::vector<std::string>{
			std::vector<std::string> tokens;
			std::string token;
			std::istringstream tokenStream(str);

			while (std::getline(tokenStream, token, delimiter)) {
				tokens.push_back(token);
			}

			return tokens;
		};



		ImGui::Begin("Recent Projects", &m_bDraw );
//		if( ImGui::BeginPopupModal("Recent Projects") )
		{

			if(XPHost::m_vecRecentProjects.empty()){
				ImGui::Text("No recent projects.");
			}

			if( ImGui::Button("Not Now..") ){
				ImGui::CloseCurrentPopup();
				m_bDraw = false;
			}


			//FIXMEL child region for scroll
			for( const auto& fn: XPHost::m_vecRecentProjects ){

				auto tokens = lam_splitString( fn, '/' );
				std::string fn_clean = tokens[ tokens.size() - 1 ];

				if(ImGui::Button( ("Open [" + fn_clean +  "]##_btn_" + fn).c_str() )){
					m_bDraw = false;
					ImGui::CloseCurrentPopup();
					cbf_load_project( fn );
				}
				//ImGui::SameLine();

				//ImGui::TextUnformatted( fn_clean.c_str() );
			}

//			ImGui::EndPopup();
		}
		ImGui::End();

	}

};


#endif //XDBG_GUIRECENTPROJECTS_H
