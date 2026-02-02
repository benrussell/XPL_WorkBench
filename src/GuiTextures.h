//
// Created by Ben on 10/8/2024.
//

#ifndef XDBG_GUITEXTURES_H
#define XDBG_GUITEXTURES_H

#include <vector>
#include <string>
#include <iostream>

#include "imgui.h"


#include <GL/glew.h>

#include <functional>


class GuiTextures {
public:

	static bool m_bDraw;

	static int m_maxTexId;

	//this function is provided by the code that opens the inspector.
	static std::function<void(int)> openImageInspector;

	static void draw(){

		if( m_bDraw ){

			ImGui::SetNextWindowSize( ImVec2(640,600), ImGuiCond_FirstUseEver );
			ImGui::Begin("Textures", &m_bDraw);

			ImGui::InputInt( "Max Tex Id", &m_maxTexId );

			{
				// Calculate the size of the child area (10 pixels smaller on each side)
				ImVec2 windowSize = ImGui::GetContentRegionAvail();
				ImVec2 childSize(windowSize.x - 20, windowSize.y - 20);

				// Start a child region within the main window with the calculated size
				ImGui::BeginChild("Child Area", childSize, true);


				auto lam_drawRawImage = []( int tex_id, int width, int height ){

					bool draw_tex = false;
					if( glIsTexture(tex_id) ){
						draw_tex = true;
					}

					ImVec2 size(width,height);

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
						auto my_texture_id = (ImTextureID)(intptr_t)tex_id; // cast your GLuint to ImTextureID

						ImVec2 uv0 = ImVec2(0.0f, 0.0f);
						ImVec2 uv1 = ImVec2(1.0f, 1.0f);

						ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>( tex_id )),
									 size, //ImVec2(64, 64),
									 uv0,
									 uv1
						);

					} else {
						// textureID is not bound
						ImGui::Dummy(size);
					}


					if( ImGui::IsItemClicked() ){
						if( openImageInspector ){
							openImageInspector( tex_id );
						}else{
							std::string msg = "tex click: no handler: tex_id: " + std::to_string(tex_id);
							throw std::runtime_error( msg );
						} //got a handler?
					} //is clicked?

				}; //lam_drawRawImage(..)


				const int imageSize = 64;

				const float fImageFitCount = childSize.x / (imageSize * 1.05);
				const int fit_count = (int)fImageFitCount;
				//std::cout<<"fit count:" << fit_count << "\n";

				//fImageFitCount
				int imgx=0;
				while( imgx < m_maxTexId ){

					for( int col_i=0; col_i < fit_count-1; ++col_i ){
						lam_drawRawImage( imgx, imageSize, imageSize );
						ImGui::SameLine();
						++imgx;
					}

					lam_drawRawImage( imgx, imageSize, imageSize );
					++imgx;

				} //loop images


				// End the child region
				ImGui::EndChild();
			}
			ImGui::End();
		}

	}

};


#endif //XDBG_GUITEXTURES_H
