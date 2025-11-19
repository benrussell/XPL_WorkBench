//
// Created by Ben on 3/8/2024.
//

#include "HostApp.h"

// Set to the active plugin we're about to fire callbacks for.
Plugin* global_target_plugin{};

GuiDatarefs HostApp::gui_Datarefs;

GuiPlugins HostApp::gui_Plugins;


std::vector<xp_dref*> HostApp::m_dref_pool;


size_t HostApp::m_plugin_id_ctr=0;
#if 0 //FIXME: FXPLM
std::vector<Plugin*> XPHost::m_vecPlugins;
#endif

std::vector<std::string> HostApp::m_vecRecentProjects;
std::vector<std::string> HostApp::m_vecRecentPlugins;
std::vector<std::string> HostApp::m_vecXPlaneFolders;

std::string HostApp::m_active_xp_folder;

std::vector<std::string> HostApp::m_vecLog;

std::vector<std::string> HostApp::m_vecPluginMessages;



Timer HostApp::m_timer;


//FIXME: FXPLM: ported to: size_t dref_factory::search_ctr=0;



float HostApp::fps;



