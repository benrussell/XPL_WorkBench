//
// Created by Ben on 6/8/2024.
//

#ifndef XDBG_GUIPLUGINS_H
#define XDBG_GUIPLUGINS_H

#include <GL/glew.h>

#include "XPHost.h"

#include <string>
#include <vector>

#include "imgui.h"

//#include "xp_sdk_shim/shim_XPLMDataAccess.h"


class GuiPlugins {
public:

	bool win_open=true;

	static std::function<void(int)> openImageInspector;

	void draw();

};


#endif //XDBG_GUIPLUGINS_H
