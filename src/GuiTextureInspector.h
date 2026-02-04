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
// Created by Ben on 10/8/2024.
//

#ifndef XDBG_GUITEXTUREINSPECTOR_H
#define XDBG_GUITEXTUREINSPECTOR_H


#include <GL/glew.h>
#include "imgui.h"

#include "imfilebrowser.h"

#include <iostream>

#include "stb_image.h"

#include <gz_shader.h>


class GuiTextureInspector {
public:

	gz::shader* m_shader;

	bool m_FlipVert=false;
	bool m_FlipHoriz=false;

	bool m_bDraw=false;
	int m_showTexId=10;

	ImGui::FileBrowser fileDialog_OpenImage;


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


	void draw() {

		if (m_bDraw) {
			ImGui::SetNextWindowSize(ImVec2(640, 600), ImGuiCond_FirstUseEver);
			ImGui::Begin("Texture Inspector", &m_bDraw);

			ImGui::InputInt("Tex Id", &m_showTexId);

			ImGui::SameLine();
			if(ImGui::Button("Open")){
				fileDialog_OpenImage.SetTitle("Open Image");
				fileDialog_OpenImage.SetTypeFilters({".png",".jpg"});
				fileDialog_OpenImage.Open();
			}

			fileDialog_OpenImage.Display();
			if( fileDialog_OpenImage.HasSelected() ){
				std::string fname = fileDialog_OpenImage.GetSelected();
				std::cout << "Open image: " + fname + "\n";
				fileDialog_OpenImage.ClearSelected();

				load_image( fname, m_showTexId );
			}

			ImGui::SameLine();
			ImGui::Text("flip:");
			ImGui::SameLine();
			ImGui::Checkbox("v", &m_FlipVert);
//			ImGui::SameLine();
//			ImGui::Checkbox("h", &m_FlipHoriz);


			{

				auto cur_pos = ImGui::GetCursorPos();

				// Calculate the size of the child area (10 pixels smaller on each side)
				ImVec2 windowSize = ImGui::GetContentRegionAvail();
				ImVec2 childSize(windowSize.x - 5, windowSize.y - 5);
//				childSize.y -= cur_pos.y;

				// Start a child region within the main window with the calculated size
				ImGui::BeginChild("Child Area", childSize, true);


				auto lam_drawRawImage = [](int tex_id, int width, int height, bool flip_vert) {

					bool draw_tex = false;
					if (glIsTexture(tex_id)) {
						draw_tex = true;
					}


					ImVec2 size(width, height);

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


					if (draw_tex) {
						// textureID is currently bound
						auto my_texture_id = (ImTextureID) (intptr_t) tex_id; // cast your GLuint to ImTextureID

						ImVec2 uv0;
						ImVec2 uv1;
						if( flip_vert ){
							uv0 = ImVec2(0.0f, 1.0f);
							uv1 = ImVec2(1.0f, 0.0f);
						}else{
							uv0 = ImVec2(0.0f, 0.0f);
							uv1 = ImVec2(1.0f, 1.0f);
						}

						ImGui::Image(reinterpret_cast<void *>(static_cast<intptr_t>( tex_id )),
									 size, //ImVec2(64, 64),
									 uv0,
									 uv1
						);

					} else {
						// textureID is not bound
						ImGui::Dummy(size);
					}


					if (ImGui::IsItemClicked()) {
//						if( openImageInspector ){
//							openImageInspector( tex_id );
//						}else{
//							std::string msg = "tex click: no handler: tex_id: " + std::to_string(tex_id);
//							throw std::runtime_error( msg );
//						} //got a handler?
					} //is clicked?

				}; //lam_drawRawImage(..)


				lam_drawRawImage(m_showTexId,
					 		childSize.x - 20, childSize.y - 20, //width,height
							m_FlipVert //flip vert
					 		);

				// End the child region
				ImGui::EndChild();
			}//scope
			ImGui::End();

		} // if m_bDraw

	} //draw(..)

};


#endif //XDBG_GUITEXTUREINSPECTOR_H
