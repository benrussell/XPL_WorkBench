//
// Created by Ben on 5/8/2024.
//

#ifndef GLFW_XP_HOST_GUIDATAREFS_H
#define GLFW_XP_HOST_GUIDATAREFS_H
#include <GL/glew.h>

#include "XPHost.h"

#include <string>
#include <vector>

#include "imgui.h"

#include "shim/shim_XPLMDataAccess.h"


class GuiDatarefs {
public:

	bool win_open = true; //FIXME: unify naming across all guis

	void draw();

};


#endif //GLFW_XP_HOST_GUIDATAREFS_H
