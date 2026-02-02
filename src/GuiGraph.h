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
