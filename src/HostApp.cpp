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
// Created by Ben on 3/8/2024.
//

#include "HostApp.h"

// Set to the active plugin we're about to fire callbacks for.
Plugin* global_target_plugin{};

GuiDatarefs HostApp::gui_Datarefs;

GuiPlugins HostApp::gui_Plugins;


std::vector<xp_dref*>* HostApp::m_dref_pool;


size_t HostApp::m_plugin_id_ctr=0;

std::vector<std::string> HostApp::m_vecRecentProjects;
std::vector<std::string> HostApp::m_vecRecentPlugins;
std::vector<std::string> HostApp::m_vecXPlaneFolders;

std::string HostApp::m_active_xp_folder;

std::vector<std::string>* HostApp::m_vecLog;

std::vector<std::string> HostApp::m_vecPluginMessages;


float HostApp::fps;

