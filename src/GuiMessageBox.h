//
// Created by Ben on 3/12/2025.
//

#ifndef GUIMESSAGEBOX_H
#define GUIMESSAGEBOX_H


#include <vector>
#include <string>
#include <sstream>

#include "imgui.h"
#include "imgui_common.h"

#include "HostApp.h"

#include "imfilebrowser.h"


#include <XPLM.h>

class GuiMessageBox {
public:

	static inline bool m_bDraw=false;

	static inline std::string m_Message;

	static void draw() {
		if( ! m_bDraw ) { return; }

		ImGui::SetNextWindowPos(ImVec2(320,30), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(320,240), ImGuiCond_FirstUseEver);
		ImGui::Begin("Message", &m_bDraw);
		{
			ImGui::Indent();
			//ImGui::Text("_");
			// Set the width for wrapping
			const float wrap_width = ImGui::GetWindowWidth() - 30.f;

			//FIXME: msg dialog: put something red on this dialog for attention grab

			ImGui::PushTextWrapPos(wrap_width);
			ImGui::Text("%s", m_Message.c_str());
			ImGui::PopTextWrapPos();
		}
		ImGui::End();

	}

};



#endif //GUIMESSAGEBOX_H
