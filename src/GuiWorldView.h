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
// Created by Ben on 31/12/2025.
//

#ifndef XPL_WORKBENCH_GUIWORLDVIEW_H
#define XPL_WORKBENCH_GUIWORLDVIEW_H



#include <GL/glew.h>
#include "imgui.h"

#include "imfilebrowser.h"

#include <iostream>

#include "stb_image.h"

#include <gz_shader.h>

#include "XPLM.h"


class GuiWorldView {
public:

	gz::shader* m_shader;

	bool m_FlipVert=true;
	bool m_FlipHoriz=false;

	bool m_bDraw=true;
	int m_showTexId=3; //FIXME: pass from parent scope

	ImGui::FileBrowser fileDialog_OpenImage; //might want this for saving screenshots?

	//whats the save image equiv to this fn?
	GLuint load_image( std::string filename, int tex_id ){
//		GLuint LoadTexture(const char* filename) {
		int width, height, channels;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 4); // Load image with 4 channels (RGBA)

		if (!data) {
			std::cerr << "Failed to load texture: " << filename << std::endl;
			return 0;
		}


//		GLuint textureID;
//		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, tex_id);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(data);

		return tex_id;

	}


	void draw( double dt ) {

		if (m_bDraw) {
			ImGui::SetNextWindowSize(ImVec2(640, 600), ImGuiCond_FirstUseEver);
//			ImGui::Begin("World View", &m_bDraw, ImGuiWindowFlags_MenuBar);
			ImGui::Begin("World View", &m_bDraw);

			if( ImGui::BeginMenuBar() ) {
				if(ImGui::BeginMenu("Aircraft")){
					if(ImGui::MenuItem("About..", nullptr, false, true)) {
				//		m_GuiAbout.win_open = true;
					}
					ImGui::EndMenu();
				}

				if(ImGui::BeginMenu("Plugins")){
					if(ImGui::MenuItem("About..", nullptr, false, true)) {
						//		m_GuiAbout.win_open = true;
					}
					ImGui::EndMenu();
				}


				ImGui::EndMenuBar();
			}




			int m_width = 1024;
			int m_height = 768;

			int m_tex = 3;

//			ImVec2 size = ImVec2( m_width/2, m_height/2 );
			ImVec2 size = ImVec2( m_width, m_height );

			ImVec2 uv0 = ImVec2(0.0f, 1.0f);
			ImVec2 uv1 = ImVec2(1.0f, 0.0f);

			auto imageSize = size;
			// Calculate the position and size of the image and border
			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImVec2 borderSize(imageSize.x + 2, imageSize.y + 2); // 1px border on all sides

			// Draw the border
			ImGui::GetWindowDrawList()->AddRect(
					pos,
					ImVec2(pos.x + borderSize.x, pos.y + borderSize.y),
					IM_COL32(0, 0, 255, 255) // Blue color (RGBA)
			);

			// Draw the image inside the border
			ImGui::SetCursorScreenPos(ImVec2(pos.x + 1, pos.y + 1)); // Offset by 1px to fit within the border

			ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(m_tex)),
						 size, //ImVec2(64, 64), //viewport size
						 uv0,
						 uv1
			);


			bool img_clicked = false;
			if( ImGui::IsItemClicked() ) {
				img_clicked = true;
			}


			// Check if the mouse is over the image
//				if (ImGui::IsItemHovered())
			{
				// Get the mouse position relative to the current window
				ImVec2 mouse_pos = ImGui::GetMousePos();

				// Optionally, you can get the relative position within the image
				ImVec2 image_pos = ImGui::GetItemRectMin(); // Top-left corner of the image
				ImVec2 relative_pos = ImVec2(mouse_pos.x - image_pos.x, mouse_pos.y - image_pos.y);

//					ImGui::Text("Mouse Position: (%.1f, %.1f)", mouse_pos.x, mouse_pos.y);
//					ImGui::Text("Relative Position: (%.1f, %.1f)", relative_pos.x, relative_pos.y);

				float xp_rel_y = size[1] - relative_pos.y;
//					ImGui::Text("XP Position: (%.1f, %.1f)", relative_pos.x, xp_rel_y);

				float x_rat = relative_pos.x / size[0];
				float y_rat = xp_rel_y / size[1];

				ImGui::Text("rat: %0.2f,%0.2f", x_rat, y_rat );

				float emu_x = x_rat * m_width;
				float emu_y = y_rat * m_height;

				ImGui::Text("GL: %0.2f,%0.2f", emu_x, emu_y );

				ImGui::Text("dt: %.3f", dt);


				bool io_want_capture = ImGui::GetIO().WantCaptureMouse;
				const char* f = io_want_capture ? "true" : "false";
				ImGui::Text("wants cap: %s", f);



//				float nvg_y_rat = 1.0 - y_rat;
//				float nvg_x = x_rat * m_width;


				if( img_clicked ) {
					FXPLM_HandleWindowClick(emu_x, emu_y);
				}

			} //get mouse pos



			ImGui::End();

		} // if m_bDraw

	} //draw(..)

};


#endif //XPL_WORKBENCH_GUIWORLDVIEW_H
