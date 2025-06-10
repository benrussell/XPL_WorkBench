//
// Created by Ben on 3/8/2024.
//

#ifndef GLFW_XP_HOST_XPHOST_H
#define GLFW_XP_HOST_XPHOST_H

#include <GL/glew.h>

#include <vector>
#include <string>
#include <map>
#include "../xp_sdk_shim/shim_XPLMProcessing.h"
#include "AvionicsHost.h"
#include "GuiAvionicsDevice.h"
#include "GuiDatarefs.h"

class GuiPlugins;

#include "GuiPlugins.h"



#include "../third_party/timer/src/Timer.h"


#include "Plugin.h"
class Plugin;
extern Plugin* global_target_plugin;

class GuiDatarefs;

class XPHost {
public:


	static Timer m_timer;



	static size_t m_plugin_id_ctr;
	static std::vector<Plugin*> m_vecPlugins;

	static GuiDatarefs gui_Datarefs;

	static GuiPlugins gui_Plugins;

	static std::vector<std::string> m_vecRecentProjects;
    static std::vector<std::string> m_vecRecentPlugins;

	static std::vector<std::string> m_vecPluginMessages;

    static std::vector<std::string> m_vecLog;



private:



};


#endif //GLFW_XP_HOST_XPHOST_H
