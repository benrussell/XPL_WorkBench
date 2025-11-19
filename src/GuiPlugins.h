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



class GuiPlugins {
public:

	bool win_open=true;

	static std::function<void(int)> openImageInspector;

	static std::function<void(AvionicsHost*)> openAvionicsInspector;

	void draw();

};


#endif //XDBG_GUIPLUGINS_H
