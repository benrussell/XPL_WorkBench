//
// Created by Ben on 4/8/2024.
//

#ifndef GLFW_XP_HOST_AVIONICSHOST_H
#define GLFW_XP_HOST_AVIONICSHOST_H

#include <iostream>

#include <GL/glew.h>


#include "imgui.h"
#include "imgui_internal.h"
//FIXME: add support for std::string usage.

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"


#include "../xp_sdk_shim/shim_XPLMDisplay.h"

#include "../third_party/timer/src/Timer.h"


#include "FBO.h"

#include <gz_shader.h>


class AvionicsHost {
public:

	//profiling timer.
	double m_bakeStart_Screen=0;
	double m_bakeStop_Screen=0;
	double m_bakeStart_Bezel=0;
	double m_bakeStop_Bezel=0;
	Timer m_timer;

	XPLMCreateAvionics_t* m_params;

//	// fbo handle
//	GLuint m_fbo=0;
//	GLuint m_rbo=0;
//
//	int m_width=64;
//	int m_height=64;
//
//	GLuint m_tex=0;

	std::string m_deviceName;
	std::string m_deviceId;

//	float m_FboClearColorRGBA[4]={0.5, 0.0, 0.5, 1.0};

	gz_fbo* m_screen_fbo;
	gz_fbo* m_bezel_fbo;

	gz_fbo* m_composite_fbo;


	gz::shader m_shader;


	explicit AvionicsHost( XPLMCreateAvionics_t* p ){

		m_timer.start();

		m_screen_fbo = new gz_fbo(p->screenWidth, p->screenHeight );
		m_bezel_fbo = new gz_fbo(p->bezelWidth, p->bezelHeight, 0.f );

		m_composite_fbo = new gz_fbo(p->bezelWidth, p->bezelHeight);

		m_params = p; //prob need to pass these to fbo?

		m_deviceName = std::string(p->deviceName);
		m_deviceId = std::string(p->deviceID);





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

			const std::string m_sVtxFilename = "/Users/br/Dev/shaders/gfx/CrtSimple_vtx.glsl";
			const std::string m_sFragFilename = "/Users/br/Dev/shaders/gfx/CrtSimple_frag.glsl";

			const std::string sVtxSrc = lam_loadFile( m_sVtxFilename );

			const std::string sFragSrc = lam_loadFile( m_sFragFilename );

//			if( m_shader ){
//				delete m_shader;
//				m_shader = new gz::shader();
//			}

			auto good_shader = m_shader.compileShaders( sVtxSrc.c_str(), sFragSrc.c_str() );

			if( good_shader ){
				std::cout << "good shader.\n";
				m_shader.m_vecErrorLog.push_back("good shader");
			}else{
				std::cout << "bad shader.\n";
				m_shader.m_vecErrorLog.push_back("bad shader:");
			}

		} catch (const std::runtime_error& e) {
			std::string msg = "Runtime error: " + std::string(e.what());
			std::cerr << msg << std::endl;
			m_shader.m_vecErrorLog.push_back(msg);
		} catch (const std::exception& e) {
			std::cerr << "An error occurred: " << e.what() << std::endl;
		}





	}

	~AvionicsHost(){
		std::cout<<"~AvionicsHost()\n";
		std::cout <<" destruct:" << m_deviceId << "\n";

		delete m_bezel_fbo;
		delete m_screen_fbo;
		delete m_composite_fbo;
	}


	void draw_bezel(){
		if( m_params ){

			if( m_params->bezelDrawCallback ){

				XPLMAvionicsBezelCallback_f cb_f;
				cb_f = m_params->bezelDrawCallback;

				//FIXME: ambient light val
				float fAmbientR = 0.5f;
				float fAmbientG = 0.5f;
				float fAmbientB = 0.5f;

				cb_f(
						fAmbientR, fAmbientG, fAmbientB,
						m_params->refcon );

			}

		} //do we have a params struct?

	}


	//this is called by bake but can also be called directly.
	void draw_screen(){

		if( m_params ){

			if( m_params->drawCallback ){
				//std::cout << "draw cb: " << (size_t)m_params->drawCallback << ", refcon:" << (size_t)m_params->refcon  << "\n";

				XPLMAvionicsScreenCallback_f cb_f;
				cb_f = m_params->drawCallback;
				cb_f( m_params->refcon );

			} //do we have a draw cb?

		} //do we have a params struct?

	}


	void draw_composite(){
		// draw two textured quads.



		auto lam_drawTexturedQuad = [](float w, float h, GLuint tex_id) {
			// Enable 2D texture
			glEnable(GL_TEXTURE_2D);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

			glDisable(GL_BLEND);

			// Disable 2D texture
			glDisable(GL_TEXTURE_2D);
		};

		const float sz = 512;

//		m_composite_fbo->m_width

		//if there's no alpha hole then screen_first is pointless.
		bool screen_first = true;

		glPushMatrix();
		if( screen_first ){

			m_shader.enable();
			m_shader.setUniFloat("u_time", m_timer.getElapsedTime());

			glPushMatrix();
				glTranslatef( m_params->screenOffsetX, m_params->screenOffsetY, 0 );
				lam_drawTexturedQuad(m_screen_fbo->m_width, m_screen_fbo->m_height, m_screen_fbo->m_tex );
			glPopMatrix();
			m_shader.disable();

			lam_drawTexturedQuad(m_bezel_fbo->m_width, m_bezel_fbo->m_height, m_bezel_fbo->m_tex );


		}else{
			lam_drawTexturedQuad(m_bezel_fbo->m_width, m_bezel_fbo->m_height, m_bezel_fbo->m_tex );

			glTranslatef( m_params->screenOffsetX, m_params->screenOffsetY, 0 );
			lam_drawTexturedQuad(m_screen_fbo->m_width, m_screen_fbo->m_height, m_screen_fbo->m_tex );
		}
		glPopMatrix();

	}


	void bake(){
		//bake the screen.
		m_bakeStart_Screen = m_timer.getElapsedTimeInMilliSec();
			m_screen_fbo->push_fbo();
				draw_screen();
			m_screen_fbo->pop_fbo();
		m_bakeStop_Screen = m_timer.getElapsedTimeInMilliSec();

		//bake the bezel
		m_bakeStart_Bezel = m_timer.getElapsedTimeInMilliSec();
			m_bezel_fbo->push_fbo();
				draw_bezel();
			m_bezel_fbo->pop_fbo();
		m_bakeStop_Bezel = m_timer.getElapsedTimeInMilliSec();


		//we need a third, to do composition. Ugh.

		m_composite_fbo->push_fbo();
			draw_composite();
		m_composite_fbo->pop_fbo();


	}


private:


	void draw_triangle_avhost(){
		/* Render here */

		glPushMatrix();

		glBegin(GL_TRIANGLES);
		glColor3f( 1.f, 0.2f, 0.f );
		glVertex3f( 0.f, 0.f, 0.f );

		glColor3f( 0.2f, 1.f, 0.f );
		glVertex3f( 100.f, 0.f, 0.f );

		glColor3f( 1.f, 0.2f, 1.f );
		glVertex3f( 50.f, 100.f, 0.f );
		glEnd();

		glPopMatrix();

	}


};


#endif //GLFW_XP_HOST_AVIONICSHOST_H
