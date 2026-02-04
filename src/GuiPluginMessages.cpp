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
// Created by br on 7/06/25.
//

#include "GuiPluginMessages.h"

bool GuiPluginMessages::m_bDraw=true;


size_t GuiPluginMessages::m_msg_sender_plugin_id=0;
size_t GuiPluginMessages::m_msg_target_plugin_id=1;
size_t GuiPluginMessages::m_msg_message=2;
size_t GuiPluginMessages::m_msg_param=3; //refcons etc
