//
// Created by Ben on 3/8/2024.
//

#ifndef GLFW_XP_HOST_WINCONTENT_H
#define GLFW_XP_HOST_WINCONTENT_H


#include <GL/glew.h> //required
#include "../../glfw/include/GLFW/glfw3.h"


// #include "imgui.h"
#include "imgui_internal.h"

#include "imfilebrowser.h"

#include <fstream>

#include "GuiAvionicsDevice.h"

#include "GuiAbout.h"

#include "HostApp.h"

#include "GuiTextureInspector.h"
#include "GuiShaderTest.h"
#include "GuiPluginLoader.h"
#include "GuiWorldView.h"
#include "GuiGraph.h"


#include "glue_FBO.hpp" //FIXME: port to gz_core




#if 0

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

#endif



// this contains our master GUI

class WinBox{
public:

	ImGui::FileBrowser fileDialog_Open;
	ImGui::FileBrowser fileDialog_OpenProject;

	ImGui::FileBrowser* fileDialog_SaveProject;

	std::string m_lastProjectFilename{};

	std::string m_globalStartupFolder{};

//private:
	GLFWwindow* m_winh;
	ImGuiContext* imguiContext = nullptr;

	bool m_bDumpTextures = false;
	bool m_bDrawTriangle = false;

	//this has a click handler that we need to set up in ctor
	GuiTextureInspector m_texInspector;

	GuiShaderTest* m_shaderTest; //must be a pointer because static init of shaders will CTD

	GuiAvionicsDevice m_GuiAvionicsDevice;

	GuiAbout m_GuiAbout;

	GuiPluginLoader m_pluginLoader;

	GuiWorldView m_GuiWorldView;

	GuiGraph m_GuiGraph;

	gz_fbo* m_fboCanvas;


	int m_fps_cap_sleep_delay = 0;


	XPLMDataRef m_dr_network_time;
	XPLMDataRef m_dr_running_time;
	XPLMDataRef m_dr_frp;

	WinBox( int width, int height );
	virtual ~WinBox();

	void set_xp_choice( const std::string& fname );

	void load_plugin( const std::string& fname );
	void load_project( const std::string& filename );
	void save_project( const std::string& filanem );

	void load_fmod();
	void load_fmod_studio();

	void draw_triangle_box( double dt );

	void menu_TitlebarMenu();
	void menu_File();
	void menu_View();
	void menu_Tools();
	void menu_SimMenus();

	void CreateDockSpace();

	//render a grid of textured quads
	void draw_TextureDump();


	void Display();

	//draw the background of the main gui window
	//this will be filled in with an imgui dockable area
	void OnCallDraw();

};


#endif //GLFW_XP_HOST_WINCONTENT_H
