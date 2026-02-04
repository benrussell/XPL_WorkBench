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
// Created by Ben on 27/1/2026.
//

#ifndef XPL_WORKBENCH_GUIGRAPH_H
#define XPL_WORKBENCH_GUIGRAPH_H


#include <deque>


class GuiGraph {
public:
	bool win_open = true;

	void draw();

	std::deque<float> dq_window;
	const size_t MAX_SIZE = 10 * 1000;

	void add_item(float val, bool at_tail = true);

};


#endif //XPL_WORKBENCH_GUIGRAPH_H
