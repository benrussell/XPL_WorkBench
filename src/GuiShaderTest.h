//
// Created by Ben on 12/8/2024.
//

#ifndef XDBG_GUISHADERTEST_H
#define XDBG_GUISHADERTEST_H

#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include <fstream>

#include <nlohmann/json.hpp>

#include "imgui_common.h"

#include <gz_shader.h>

#include "glue_FBO.hpp" //FIXME: port to gz_core


class GuiShaderTest {
private:

public:

	gz::shader* m_shader;

	gz_fbo* m_fboCanvas;


	bool m_bDraw=false;

	std::string m_shaderFolder = "/Users/br/Dev/shaders/gfx/";
	std::string m_shaderName = "CrtSimple";

//	std::string m_sVtxFilename = "/Users/br/Dev/shaders/gfx/vert_passthrough.glsl";
//	std::string m_sFragFilename = "/Users/br/Dev/shaders/gfx/frag_grayscale.glsl";

	int m_showTexId=1;

	bool m_applyShader=true;

	GuiShaderTest(){
		m_shader = new gz::shader();
		m_fboCanvas = new gz_fbo( 400,300 );

	}

	~GuiShaderTest(){
		delete m_fboCanvas;
		delete m_shader;
	}



	void load_shaders(){

		auto lam_loadFile = [](const std::string& filename) -> std::string {
			std::ifstream file(filename);
			if (!file.is_open()) {
				throw std::runtime_error("Could not open file: " + filename);
			}

			std::stringstream buffer;
			buffer << file.rdbuf();
			return buffer.str();
		};



		try {
//			std::string content = lam_loadFile("example.txt");
//			std::cout << content << std::endl;


			std::string sVtxFilename = m_shaderFolder + m_shaderName + "_vtx.glsl";
			std::string sFragFilename = m_shaderFolder + m_shaderName + "_frag.glsl";

			const std::string sVtxSrc = lam_loadFile( sVtxFilename );

			const std::string sFragSrc = lam_loadFile( sFragFilename );

			if( m_shader ){
				delete m_shader;
				m_shader = new gz::shader();
			}

			auto good_shader = m_shader->compileShaders( sVtxSrc.c_str(), sFragSrc.c_str() );

			if( good_shader ){
				std::cout << "good shader.\n";
				m_shader->m_vecErrorLog.push_back("good shader");
			}else{
				std::cout << "bad shader.\n";
				m_shader->m_vecErrorLog.push_back("bad shader:");
			}

		} catch (const std::runtime_error& e) {
			std::string msg = "Runtime error: " + std::string(e.what());
			std::cerr << msg << std::endl;
			m_shader->m_vecErrorLog.push_back(msg);
		} catch (const std::exception& e) {
			std::cerr << "An error occurred: " << e.what() << std::endl;
		}


	}



	void draw(){

		if (m_bDraw) {
			ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
			ImGui::Begin("Shader Harness", &m_bDraw);

			ImGui::InputText("folder", &m_shaderFolder);
			ImGui::InputText("shader", &m_shaderName);

			if( ImGui::Button("Load") ){
				load_shaders(); //FIXME: take fnames as args
			}


			ImGui::InputInt("sample tex", &m_showTexId);
			ImGui::SameLine();
			ImGui::Checkbox("apply", &m_applyShader);


			{

				auto cur_pos = ImGui::GetCursorPos();

				// Calculate the size of the child area (10 pixels smaller on each side)
				ImVec2 windowSize = ImGui::GetContentRegionAvail();
				ImVec2 childSize(windowSize.x - 5, windowSize.y - 5);
//				childSize.y -= cur_pos.y;

				childSize = ImVec2(400,300);

				// Start a child region within the main window with the calculated size
				//ImGui::BeginChild("Child Area", childSize, true);

				auto lam_drawRawImage = [](int tex_id, int width, int height) {

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

						ImVec2 uv0 = ImVec2(0.0f, 0.0f);
						ImVec2 uv1 = ImVec2(1.0f, 1.0f);

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

				lam_drawRawImage( m_fboCanvas->m_tex, childSize.x, childSize.y);

				// End the child region
				//ImGui::EndChild();
			}//scope

			ImGui::End(); //end of shader canvas



			ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
			ImGui::Begin("Shader Log");

			if( ImGui::Button("Clear Log") ){
				m_shader->m_vecErrorLog.clear();
			}
			for( auto msg: m_shader->m_vecErrorLog ){
				ImGui::TextUnformatted( msg.c_str() );
			}

			ImGui::End();









			auto lam_drawTexturedQuad = [](float w, float h, GLuint tex_id) {

				// Enable 2D texture
				glEnable(GL_TEXTURE_2D);

				// Bind the texture
				glBindTexture(GL_TEXTURE_2D, tex_id);

				// Draw the quad
				glBegin(GL_QUADS);

				// Bottom-left corner
				glTexCoord2f(0.0f, 0.0f);
				glVertex2f(0.0f, 0.0f);

				// Bottom-right corner
				glTexCoord2f(1.0f, 0.0f);
				glVertex2f(w, 0.0f);

				// Top-right corner
				glTexCoord2f(1.0f, 1.0f);
				glVertex2f(w, h);

				// Top-left corner
				glTexCoord2f(0.0f, 1.0f);
				glVertex2f(0.0f, h);

				glEnd();

				// Unbind the texture
				glBindTexture(GL_TEXTURE_2D, 0);

				// Disable 2D texture
				glDisable(GL_TEXTURE_2D);
			};



			// do we need another fbo? o_O

			if( m_fboCanvas ){
				m_fboCanvas->push_fbo();

				const float w = m_fboCanvas->m_width;
				const float h = m_fboCanvas->m_height;

				if( m_applyShader ){
					m_shader->enable();

					lam_drawTexturedQuad( w,h, m_showTexId );
//					draw_triangle();

					m_shader->disable();
				}else{
					lam_drawTexturedQuad( w,h, m_showTexId );
//					draw_triangle();
				}
				m_fboCanvas->pop_fbo();
			}else{
//				std::cerr<<"fbo canvas failure.\n";
			}



		} //do we draw? window hidden?

	} //draw()



	void draw_triangle(){
		/* Render here */

		static float r = 0.f;
		r+=0.2f;

		//we want a 100pix base size on the triangle so its easy to see.
		const float base_size = 200.f;
		const float half = base_size / 2.f;

		glPushMatrix();

		glTranslatef( half/2, 0, 0 );
		glTranslatef( half/2, half/2, 0 );
		//glTranslatef( -half, -half, 0 );
		glRotatef( r, 0,0,-1 );

		glBegin(GL_TRIANGLES);
		glColor3f( 1.f, 0.f, 0.f );
		glVertex3f( -half, -half, 0.f );

		glColor3f( 0.f, 1.f, 0.f );
		glVertex3f( half, -half, 0.f );

		glColor3f( 0.f, 0.f, 1.f );
		glVertex3f( 0.f, half, 0.f );
		glEnd();

		glPopMatrix();

	}

};


#endif //XDBG_GUISHADERTEST_H
