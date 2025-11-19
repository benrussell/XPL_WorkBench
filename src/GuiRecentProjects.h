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
