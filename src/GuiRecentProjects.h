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

#ifndef XDBG_GUIRECENTPROJECTS_H
#define XDBG_GUIRECENTPROJECTS_H


#include <vector>
#include <string>
#include <sstream>

#include "imgui.h"

#include "HostApp.h"
#include <functional>

class GuiRecentProjects {
public:

	static bool m_bDraw;


//	if( m_bDrawRecent )
	static void draw(
		const std::function<void(std::string)>& cbf_load_project, //lambda
		const std::function<void(std::string)>& cbf_load_plugin //lambda
		);

};


#endif //XDBG_GUIRECENTPROJECTS_H
