//
// Created by Ben on 3/8/2024.
//

#include "XPHost.h"

// Set to the active plugin we're about to fire callbacks for.
Plugin* global_target_plugin{};

GuiDatarefs XPHost::gui_Datarefs;

GuiPlugins XPHost::gui_Plugins;


std::vector<xp_dref*> XPHost::m_dref_pool;


size_t XPHost::m_plugin_id_ctr=0;
#if 0 //FIXME: FXPLM
std::vector<Plugin*> XPHost::m_vecPlugins;
#endif

std::vector<std::string> XPHost::m_vecRecentProjects;
std::vector<std::string> XPHost::m_vecRecentPlugins;
std::vector<std::string> XPHost::m_vecXPlaneFolders;

std::string XPHost::m_active_xp_folder;

std::vector<std::string> XPHost::m_vecLog;

std::vector<std::string> XPHost::m_vecPluginMessages;



Timer XPHost::m_timer;


size_t dref_factory::search_ctr=0;



float XPHost::fps;



