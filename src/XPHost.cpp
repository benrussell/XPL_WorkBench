//
// Created by Ben on 3/8/2024.
//

#include "XPHost.h"

// Set to the active plugin we're about to fire callbacks for.
Plugin* target_plugin{};

GuiDatarefs XPHost::gui_Datarefs;

GuiPlugins XPHost::gui_Plugins;

size_t XPHost::m_plugin_id_ctr=0;
std::vector<Plugin*> XPHost::m_vecPlugins;

std::vector<std::string> XPHost::m_vecRecentProjects;

std::vector<std::string> XPHost::m_vecLog;