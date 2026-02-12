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
// Created by br on 8/06/25.
//


#include "WinContent.h"

#include <thread> //required


#include <XPLM.h>
#include <XPLMPlugin.h>
#include <XPLMDataAccess.h>

#include "FXPLM.h"

#include "GuiPluginMessages.h"
#include "GuiMemory.h"

#include "GuiRecentProjects.h"
#include "GuiTextures.h"

#include "GuiPluginLoader.h"

#include "GuiXPLMLog.h"

#include "GuiMessageBox.h"


WinBox::WinBox( const int width, const int height ){
    /* Create a windowed mode window and its OpenGL context */
	// create a window that belongs to a member var
	// window created by main.cpp is only used for getting a GL context.
    m_winh = glfwCreateWindow( width, height, "XPL_WorkBench_WinBox", nullptr, nullptr);

	HostApp::m_timer.start();

	fileDialog_SaveProject = new ImGui::FileBrowser(ImGuiFileBrowserFlags_EnterNewFilename);


    if (!m_winh)
    {
        glfwTerminate();
        throw std::runtime_error("xwb/ WinBox/ glfw Window Create Failed.");
    }

    glfwMakeContextCurrent( m_winh );


    m_shaderTest = new GuiShaderTest();


#if 1
    IMGUI_CHECKVERSION();
    imguiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(imguiContext);
    ImGuiIO &io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    //	ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_winh, true);
    ImGui_ImplOpenGL2_Init();

	//added to assist with OS .app package debug
	{
		namespace fs = std::filesystem;
    	try {
    		// report and store the cwd where we loaded from.
    		const std::string folder_now = fs::current_path();
    		std::cout << "xwb/ cwd for font load: " << folder_now << std::endl;
    	} catch (const fs::filesystem_error& e) {
    		std::cerr << "Filesystem error: " << e.what() << std::endl;
    	}
    	catch (const std::exception& e) {
    		std::cerr << "General error: " << e.what() << std::endl;
    	}
	}


    //ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    io.FontDefault = io.Fonts->AddFontFromFileTTF("Resources/fonts/Roboto-Regular.ttf", 16.0f); //FIXME: make this configurable for size?
    if (!io.FontDefault) {
        std::cerr << "Error: Could not load font 'Roboto-Regular.ttf'" << std::endl;
    }
    ImGui_ImplOpenGL2_CreateFontsTexture();

#endif


	m_fboCanvas = new gz_fbo(1024,768);


    //setup the texture inspector on click handler
    //so it opens the tex inspector and focus' the clicked
    //tex_id
    //uses a lambda to capture [this]
    GuiTextures::openImageInspector = [this]( int tex_id ){
        m_texInspector.m_bDraw = true;
        m_texInspector.m_showTexId = tex_id;
    };


    GuiPlugins::openImageInspector = [this]( int tex_id ){
        m_texInspector.m_bDraw = true;
        m_texInspector.m_showTexId = tex_id;
    };


	GuiPlugins::openAvionicsInspector = [this]( AvionicsHost* av_dev ){
		m_GuiAvionicsDevice.m_bDraw = true;
		m_GuiAvionicsDevice.m_dev = av_dev;
	};


	std::cout<<"xwb/ Creating basic host drefs.\n";
	m_dr_network_time = FXPLM_DrefCreate("sim/network/misc/network_time_sec");
	m_dr_running_time = FXPLM_DrefCreate("sim/time/total_running_time_sec");
	m_dr_frp = FXPLM_DrefCreate("sim/operation/misc/frame_rate_period");


    std::cout<<"xwb/ WinBox ctor finished, waiting for user input..\n";
};




WinBox::~WinBox(){

	delete m_fboCanvas;

	delete fileDialog_SaveProject;

	delete m_shaderTest;

    if( m_winh ){

#if 1
        ImGui_ImplOpenGL2_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(imguiContext);
#endif

        glfwDestroyWindow(m_winh);

    }
};


void WinBox::set_xp_choice( const std::string& fname ) {
	std::cout << "xwb/ set_xp_choice: ["<< fname <<"]\n";
	HostApp::m_active_xp_folder = fname;
}


void WinBox::load_plugin( const std::string& fname ){
    std::cout << "xwb/ winbox->load_plugin(" << fname << ")\n";


	auto lam_addToRecentList = []( std::vector<std::string>& items, const std::string& new_fname ) {
		// this snippet will add the project name to our recents list
		bool store_item = true;
		for( const auto& fn: items ){
			if( fn == new_fname ){
				store_item = false;
			}
		}
		if( store_item ){
			items.insert(items.begin(), new_fname);
		}else {
			std::cout<<"    Item already in recents list. Ignored.\n";
		}

	};


#define XWB_ALLOW_LOAD_PLUGIN_TO_CHANGE_FOLDER 0


#if XWB_ALLOW_LOAD_PLUGIN_TO_CHANGE_FOLDER
	const auto cwd_before_plugin = std::filesystem::current_path();

	std::string jump_to_folder = std::filesystem::path(fname).parent_path().string();
	std::filesystem::current_path(jump_to_folder); //disabled

	std::cout << "jumping to folder: " << jump_to_folder << "\n";
#endif


	const int plugin_loaded = FXPLM_LoadPlugin( fname.c_str() );
	if( plugin_loaded ) {
		printf("WinBox::load_plugin(): loaded plugin.\n");

		lam_addToRecentList(
		HostApp::m_vecRecentPlugins,
			fname
			);

	}else {
		const auto err_msg = "load_plugin: " + std::string(FXPLM_LoadPlugin_GetErr());
		GuiMessageBox::m_bDraw = true;
		GuiMessageBox::m_Message = err_msg; //.c_str();
	}

#if XWB_ALLOW_LOAD_PLUGIN_TO_CHANGE_FOLDER
	//jump back to original folder
	std::filesystem::current_path(cwd_before_plugin); //disabled
#endif


	printf("xwb/ resume main loop.\n");
}



void WinBox::save_project( const std::string& filename ){
	std::cout << "xwb/ save_project: [" << filename << "]\n";

	//CHECK THE UNLOAD CODE AND SEE IF IT CALLS PLUGINS THAT DIDNT XPLUGINSTART

	const int plugin_count = XPLMCountPlugins();
	std::cout << "  plugin_count: [" << plugin_count << "]\n";

	if( plugin_count == 0 ){
		ImGui::Text("  No plugins loaded.");
		return;
	}

	nlohmann::json json_arr_plugins = nlohmann::json::array();

	for ( int x=1; x<plugin_count; x++ ) {
		auto p = (Plugin*)FXPLM_PluginInstPtr(x);

		nlohmann::json details;
		details["working_folder"] = p->m_workingFolder;
		details["plugin_file"] = p->m_pluginFilename;

		json_arr_plugins.emplace_back(details);

	} //loop all plugins ctr

	nlohmann::json json;
	json["filetype"] = "x-plugins / XWB project file / v0.1";
	json["readme"] = "FIXME: Allow GUI edit of this.";
	json["plugins"] = json_arr_plugins;


	std::ofstream myfile;
	myfile.open(filename);
	myfile << json.dump(1,'\t');
	myfile.close();

	std::cout << "save_project: wrote json 1643\n";

} //save_project()



void WinBox::load_project( const std::string& filename ){
    std::cout << "xwb/ load_json_prj: [" << filename << "]\n";

    m_lastProjectFilename = filename;

    // this snippet will add the project name to our recents list
    bool store_prj = true;
    for( const auto& fn: HostApp::m_vecRecentProjects ){
        if( fn == filename ){
            store_prj = false;
        }
    }
    if( store_prj ){
        HostApp::m_vecRecentProjects.insert(HostApp::m_vecRecentProjects.begin(), filename);
    }


    //unload our plugins!
    std::cout<<"xwb/ ----- unloading plugins -------\n";
	FXPLM_UnloadPlugins(); //load_project() auto unload

	std::cout<<"load_project fname: [" << filename << "]\n";

    std::ifstream f( filename );
	if (!f) {
		const std::string msg = "load_project fname: [" + filename + "] 404 !\n";
		std::cout<<msg;
		GuiMessageBox::m_bDraw = true; //load project error
		GuiMessageBox::m_Message = msg;
		return;
	}


    nlohmann::json data = nlohmann::json::parse(f);

    for( auto project_plugin: data["plugins"] ){
        std::cout<<"xwb/  project_plugin folder: " << project_plugin["working_folder"] << "\n";
        std::cout<<"xwb/  project_plugin    xpl: " << project_plugin["plugin_file"] << "\n";

        try{
            namespace fs = std::filesystem;
            std::string cwd = fs::current_path();

            //jump to folder, eg, /X-Plane 12/
        	std::cout<<"xwb/ folder change: [" << project_plugin["working_folder"] << "]\n";
            fs::current_path( project_plugin["working_folder"] );

            //load the plugin file, does not need to be in cwd
            //path can be absolute or relative.
            load_plugin( std::string(project_plugin["plugin_file"]) );

            //jump back to folder that XDbg was launched in
        	std::cout<<"xwb/ folder change: [" << cwd << "]\n";
            fs::current_path( cwd );


        }catch (const std::runtime_error& e) {
            std::cerr << "xwb/ Caught a runtime_error: " << e.what() << std::endl;

            GuiMessageBox::m_Message = std::string("load_project: ") + e.what();
			GuiMessageBox::m_bDraw = true;
        }

    } //loop project plugins

} //load_project()



void WinBox::load_fmod() {
	std::cout << "xwb/ load_fmod()\n";

	const std::string fname = "/Users/br/XP12_Libs/X-Plane.app/Contents/Frameworks/libfmod.dylib";

	//works ok on mac
	std::cout<<"xwb/ calling dlopen(" << fname << ")\n";
	std::cout<<"xwb/ --- fmod static init / begin ---\n";
	dlerror(); //clear errors.

	void* dlh = dlopen(fname.c_str(), RTLD_NOW);

	if( dlh == nullptr ){
		std::string sLoadError = dlerror();
		throw std::runtime_error( sLoadError ); //we capture this for GUI display

	}else{
		std::cout<<"xwb/ --- fmod static init / end   ---\n";
		printf("xwb/  loaded dylib; dlh: %p\n", dlh);

	} //dlopen worked

}



void WinBox::load_fmod_studio() {
	std::cout << "xwb/ load_fmod_studio()\n";

	std::string fname = "/Users/br/XP12_Libs/X-Plane.app/Contents/Frameworks/libfmodstudio.dylib";

	//works ok on mac
	std::cout<<"xwb/ calling dlopen(" << fname << ")\n";
	std::cout<<"xwb/ --- fmodstudio static init / begin ---\n";
	dlerror(); //clear errors.

	void* dlh = dlopen(fname.c_str(), RTLD_NOW);

	if( dlh == nullptr ){
		std::string sLoadError = dlerror();
		throw std::runtime_error( sLoadError ); //we capture this for GUI display

	}else{
		std::cout<<"xwb/ --- fmodstudio static init / end   ---\n";
		printf("xwb/  loaded dylib; dlh: %p\n", dlh);

	} //dlopen worked

}





void WinBox::draw_triangle_box( double dt ){
    /* Render here */

    static float r = 0.f;
    r += (30.0f * dt);

    //we want a 100pix base size on the triangle so its easy to see.
    constexpr float base_size = 1000.f;
    constexpr float half = base_size / 2.2f;

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



	void WinBox::menu_File() {
		if(ImGui::BeginMenu("File")){
			if(ImGui::MenuItem("Quick Load Plugin..", nullptr, false, true)){
				fileDialog_Open.SetTitle("Choose X-Plane Plugin");
				fileDialog_Open.SetTypeFilters({ ".xpl",".so",".dylib",".dll" });
				#if 0
				namespace fs = std::filesystem;
				fs::path newCwd = fs::current_path();
				std::cout << "New working directory: " << newCwd << std::endl;
				fileDialog_Open.SetPwd(newCwd);
				#endif
				std::string newCwd = "/Users/br/Downloads/xp/";
				fileDialog_Open.SetPwd(newCwd);
				fileDialog_Open.Open();
			}


			if(ImGui::MenuItem("Load Plugin..", nullptr, false, true)){
#if 0
				fileDialog_Open.SetTitle("Choose X-Plane Plugin");
				fileDialog_Open.SetTypeFilters({ ".xpl",".so",".dylib",".dll" });
#if 0
				namespace fs = std::filesystem;
				fs::path newCwd = fs::current_path();
				std::cout << "New working directory: " << newCwd << std::endl;
				fileDialog_Open.SetPwd(newCwd);
#endif
				std::string newCwd = "/Users/br/Downloads/xp/";
				fileDialog_Open.SetPwd(newCwd);
				fileDialog_Open.Open();
#endif

				m_pluginLoader.m_bDraw = true;


			}



            if( ImGui::BeginMenu("Recent Plugins") ){
                if(HostApp::m_vecRecentPlugins.empty()){
                    ImGui::Separator();
                }

                for( const auto& fn: HostApp::m_vecRecentPlugins ){
                    if( fn != "" ){
                        if(ImGui::MenuItem(fn.c_str())){
                            std::cout<<"menu/file/open recent plugin/[" << fn << "]\n";
                            load_plugin( fn );
                        }
                    }

                }

                ImGui::EndMenu();
            }


            if(ImGui::MenuItem("Unload all Plugins..", nullptr, false, true)){
                //unload our plugins!
                std::cout<<"xwb/ ----- unloading plugins -------\n";
                FXPLM_UnloadPlugins(); //menu item
            }

            ImGui::Separator();


            if(ImGui::MenuItem("Load Project..", nullptr, false, true)){
                fileDialog_OpenProject.SetTitle("Choose Project");
                fileDialog_OpenProject.SetTypeFilters({ ".json",".*",".xwb-json" });
                fileDialog_OpenProject.Open();
            }


			if(ImGui::MenuItem("Save Project..", nullptr, false, true)){
				std::cout << "xwb/ menu/ File/Save Project..\n";
				fileDialog_SaveProject->SetTitle("Save Project As");
				fileDialog_SaveProject->SetTypeFilters({ ".json",".*",".xwb-json" });
				fileDialog_SaveProject->Open();
			}


			if( ImGui::BeginMenu("Recent Projects") ){
				if(HostApp::m_vecRecentProjects.empty()){
					ImGui::Separator();
				}

				for( const auto& fn: HostApp::m_vecRecentProjects ){
					if(ImGui::MenuItem(fn.c_str())){
						std::cout<<"menu/file/open recent/[" << fn << "]\n";
						load_project( fn );
					}
				}

				ImGui::EndMenu();
			}

            if( ImGui::MenuItem("Reload Project", "F5") ){
                if( ! m_lastProjectFilename.empty() ){
                    load_project( m_lastProjectFilename );
                }
            }
            ImGui::Separator();


			if( ImGui::BeginMenu("X-Plane Folder") ){
				if(HostApp::m_vecXPlaneFolders.empty()){
					ImGui::Separator();
				}

				bool bActiveMenu = false;
				for( const auto& fn: HostApp::m_vecXPlaneFolders ){
					if (HostApp::m_active_xp_folder == fn) {
						bActiveMenu = true;
					}else {
						bActiveMenu = false;
					}
					if(
						ImGui::MenuItem(fn.c_str(),
							NULL,
							bActiveMenu
						)
					){
						std::cout<<"menu/file/xplane folder/[" << fn << "]\n";
						set_xp_choice( fn );
					}
				}

				ImGui::EndMenu();
			}


			if( ImGui::MenuItem("Load FMOD") ){
				std::cout<<"menu/file/load fmod\n";
				// set_xp_choice( fn );
				load_fmod();
				load_fmod_studio();
			}


			// if( ImGui::MenuItem("Load XPLM") ){
			// 	std::cout<<"menu/file/load xplm\n";
			// 	// set_xp_choice( fn );
			// 	load_xplm();
			// }
			ImGui::Separator();


			if( ImGui::MenuItem("Exit") ){
				std::cout<<"xwb/ menu/file/exit\n";
				glfwSetWindowShouldClose(this->m_winh, 1);

			}

			ImGui::EndMenu();
		}

	}


	void WinBox::menu_Tools(){
		if(ImGui::BeginMenu("Tools")){

			if(ImGui::BeginMenu("FPS Limit")){
				bool b_Unlimited = false;
				bool b_fps_60 = false;
				bool b_fps_30 = false;

				switch( m_fps_cap_sleep_delay ){
				case 0:
					b_Unlimited = true;
					break;
				case 10:
					b_fps_60 = true;
					break;
				case 20:
					b_fps_30 = true;
					break;
				};

				if(ImGui::MenuItem("No limit", nullptr, b_Unlimited, true)){
					m_fps_cap_sleep_delay = 0;
				}

				if(ImGui::MenuItem("60", nullptr, b_fps_60, true)){
					m_fps_cap_sleep_delay = 10;
				}

				if(ImGui::MenuItem("30", nullptr, b_fps_30, true)){
					m_fps_cap_sleep_delay = 20;
				}

				ImGui::EndMenu();
			}



			if(ImGui::MenuItem("Shader Harness", nullptr, m_shaderTest->m_bDraw, true)){
				m_shaderTest->m_bDraw = ! m_shaderTest->m_bDraw;
			}

			if(ImGui::MenuItem("Texture List", nullptr, GuiTextures::m_bDraw, true)){
				GuiTextures::m_bDraw = ! GuiTextures::m_bDraw;
			}

			if(ImGui::MenuItem("Texture Inspector", nullptr, m_texInspector.m_bDraw, true)){
				m_texInspector.m_bDraw = ! m_texInspector.m_bDraw;
			}

			ImGui::EndMenu();
		}

	}


	void WinBox::menu_View() {
		if(ImGui::BeginMenu("View")){

			if(ImGui::BeginMenu("Avionics Devices")) {

				int av_devs_found = 0;
				const int plugin_count = XPLMCountPlugins();
				for ( int x=0; x<plugin_count; x++ ) {
					auto p = (Plugin*)FXPLM_PluginInstPtr(x);

					if( ! p->m_vecAvionicsHost.empty() ){
						//we might not render the menu loop so we accum the entire av dev count here.
						av_devs_found += p->m_vecAvionicsHost.size();
						if(ImGui::BeginMenu( p->m_pluginSig.c_str() )) {
							for(const auto dev: p->m_vecAvionicsHost ){
								if(ImGui::MenuItem( dev->m_deviceId.c_str() )) {
									this->m_GuiAvionicsDevice.m_dev = dev;
									this->m_GuiAvionicsDevice.m_bDraw = true;
								}
							}
							ImGui::EndMenu();
						}
					}	//loop av_devs
				} //loop all plugins

				if ( av_devs_found == 0 ) {
					ImGui::MenuItem( "No avionics devices" );
				}


				ImGui::EndMenu();
			} //sub menu avionics devics

			if(ImGui::MenuItem("Datarefs", nullptr, HostApp::gui_Datarefs.win_open, true)){
				HostApp::gui_Datarefs.win_open = !HostApp::gui_Datarefs.win_open;
			}

			if(ImGui::MenuItem("Plugins", nullptr, HostApp::gui_Plugins.win_open, true)){
				HostApp::gui_Plugins.win_open = !HostApp::gui_Plugins.win_open;
			}

			if(ImGui::MenuItem("Plugin Messages", nullptr, GuiPluginMessages::m_bDraw, true)){
				GuiPluginMessages::m_bDraw = !GuiPluginMessages::m_bDraw;
			}

			if(ImGui::MenuItem("Recent Projects", nullptr, GuiRecentProjects::m_bDraw, true)){
				GuiRecentProjects::m_bDraw = ! GuiRecentProjects::m_bDraw;
			}

			if(ImGui::MenuItem("Message", nullptr, GuiMessageBox::m_bDraw, true)){
				GuiMessageBox::m_bDraw = ! GuiMessageBox::m_bDraw;
			}

			if(ImGui::MenuItem("XPLMDebugString Log", nullptr, GuiXPLMLog::m_bDraw, true)){
				GuiXPLMLog::m_bDraw = ! GuiXPLMLog::m_bDraw;
			}


			if(ImGui::MenuItem("World View", nullptr, m_GuiWorldView.m_bDraw, true)){
				m_GuiWorldView.m_bDraw = ! m_GuiWorldView.m_bDraw;
			}

			if(ImGui::MenuItem("Graph", nullptr, m_GuiGraph.win_open, true)){
				m_GuiGraph.win_open = ! m_GuiGraph.win_open;
			}



			ImGui::Separator();

			if(ImGui::MenuItem("Dbg Textures", nullptr, m_bDumpTextures, true)){
				m_bDumpTextures = !m_bDumpTextures;
			}
			if(ImGui::MenuItem("Dbg Triangle", nullptr, m_bDrawTriangle, true)){
				m_bDrawTriangle = !m_bDrawTriangle;
			}



			ImGui::EndMenu();
		}

	}


#include "XPLMMenus.h" //FIXME: Move this.
#include "glue_Menus.h"
#include <functional>

void WinBox::menu_SimMenus(){
	if(ImGui::BeginMenu("Sim Menus")){

		static glue_MenuNode* mnu_PluginsRoot = nullptr;
		static glue_MenuNode* mnu_AircraftRoot = nullptr;
		if( ! mnu_PluginsRoot ){
//			std::cout << "xwb/ calling find plugins menu..\n";
			mnu_PluginsRoot = (glue_MenuNode*)XPLMFindPluginsMenu();
//			std::cout << "xwb/ mnu_PluginsRoot &:" << (void*)mnu_PluginsRoot << "\n";

//			std::cout << "xwb/ calling find plugins menu..\n";
			mnu_AircraftRoot = (glue_MenuNode*)XPLMFindAircraftMenu();
//			std::cout << "xwb/ mnu_AircraftRoot &:" << (void*)mnu_AircraftRoot << "\n";
		} //static var init


		using MenuRenderer = std::function<void(glue_MenuNode*)>;
		MenuRenderer lam_renderMenuTree;

		//create lambda and assign to fn_h so it can be passed into the lambda
		lam_renderMenuTree = [&]( glue_MenuNode* mnu_root ) -> void{

			if( mnu_root->m_vecSubNodes.empty() ){

				auto mnu_glue = (glue_MenuNode*)mnu_root->m_parentMenuNode;

				bool render_item = false;
				if( mnu_glue ){
					if( mnu_glue->m_fn_click_handler ){
						render_item = true;
					}
				}

				if( render_item && ImGui::MenuItem(
							mnu_root->m_sLabel.c_str(), 		//label
							nullptr,	//shortcut
							false, 	//selected
							true   	//enabled
					) ){
						//exec click handler
						if( mnu_glue ){
							std::cout << "xwb/ Sim Menu/ [" << mnu_root->m_sLabel << "] / click handler:" << (void*)mnu_glue->m_fn_click_handler << ":";
							std::cout << " inMenuRef:" << mnu_glue->m_refcon;
							std::cout << " inItemRef:" << mnu_root->m_refcon;
							std::cout << "\n";

							void* inMenuRef = mnu_glue->m_refcon;
							void* inItemRef = mnu_root->m_refcon;

							((Plugin*)mnu_glue->m_parent_plugin)->takeContext();
							mnu_glue->m_fn_click_handler( inMenuRef, inItemRef );
							((Plugin*)mnu_glue->m_parent_plugin)->releaseContext();

							std::cout << "xwb/ mnu click handler returned.\n";

						}else{
							std::cout << "xwb/ Sim Menu/ No click handler. ***\n";
						}

					} //create mnu item


			}else{
				if( ImGui::BeginMenu(mnu_root->m_sLabel.c_str()) ) {

					for( const auto& vp_sub_node: mnu_root->m_vecSubNodes ){
						auto mnu_item = (glue_MenuNode*)vp_sub_node;
						lam_renderMenuTree( mnu_item );
					} //loop sub nodes

					ImGui::EndMenu();

				} //created mnu branch
			} //root item check for sub nodes

		};//lam

		lam_renderMenuTree( mnu_AircraftRoot );
		lam_renderMenuTree( mnu_PluginsRoot );

		ImGui::EndMenu(); // sim menus root
	}

}



void WinBox::menu_TitlebarMenu(){
		if( ImGui::BeginMainMenuBar() ){

			menu_File();

			menu_Tools();

			menu_View();

			menu_SimMenus();

			if(ImGui::BeginMenu("Help")){
				if(ImGui::MenuItem("About..", nullptr, false, true)) {
					m_GuiAbout.win_open = true;
				}
				ImGui::EndMenu();
			}


			if (ImGui::IsKeyPressed(ImGuiKey_F5)) {
				// Do something when F5 is pressed
				std::cout << "F5 key was pressed!" << std::endl;
				if( ! m_lastProjectFilename.empty() ){
					load_project( m_lastProjectFilename );
				}
			}


			// spring to force next items to the right
			ImGui::SameLine(ImGui::GetContentRegionAvail().x);

			const float fps = ImGui::GetIO().Framerate;
			ImGui::Text("FPS: %.1f", fps);
			HostApp::fps = fps;

			m_GuiGraph.add_item( fps );


			auto lam_formatTime = [](double totalSeconds) {
				const int hours   = static_cast<int>(totalSeconds / 3600);
				const double rem  = totalSeconds - hours * 3600;
				const int minutes = static_cast<int>(rem / 60);
				const double secs = rem - minutes * 60;

				char buf[32];
				std::snprintf(buf, sizeof(buf), "%02d:%02d:%04.1f",
							  hours, minutes, secs);
				return std::string(buf);
			};

			const std::string runtime = lam_formatTime( HostApp::m_timer.getElapsedTimeInSec() );
			ImGui::Text("RT: %s", runtime.c_str() );

			const double now = HostApp::m_timer.getElapsedTimeInSec();
			static double global_ts = now;
			const double global_dt = now - global_ts;
			ImGui::Text("dT: %0.3f", global_dt );
			global_ts = now;


			ImGui::EndMainMenuBar();
		} //menu bar

	}







void WinBox::CreateDockSpace() {
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::Begin("DockSpace", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
									   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
									   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground |
									   ImGuiWindowFlags_NoBringToFrontOnFocus  |
									   ImGuiWindowFlags_NoDocking);
	ImGui::DockSpace(ImGui::GetID("DockSpace_Root"));

	ImGui::End(); //dock space
}






void WinBox::draw_TextureDump(){
	auto lam_drawBox = [this]( int tex, float size ){

		glColor3f( 1,1,1 );

		glBindTexture(GL_TEXTURE_2D, tex);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(-size, -size);
		glTexCoord2f(1.0f, 0.0f); glVertex2f( size, -size);
		glTexCoord2f(1.0f, 1.0f); glVertex2f( size,  size);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(-size,  size);
		glEnd();

	};

	if( m_bDumpTextures )
	{
		glPushMatrix();

		glEnable(GL_TEXTURE_2D);

		glTranslatef(200, 200, 0.0);
		//glScalef( 0.2, 0.2, 0.2 );

		size_t grid_count = 2;

		constexpr float size = 100.0f;
		const float margin = size + 10.f;
		const float shift = size + margin;
		const float cr = shift * (grid_count * -1.f);
		int tx = 1;
		for (int x = 0; x < grid_count; ++x) {
			for (int y = 0; y < grid_count; ++y) {
				lam_drawBox(tx, size);
				++tx;
				glTranslatef(shift, 0.0, 0.0);
			}
			glTranslatef(cr, 0.0, 0.0);
			glTranslatef(0.f, shift, 0.0);
		}

		glDisable(GL_TEXTURE_2D);

		glPopMatrix();
	}

}






void WinBox::Display(){

    //FPS / CPU usage cap
	if ( m_fps_cap_sleep_delay ) {
		int ms = m_fps_cap_sleep_delay;
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	}

		XPLMSetDataf(m_dr_running_time, HostApp::m_timer.getElapsedTimeInSec());
		XPLMSetDataf(m_dr_network_time, HostApp::m_timer.getElapsedTimeInSec());

		XPLMSetDataf(m_dr_frp, 1.f / HostApp::fps);



//		if( m_bDrawTriangle ){
//			glPushMatrix();
//			//		glTranslatef( 150, 150, 0 );
//			draw_triangle_box(dt);
//			glPopMatrix();
//		}


		// Render all avionics device FBO surfaces.
		// render all draw callbacks
		FXPLM_DrawCBS();



	const double now = HostApp::m_timer.getElapsedTimeInSec();
	static double s_last_draw = now;
	const double dt = now - s_last_draw;
	s_last_draw = now; //HostApp::m_timer.getElapsedTimeInSec();

	// switch to an FBO target so we can
		// render the FXPLM 2D layers
		if( m_fboCanvas ) {
#if 1
			m_fboCanvas->push_fbo();
			glPushMatrix();
			{
				{
					glPushMatrix();
					glTranslatef(128, 128, 0);
					draw_triangle_box(dt);
					glPopMatrix();
				}
				FXPLM_DrawWindows();

	//			draw_TextureDump(); //this draws into the bg of the main host window
			}
			glPopMatrix();
			m_fboCanvas->pop_fbo();
#endif
			//HostApp::gui_Plugins.m_fbo = m_fboCanvas;
		}


	//draws a grid of textures that should give us
		//an FBO debug channel.
		draw_TextureDump(); //this draws into the bg of the main host window







//turn the imgui code on and off easily
#define XPLWB_USE_IMGUI 1
#if XPLWB_USE_IMGUI
		ImGui::SetCurrentContext(imguiContext);

		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		menu_TitlebarMenu();

		//CreateDockSpace();


		fileDialog_OpenProject.Display();
		if( fileDialog_OpenProject.HasSelected() ){
			const std::string fname = fileDialog_OpenProject.GetSelected().string();
			fileDialog_OpenProject.ClearSelected();
			std::cout << "fileDialog_OpenProject: " << fname << std::endl;

			load_project( fname );

		} //deal with plugin open dialog


		fileDialog_Open.Display();
		if( fileDialog_Open.HasSelected() ){
			const std::string fname = fileDialog_Open.GetSelected().string();
			fileDialog_Open.ClearSelected();
			std::cout << "file/open_plugin://[" << fname << "]\n";
			load_plugin( fname );

		} //deal with plugin open dialog


		fileDialog_SaveProject->Display();
		if ( fileDialog_SaveProject->HasSelected() ) {
			const std::string fname = fileDialog_SaveProject->GetSelected().string();
			fileDialog_SaveProject->ClearSelected();

			//std::cout << "file/ save project/ target file["<< fname <<"]\n";
			save_project( fname );
		}


#if 0 //FIXME: FXPLM - av_dev GUI instance loop
		//loop over all avionics-device gui instances and call their imgui draw code.
		for( auto p: XPHost::m_vecPlugins ){
			for( auto gui: p->m_vecGuiAv ){
				gui->draw();
			}
		}
#endif


		GuiMessageBox::draw();

		HostApp::gui_Plugins.draw();

		HostApp::gui_Datarefs.draw();

		m_texInspector.draw(); //FIXME: upgrade: vec of instances

		m_shaderTest->draw(); //FIXME: upgrade: vec of instances

		m_GuiAvionicsDevice.draw();

		m_GuiAbout.draw(m_globalStartupFolder);

		GuiPluginMessages::draw();

		GuiXPLMLog::draw();

		GuiTextures::draw();

		GuiMemory::draw();

		m_GuiGraph.draw();

		m_GuiWorldView.draw(dt);


		if ( m_pluginLoader.m_bDraw ) {
			auto lam_plugin_loader = [this]( std::string filename ){
				load_plugin( filename );
			};

			m_pluginLoader.draw(lam_plugin_loader);

		}

		if( GuiRecentProjects::m_bDraw ){
			auto lam_project_loader = [this]( std::string filename ){
				load_project( filename );
			};

			auto lam_plugin_loader = [this]( std::string filename ){
				load_plugin( filename );
			};

			GuiRecentProjects::draw( lam_project_loader, lam_plugin_loader );
		}


		ImGui::Render();
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

#endif ///imgui render block



//this code is attempting to stuff an XP SDK window inside a glfw canvas
#if XPHO_RENDER_SDK_WIN_CANVAS

		void* raw_params = glfwGetWindowUserPointer( m_winh );
		if( raw_params ){
//			ImGui::Text("Boxes in boxes in boxes.");

			auto params = (XPLMCreateWindow_t*)raw_params;
			if( sizeof(XPLMCreateWindow_t) != params->structSize ){
				std::cout << " params struct size mismatch/unknown\n";

			}else{
				if( params->drawWindowFunc ){
//					ImGui::Text("drawWindowFUnc");
//							draw_triangle();


					glDisable( GL_CULL_FACE );

//					std::cout<<"draw cb addr: " << std::to_string((size_t)params->drawWindowFunc) << "\n";
					(*params->drawWindowFunc)(this,params->refcon);

					glEnable( GL_CULL_FACE );
				}else{
//					ImGui::Text("no drawWIndowFUnc");
				}
			}

		}
#endif


	} //OnDraw()





void WinBox::OnCallDraw(){

	glfwMakeContextCurrent(m_winh);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);  // Set clear color to red
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the color and depth buffer

	glPushMatrix();

	// Get the window size
	int width, height;
	glfwGetWindowSize(m_winh, &width, &height);

	//FIXME: this code to set the viewport px
	// fails on linux. does it really fail?
	{
		// Set up orthographic projection to map OpenGL coordinates to window pixels
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, width, 0, height, -1, 1);

		// Set up model view matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// Apply the scaling transformation
		glScalef(1.0f, 1.0f, 1.0f);
	}


	// render the core of the window area
	Display();


	//viewport reset here?

	glPopMatrix();

	/* Swap front and back buffers */
	glfwSwapBuffers(m_winh);

}

