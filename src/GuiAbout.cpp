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
// Created by Ben on 27/11/2025.
//

#include "GuiAbout.h"

#include "GuiDatarefs.h"

#include <XPLM.h>


void GuiAbout::draw( const std::string& startup_folder ){
    if( ! win_open ){
        return;
    }

    ImGui::SetNextWindowPos(ImVec2(0,390), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(320,360), ImGuiCond_FirstUseEver );

    const std::string devDesc = "About";
    ImGui::Begin(devDesc.c_str(), &win_open );

	// Calculate the size of the child area (10 pixels smaller on each side)
	ImVec2 windowSize = ImGui::GetContentRegionAvail();
	ImVec2 childSize(windowSize.x - 20, windowSize.y - 20);

	// Start a child region within the main window with the calculated size
	ImGui::BeginChild("Child Area", childSize, true);


	{
		ImGui::Text("XPL WorkBench v0.1.125.12.03.0520");
		ImGui::Text("             built: 25.12.29.0225");
		ImGui::Text("");
		ImGui::Text("(C) 2024-2025, Ben Russell - br@x-plugins.com");
		ImGui::Text("");

		ImGui::Text("This is an open source freeware project.");
		if (ImGui::Button("Open project on github")) {
			system("open https://github.com/benrussell/XPL_WorkBench");
		}
		ImGui::Text("");

		if (ImGui::Button("Follow along on Patreon")) {
			system("open https://patreon.com/benrussell/");
		}
		ImGui::Text("");

		if (ImGui::Button("Tip me on ko-fi")) {
			system("open https://ko-fi.com/benr/");
		}
		ImGui::Text("");

		ImGui::Text("Startup Folder:");
		ImGui::Text("[%s]", startup_folder.c_str());


#if 0
		static char filter_buff[256]{};
		ImGui::InputText("filter", filter_buff, 1024);
		size_t filter_len = strlen(filter_buff);

		ImGui::SameLine();
		static bool bShowAll = true;
		ImGui::Checkbox("All", &bShowAll);

		if ( HostApp::m_dref_pool == nullptr ) {
			ImGui::Text("dref pool is nullptr");
			ImGui::End();

			HostApp::m_dref_pool = (std::vector<xp_dref*>*)FXPLM_DrefPoolPtr();

			return;
		}



		if(HostApp::m_dref_pool->empty()) {
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
		bool bShowThisItem = false;
		for( auto dr: *HostApp::m_dref_pool ) {

			bShowThisItem = (dr->m_vecPluginConsumers.size() > 0) || bShowAll;

			if (
				lam_contains(dr->drefName.c_str(), filter_buff)
				&& bShowThisItem
				)
			{
				snprintf(caLabel, 512, "%s  %s  %s",
								 dr->drefName.c_str(), dr->typeName().c_str(), dr->drefTypeName.c_str()
						);

				if (ImGui::TreeNode(caLabel)) {

					char buff[512];
#if 1
					snprintf( buff, 512, "consumers[%li]", dr->m_vecPluginConsumers.size() );
					if (ImGui::TreeNode(buff)) {
						for ( auto plugin: dr->m_vecPluginConsumers ) {
							ImGui::Text("%s", plugin->m_pluginSig.c_str() );
						}
						ImGui::TreePop();
					}
#endif

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
#endif
	}
	ImGui::EndChild();

    ImGui::End();
}
