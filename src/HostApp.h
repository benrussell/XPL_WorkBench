//
// Created by Ben on 3/8/2024.
//

#ifndef GLFW_XP_HOST_XPHOST_H
#define GLFW_XP_HOST_XPHOST_H

#include <GL/glew.h>

#include <vector>
#include <string>
#include <map>
// #include "shim/shim_XPLMProcessing.h"
// #include "AvionicsHost.h"
// #include "GuiAvionicsDevice.h"
#include "GuiDatarefs.h"


class xp_dref;
#include "xp_dref.hpp"


class GuiPlugins;
#include "GuiPlugins.h"

#include "../third_party/timer/src/Timer.h"


// #include "Plugin.h"
//FXPLM: class Plugin;
//FXPLM: extern Plugin* global_target_plugin;

class GuiDatarefs;



class HostApp {
public:

	static float fps;

	inline static Timer m_timer;

	static std::vector<xp_dref*>* m_dref_pool;


	//all loaded plugins
	static size_t m_plugin_id_ctr;
	//FXPLM: static std::vector<Plugin*> m_vecPlugins;

	//datarefs tree view
	static GuiDatarefs gui_Datarefs;

	//plugins tree view
	static GuiPlugins gui_Plugins;

	static std::vector<std::string> m_vecRecentProjects;
    static std::vector<std::string> m_vecRecentPlugins;
	static std::vector<std::string> m_vecXPlaneFolders;

	static std::string m_active_xp_folder;

	//content from XPLM IPC messages?
	static std::vector<std::string> m_vecPluginMessages;

	//general log window for XPLMDebugString?
    static std::vector<std::string>* m_vecLog;


private:


};




#endif //GLFW_XP_HOST_XPHOST_H
