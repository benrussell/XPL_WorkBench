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
// Created by Ben on 6/8/2024.
//

#ifndef XDBG_GUIPLUGINS_H
#define XDBG_GUIPLUGINS_H

#include <GL/glew.h>

#include "HostApp.h"

#include <string>
#include <vector>

#include "imgui.h"

//#include "shim/shim_XPLMDataAccess.h"

#include <glue_Plugin.hpp>

//#include "glue_FBO.hpp"

class GuiPlugins {
public:

	bool win_open=true; //FIXME: rename unify

	static std::function<void(int)> openImageInspector;

	static std::function<void(AvionicsHost*)> openAvionicsInspector;

	void draw();


//	inline static gz_fbo* m_fbo;
};


#endif //XDBG_GUIPLUGINS_H
