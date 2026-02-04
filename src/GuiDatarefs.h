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
// Created by Ben on 5/8/2024.
//

#ifndef GLFW_XP_HOST_GUIDATAREFS_H
#define GLFW_XP_HOST_GUIDATAREFS_H
#include <GL/glew.h>

#include "HostApp.h"

#include <string>
#include <vector>

#include <XPLM.h>


#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

// #include "shim/shim_XPLMDataAccess.h"


class GuiDatarefs {
public:

	bool win_open = true;

	//filter options
	bool m_bShowAll = false;
	std::string m_filterString{};


	bool m_init_groups_filter=true;
	std::map<std::string, std::vector<xp_dref*>> m_map_dref_groups;
	std::map<std::string, size_t> m_map_dref_group_consumer_counts;

	std::map<std::string, size_t> m_map_dref_group_item_counts;

	void draw();


private:

	void build_group_filter();

	static bool search_partial(const char* haystack, const char* needle) {
		// sanity checks
		if (!haystack || !needle)
			return false;
		// empty needle ⇒ consider it “found”
		if (*needle == '\0')
			return true;
		return std::strstr(haystack, needle) != nullptr;
	}

};


#endif //GLFW_XP_HOST_GUIDATAREFS_H
