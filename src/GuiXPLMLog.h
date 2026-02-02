//
// Created by Ben on 3/12/2025.
//

#ifndef GUIXPLMLOG_H
#define GUIXPLMLOG_H



#include <vector>
#include <string>
#include <sstream>

#include "imgui.h"
#include "imgui_common.h"

#include "HostApp.h"

#include "imfilebrowser.h"


#include <XPLM.h>



class GuiXPLMLog {
public:

	static inline bool m_bDraw=true;

	static void draw() {
		if( ! m_bDraw ) { return; }


		//XPLMDebugString log window
		// static bool bDrawLog=true;
		if( !HostApp::m_vecLog ) {
			std::cout<<"xwb/ HostApp::m_vecLog is nullptr - calling FXPLM_GetVecLogPtr() - GuiXPLMLog \n";
			HostApp::m_vecLog = (std::vector<std::string>*)FXPLM_GetVecLogPtr();
		}


		ImGui::SetNextWindowSize( ImVec2(320,240), ImGuiCond_FirstUseEver );
		ImGui::Begin("XPLMDebugString", &m_bDraw);

		{
			if( ImGui::Button("Clear") ){
				HostApp::m_vecLog->clear();
			}

			// Calculate the size of the child area (10 pixels smaller on each side)
			ImVec2 windowSize = ImGui::GetContentRegionAvail();
			ImVec2 childSize(windowSize.x - 20, windowSize.y - 20);

			// Start a child region within the main window with the calculated size
			ImGui::BeginChild("Child Area", childSize, true);

			const float wrap_width = ImGui::GetWindowWidth() - 30.f;
			ImGui::PushTextWrapPos(wrap_width);
			{
				// Add three text elements inside the child area
				size_t x = 0;
				if (HostApp::m_vecLog->size() > 50) {
					x = HostApp::m_vecLog->size() - 50;
				}

				const std::vector<std::string> vecFoo = *HostApp::m_vecLog;
				for (; x < HostApp::m_vecLog->size(); ++x) {
					const auto& line = vecFoo[x];
					const std::string log = std::to_string(x) + ":" + line;
					ImGui::TextUnformatted(log.c_str());
				}

				// Automatically scroll to the last item
				if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
					ImGui::SetScrollHereY(1.0f); // Scroll to bottom
				}
			}

			ImGui::PopTextWrapPos();
			// End the child region
			ImGui::EndChild();
		}
		ImGui::End();

	}

};



#endif //GUIXPLMLOG_H
