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
// Created by Ben on 4/8/2024.
//

#ifndef GLFW_XP_HOST_GUIAVIONICSDEVICE_H
#define GLFW_XP_HOST_GUIAVIONICSDEVICE_H

#include <iostream>

#include "imgui.h"
#include "imgui_internal.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"

#include <glue_AvionicsHost.hpp>




static int avio_dev_ctr=0;



class GuiAvionicsDevice {
public:

	// bool win_open=true;


	GuiAvionicsDevice(){
//		std::cout<<"GuiAvoinicsDevice()..\n";
		m_randInst = avio_dev_ctr;
		++avio_dev_ctr;
	}

	~GuiAvionicsDevice(){
//		std::cout<<"~GuiAvoinicsDevice()..\n";
		--avio_dev_ctr;
	}




	void draw_screenFBO(){

		if( ! m_dev ){
			//std::cout<<"GuiAvoinicsDevice() m_dev is nullptr, ret!\n";
			return;
		}


		if( m_dev ){
			//ImGui::Text( "Device ID: %s", m_dev->m_deviceId.c_str() );
			ImGui::Text( "Device Name: %s", m_dev->m_deviceName.c_str() );

			double delta = m_dev->m_bakeStop_Screen - m_dev->m_bakeStart_Screen;
			ImGui::Text("bake: %0.2f ms", delta);

			ImGui::SameLine();
			ImGui::Text(" / %0.1f fps", 1000.f/delta);


			ImVec2 size = ImVec2( m_dev->m_screen_fbo->m_width/2, m_dev->m_screen_fbo->m_height/2 );

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

			ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(m_dev->m_screen_fbo->m_tex)),
						 size, //ImVec2(64, 64),
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

				float emu_x = x_rat * m_dev->m_screen_fbo->m_width;
				float emu_y = y_rat * m_dev->m_screen_fbo->m_height;

				ImGui::Text("GL: %0.2f,%0.2f", emu_x, emu_y );

				float nvg_y_rat = 1.0 - y_rat;
				float nvg_x = x_rat * m_dev->m_screen_fbo->m_width;
				float nvg_y = nvg_y_rat * m_dev->m_screen_fbo->m_height;

				ImGui::Text("nvg: %0.2f,%0.2f", nvg_x, nvg_y );


				if( img_clicked ){
					//std::cout<<"send click:" << emu_x << "," << emu_y << "\n";

					if( m_dev->m_params->screenTouchCallback ){
						//std::cout<<"there is a touch callback.\n";

						XPLMAvionicsMouse_f click_cb_f = m_dev->m_params->screenTouchCallback;
						int foo = click_cb_f( emu_x, emu_y, xplm_MouseDown, m_dev->m_params->refcon );

					}else{
						std::cout<<"there is NO touch callback.\n";
					}

				}


			}


		}else{
			ImGui::Text("Invalid device ptr.");
		}


	} //draw_screenFBO()



	void draw_bezel(){

		if( ! m_dev ){
			//std::cout<<"GuiAvoinicsDevice() m_dev is nullptr, ret!\n";
			return;
		}


		ImVec2 size = ImVec2( m_dev->m_bezel_fbo->m_width/2, m_dev->m_bezel_fbo->m_height/2 );

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

		ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(m_dev->m_bezel_fbo->m_tex)),
					 size, //ImVec2(64, 64),
					 uv0,
					 uv1
		);
	}





	void draw_composite(){

		if( ! m_dev ){
			//std::cout<<"GuiAvoinicsDevice() m_dev is nullptr, ret!\n";
			return;
		}

		int w=m_dev->m_composite_fbo->m_width/2;
		int h=m_dev->m_composite_fbo->m_height/2;

		if ( m_dev->m_screen_fbo->m_width/2 > w) {
			w = m_dev->m_screen_fbo->m_width/2;
		}

		if ( m_dev->m_screen_fbo->m_height/2 > h) {
			h = m_dev->m_screen_fbo->m_height/2;
		}


		//ImVec2 size = ImVec2( m_dev->m_composite_fbo->m_width/2, m_dev->m_composite_fbo->m_height/2 );
		ImVec2 size = ImVec2( w,h );


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

		ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(m_dev->m_composite_fbo->m_tex)),
					 size, //ImVec2(64, 64),
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

			float emu_x = x_rat * m_dev->m_composite_fbo->m_width;
			float emu_y = y_rat * m_dev->m_composite_fbo->m_height;

			ImGui::Text("GL: %0.2f,%0.2f", emu_x, emu_y );

			float nvg_y_rat = 1.0 - y_rat;
			float nvg_x = x_rat * m_dev->m_composite_fbo->m_width;
			float nvg_y = nvg_y_rat * m_dev->m_composite_fbo->m_height;

			ImGui::Text("nvg: %0.2f,%0.2f", nvg_x, nvg_y );


			// we'll do a pixel perfect click test on the bezel later..

			float fScreenX_GL = emu_x - m_dev->m_params->screenOffsetX;
			float fScreenY_GL = emu_y - m_dev->m_params->screenOffsetY;

			bool oob_screen = false;
			if( fScreenX_GL < 0.0 || fScreenY_GL < 0.0 ){ oob_screen=true; }
			if( fScreenX_GL > (float)m_dev->m_params->screenWidth || fScreenY_GL > (float)m_dev->m_params->screenHeight ){ oob_screen=true; }

			if( ! oob_screen ){
				fScreenX_GL = std::max( 0.f, fScreenX_GL );
				fScreenY_GL = std::max( 0.f, fScreenY_GL );

				fScreenX_GL = std::min( fScreenX_GL, (float)m_dev->m_params->screenWidth );
				fScreenY_GL = std::min( fScreenY_GL, (float)m_dev->m_params->screenHeight );

				ImGui::Text("screen GL: %0.2f,%0.2f", fScreenX_GL, fScreenY_GL );

				float nvg_screen_y = m_dev->m_params->screenHeight - fScreenY_GL;
				ImGui::Text("screen nvg: %0.2f,%0.2f", fScreenX_GL, nvg_screen_y );

			}else{
				ImGui::Text("oob screen");
			}




			auto lam_hitPixel = []( int mouseX, int mouseY, int fbo_read, int fbo_ret ) -> bool{ //FIXME: move to gz_core
				// Read the pixel at the mouse position
				GLubyte alpha;
				glBindFramebuffer(GL_FRAMEBUFFER, fbo_read);
				glReadPixels(mouseX, mouseY, 1, 1, GL_ALPHA, GL_UNSIGNED_BYTE, &alpha);
				glBindFramebuffer(GL_FRAMEBUFFER, fbo_ret);

				// Step 3: Determine if the click is valid
				if (alpha > 0) {
					// The click was on a valid area
					return true;
				} else {
					// The click was on a transparent area
					return false;
				}
			};



			if( img_clicked ){

				bool bezel_click = false;

				if( oob_screen ){
					bezel_click = true;
				}else{
					//pixel read click check to see if we hit the bezel texture.

					bool hit_pixel = lam_hitPixel( emu_x, emu_y, m_dev->m_bezel_fbo->m_fbo, m_dev->m_composite_fbo->m_fbo );
					if( hit_pixel ){
						bezel_click = true;
					}

				}

				if( bezel_click ){
					//we hit the bezel
					if( m_dev->m_params->bezelClickCallback ){
						XPLMAvionicsMouse_f click_cb_f = m_dev->m_params->bezelClickCallback;
						int foo = click_cb_f( emu_x, emu_y, xplm_MouseDown, m_dev->m_params->refcon );
					}

				}else{
					//we hit the screen!
					if( m_dev->m_params->screenTouchCallback ){
						XPLMAvionicsMouse_f click_cb_f = m_dev->m_params->screenTouchCallback;
						int foo = click_cb_f( fScreenX_GL, fScreenY_GL, xplm_MouseDown, m_dev->m_params->refcon );
					}

				} //screen or bezel click?

			} //did the user click the imgui img element?

		} //scope hack


	} //draw_composite()



	void draw(){

		if ( ! m_bDraw ) {
			return;
		}

		std::string device_id = "NULL_DEV";
		if ( m_dev ) {
			device_id = m_dev->m_deviceId;
		}

		const std::string devDesc = "Avionics Device:[" + device_id + "]##";
		ImGui::Begin(devDesc.c_str(), &m_bDraw);


		if (ImGui::BeginTabBar("MyTabBar")) {

			if (ImGui::BeginTabItem("Composite")) {
				draw_composite();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Screen")) {
				//ImGui::Text("This is the content of Tab 1");
				draw_screenFBO();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Bezel")) {
				//ImGui::Text("This is the content of Tab 2");
				draw_bezel();
				ImGui::EndTabItem();
			}

			// End the tab bar
			ImGui::EndTabBar();
		}


		ImGui::End();

	}


	size_t m_randInst;

	bool m_bDraw=false;

	AvionicsHost* m_dev;

	// Plugin* m_plugin;

};


#endif //GLFW_XP_HOST_GUIAVIONICSDEVICE_H
