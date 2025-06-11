//
// Created by Ben on 12/8/2024.
//

#ifndef XDBG_FBO_H
#define XDBG_FBO_H


#include <iostream>

#include <GL/glew.h>


#include "imgui.h"
#include "imgui_internal.h"
//FIXME: add support for std::string usage.

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"


#include "shim/shim_XPLMDisplay.h"

#include "../third_party/timer/src/Timer.h"


class gz_fbo{
public:

	// fbo handle
	GLuint m_fbo=0;
	GLuint m_rbo=0;

	int m_width=64;
	int m_height=64;

	GLuint m_tex=0;


	float m_FboClearColorRGBA[4]; //={0.5, 0.0, 0.5, 1.0}; // Randomized in constructor



	gz_fbo(const int width, const int height, float fAlpha=1.0f ){
		std::cout<<"gz_fbo constructor()\n";

		m_rbo = 0;
		m_fbo = 0;

		m_width = width;
		m_height = height;

		m_FboClearColorRGBA[0] = static_cast<float>(rand()) / RAND_MAX;
		m_FboClearColorRGBA[1] = static_cast<float>(rand()) / RAND_MAX;
		m_FboClearColorRGBA[2] = static_cast<float>(rand()) / RAND_MAX;
		m_FboClearColorRGBA[3] = fAlpha;


		m_fbo = createFBO();
		m_tex = createTextureAttachment(width, height);
		m_rbo = createRenderbufferAttachment(width, height);
		checkFBOCompleteness();

		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo); // Bind the FBO
//		glClearColor(1.0f, 0.0f, 1.0f, 1.0f);  // Set clear color to red
		glClearColor(
				m_FboClearColorRGBA[0],
				m_FboClearColorRGBA[1],
				m_FboClearColorRGBA[2],
				m_FboClearColorRGBA[3]
		);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the color and depth buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the FBO

	};


	~gz_fbo(){
		std::cout << "~AvionicsHost_FBO()\n";

		//release depth buffer resources.
		glDeleteRenderbuffersEXT( 1, &m_rbo );

		//release texture buffer resources.
		glDeleteFramebuffersEXT( 1, &m_fbo );

	};



	void push_fbo() const{
//		std::cout<<"push fbo\n";

		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo); // Bind the FBO

		glClearColor(
				m_FboClearColorRGBA[0],
				m_FboClearColorRGBA[1],
				m_FboClearColorRGBA[2],
				m_FboClearColorRGBA[3]
		);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // Clear the color and depth buffer


#if 1
		glPushAttrib(GL_VIEWPORT_BIT);
		glViewport(0,0,m_width, m_height);
		glMatrixMode (GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0,m_width, 0, m_height, 0, 1);
		glMatrixMode (GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
#endif

	};

	//dont make this static.
	void pop_fbo() const{

#if 1
		glPopMatrix();
		glMatrixMode (GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopAttrib();
#endif
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
//		gz::gfx::xp::load_fbo_h();

//		std::cout<<"pop fbo\n";
	};



private:

	const char* GetGLErrorText(GLenum error) {
		switch (error) {
			case GL_NO_ERROR:
				return "No error";
			case GL_INVALID_ENUM:
				return "Invalid enum";
			case GL_INVALID_VALUE:
				return "Invalid value";
			case GL_INVALID_OPERATION:
				return "Invalid operation";
			case GL_STACK_OVERFLOW:
				return "Stack overflow";
			case GL_STACK_UNDERFLOW:
				return "Stack underflow";
			case GL_OUT_OF_MEMORY:
				return "Out of memory";
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				return "Invalid framebuffer operation";
			default:
				return "Unknown error";
		}
	}


	void CheckGLError() {
		GLenum error = glGetError();
		while (error != GL_NO_ERROR) {
			std::cerr << "OpenGL Error: " << GetGLErrorText(error) << std::endl;
			error = glGetError();
		}
	}





	GLuint createFBO() {
		std::cout<<"createFBO()..\n";

		if( ! glGenFramebuffers ){
			throw std::runtime_error("GL is not init.");
		}

		GLuint fbo;
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		return fbo;
	}

	GLuint createTextureAttachment(int width, int height) {
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
		return texture;
	}

	GLuint createRenderbufferAttachment(int width, int height) {
		GLuint rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		return rbo;
	}

	void checkFBOCompleteness() {
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "Framebuffer is not complete!" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

};







#endif //XDBG_FBO_H
