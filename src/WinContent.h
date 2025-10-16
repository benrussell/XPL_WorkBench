//
// Created by Ben on 3/8/2024.
//

#ifndef GLFW_XP_HOST_WINCONTENT_H
#define GLFW_XP_HOST_WINCONTENT_H

#include "AvionicsHost.h"


#include "shim/shim_XPLMDisplay.h"


#include "imgui.h"
#include "imgui_internal.h"
//FIXME: add support for std::string usage.

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include "imfilebrowser.h"

#include <fstream>
#include <nlohmann/json.hpp>


#include <thread>
#include <chrono>
#include <iostream>

#include <sstream>

#include "GuiAvionicsDevice.h"
#include "XPHost.h"
#include "GuiRecentProjects.h"
#include "GuiTextures.h"
#include "GuiTextureInspector.h"
#include "GuiPluginMessages.h"
#include "GuiMemory.h"

#include "GuiShaderTest.h"



// this might be prototype code for a shim windowing layer
class WinRender{
public:
	virtual void draw() = 0;
};


class WinRender_ImGui: WinRender{
public:
	WinRender_ImGui(){

	}

	void draw(){

	}

	//ImGuiContext ctx;
};




struct WinType {
	XPLMCreateWindow_t m_xp_win_params;
	WinRender* m_render;
};




// this contains our master GUI

class WinBox{
public:

	ImGui::FileBrowser fileDialog_Open;
	ImGui::FileBrowser fileDialog_OpenProject;

	std::string m_lastProjectFilename{};

//private:
	GLFWwindow* m_winh;
	ImGuiContext* imguiContext = nullptr;

	// this is a message from dlopen failures and C++ exceptions??
	bool m_displayErrorMessage = false;
	std::string m_sErrorMessage;

	bool m_bDumpTextures = false;
	bool m_bDrawTriangle = false;

	//this has a click handler that we need to setup in ctor
	GuiTextureInspector m_texInspector;

	GuiShaderTest* m_shaderTest; //must be a pointer because static init of shaders will CTD


	WinBox( int width, int height );
	~WinBox();

	void set_xp_choice( const std::string& fname );

	void load_plugin( const std::string& fname );
	void load_project( const std::string& filename );

	void draw_triangle_box();

	void menu_TitlebarMenu();

	void CreateDockSpace();

	//render a grid of textured quads
	void draw_TextureDump();


	virtual void OnDraw();

	//draw the background of the main gui window
	//this will be filled in with an imgui dockable area
	void OnCallDraw();

};


#endif //GLFW_XP_HOST_WINCONTENT_H
