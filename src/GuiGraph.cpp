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
// Created by Ben on 27/1/2026.
//

#include "GuiGraph.h"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"


void GuiGraph::draw() {
	if( ! win_open ){
		return;
	}


	ImGui::SetNextWindowPos(ImVec2(0,390), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(320,360), ImGuiCond_FirstUseEver );

	const std::string devDesc = "Graph";
	ImGui::Begin(devDesc.c_str(), &win_open );

	auto getter = [](void* data, int idx) -> float {
		auto* d = static_cast<std::deque<float>*>(data);
		return (*d)[idx];
	};

	ImGui::PlotLines("##FPS Graph", getter, &dq_window, static_cast<int>(dq_window.size()),
					 0, //not sure?
					 "FPS", //units?
					 0.0f, //min?
					 120.0f, //max?
					 ImVec2(0, 80)
					 );


	ImGui::End();

}



void GuiGraph::add_item(float val, bool at_tail) {
	if (at_tail) {
		dq_window.push_back(val);
	} else {
		dq_window.push_front(val);
	}

	// Maintain the window size
	if (dq_window.size() > MAX_SIZE) {
		if (at_tail) {
			dq_window.pop_front(); // Remove oldest from head
		} else {
			dq_window.pop_back();  // Remove oldest from tail
		}
	}
}
