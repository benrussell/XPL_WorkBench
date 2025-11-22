//
// Created by br on 8/06/25.
//


#include "WinContent.h"

#include <XPLMPlugin.h>

#include "FXPLM.h"


WinBox::WinBox( const int width, const int height ){
    /* Create a windowed mode window and its OpenGL context */
	// create a window that belongs to a member var
	// window created by main.cpp is only used for getting a GL context.
    m_winh = glfwCreateWindow( width, height, "XPL_WorkBench", nullptr, nullptr);

	HostApp::m_timer.start();


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
    io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/Roboto-Regular.ttf", 16.0f); //FIXME: make this configurable for size?
    if (!io.FontDefault) {
        std::cerr << "Error: Could not load font 'Roboto-Regular.ttf'" << std::endl;
    }
    ImGui_ImplOpenGL2_CreateFontsTexture();

#endif


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



	std::cout<<"xwb/ WinBox ctor finished, waiting for user input..\n";
};




WinBox::~WinBox(){
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

#if 0
	const auto cwd_before_plugin = std::filesystem::current_path();

	std::string jump_to_folder = std::filesystem::path(fname).parent_path().string();
	std::filesystem::current_path(jump_to_folder);

	std::cout << "jumping to folder: " << jump_to_folder << "\n";
#endif

	const int plugin_loaded = FXPLM::call_load_plugin( fname.c_str() );
	if( plugin_loaded ) {
		HostApp::m_vecRecentPlugins.insert(HostApp::m_vecRecentPlugins.begin(), fname);
		printf("\nWinBox::load_plugin(): loaded plugin.\n");
	}

#if 0
	//jump back to original folder
	std::filesystem::current_path(cwd_before_plugin);
#endif

}




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
	FXPLM_UnloadPlugins();


    std::ifstream f( filename );
    nlohmann::json data = nlohmann::json::parse(f);

    for( auto project_plugin: data["plugins"] ){
        std::cout<<"xwb/  project_plugin folder: " << project_plugin["working_folder"] << "\n";
        std::cout<<"xwb/  project_plugin    xpl: " << project_plugin["plugin_file"] << "\n";

        try{
            namespace fs = std::filesystem;
            std::string cwd = fs::current_path();

            //jump to folder, eg, /X-Plane 12/
            fs::current_path( project_plugin["working_folder"] ); //FIXME: load_project(); this wont work, load_plugin() sets the wd

            //load the plugin file, does not need to be in cwd
            //path can be absolute or relative.
            load_plugin( std::string(project_plugin["plugin_file"]) );

            //jump back to folder that XDbg was launched in
            fs::current_path( cwd );


        }catch (const std::runtime_error& e) {
            std::cerr << "xwb/ Caught a runtime_error: " << e.what() << std::endl;
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





void WinBox::draw_triangle_box(){
    /* Render here */

    static float r = 0.f;
    r+=0.2f;

    //we want a 100pix base size on the triangle so its easy to see.
    constexpr float base_size = 1000.f;
    constexpr float half = base_size / 2.f;

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





	void WinBox::menu_TitlebarMenu(){
		if( ImGui::BeginMainMenuBar() ){
			if(ImGui::BeginMenu("File")){
				if(ImGui::MenuItem("Load Plugin..", nullptr, false, true)){
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
                	FXPLM_UnloadPlugins();
                }

                ImGui::Separator();


                if(ImGui::MenuItem("Load Project..", nullptr, false, true)){
                    fileDialog_OpenProject.SetTitle("Choose Project");
                    fileDialog_OpenProject.SetTypeFilters({ ".json",".*",".xwb-json" });
                    fileDialog_OpenProject.Open();
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

			if(ImGui::BeginMenu("Tools")){

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


			if(ImGui::BeginMenu("View")){

				if(ImGui::BeginMenu("Avionics Devices")) {
					ImGui::MenuItem("FXPLM Fix needed.");

#if 0 //FIXME: FXPLM - av_dev menu list

					if( XPHost::m_vecPlugins.empty() ){
						ImGui::MenuItem("No plugins.");
					}

					for( const auto p: XPHost::m_vecPlugins ){
						if( ! p->m_vecAvionicsHost.empty() ){
							if(ImGui::BeginMenu( p->m_pluginName.c_str() )) {
								for(const auto dev: p->m_vecAvionicsHost ){
									//FIXME: needs click handler to show window for dev
									ImGui::MenuItem( dev->m_deviceId.c_str() );
								}
								ImGui::EndMenu();
							}
						}
					}
#endif
					ImGui::EndMenu();
				}

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

				if(ImGui::MenuItem("Message", nullptr, m_displayErrorMessage, true)){
					m_displayErrorMessage = ! m_displayErrorMessage;
				}

				if(ImGui::MenuItem("XPLMDebugString Log", nullptr, HostApp::gui_Plugins.win_open, true)){
					HostApp::gui_Plugins.win_open = !HostApp::gui_Plugins.win_open;
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


			if(ImGui::BeginMenu("Help")){
				ImGui::MenuItem("About..", nullptr, false, true);
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






void WinBox::OnDraw(){

#if 0  //FPS / CPU usage cap
		int ms = 10;
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
#endif


		if( m_bDrawTriangle ){
			glPushMatrix();
			//		glTranslatef( 150, 150, 0 );
			draw_triangle_box();
			glPopMatrix();
		}


		// Render all avionics device FBO surfaces.
		// render all draw callbacks
		FXPLM::call_draw_cbs();


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


#if 0 //FIXME: FXPLM - av_dev GUI instance loop
		//loop over all avionics-device gui instances and call their imgui draw code.
		for( auto p: XPHost::m_vecPlugins ){
			for( auto gui: p->m_vecGuiAv ){
				gui->draw();
			}
		}
#endif


		//show an error message box, probably from trying to load a bad plugin or
		//a plugin that needs more of the XP SDK than we offer.
		if(m_displayErrorMessage){
			ImGui::SetNextWindowPos(ImVec2(320,30), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(320,240), ImGuiCond_FirstUseEver);
			ImGui::Begin("Message", &m_displayErrorMessage);
			{
				ImGui::Indent();
				ImGui::Text("_");
				// Set the width for wrapping
				const float wrap_width = ImGui::GetWindowWidth() - 30.f;

				//FIXME: msg dialog: put something red on this dialog for attention grab

				ImGui::PushTextWrapPos(wrap_width);
				ImGui::Text("%s", m_sErrorMessage.c_str());
				ImGui::PopTextWrapPos();
			}
			ImGui::End();
		}


		HostApp::gui_Plugins.draw();

		HostApp::gui_Datarefs.draw();

		m_texInspector.draw(); //FIXME: upgrade: vec of instances

		if( m_shaderTest ){
			m_shaderTest->draw(); //FIXME: upgrade: vec of instances
		}

		// this->m_GuiAvionicsDevice
		this->m_GuiAvionicsDevice.draw();


		static bool bDrawLog=true;
		if( bDrawLog ){

			ImGui::SetNextWindowSize( ImVec2(320,240), ImGuiCond_FirstUseEver );
			ImGui::Begin("XPLMDebugString", &bDrawLog);
			{
				if( ImGui::Button("Clear") ){
					HostApp::m_vecLog.clear();
				}

				// Calculate the size of the child area (10 pixels smaller on each side)
				ImVec2 windowSize = ImGui::GetContentRegionAvail();
				ImVec2 childSize(windowSize.x - 20, windowSize.y - 20);

				// Start a child region within the main window with the calculated size
				ImGui::BeginChild("Child Area", childSize, true);

				// Add three text elements inside the child area
				size_t x=0;
				if( HostApp::m_vecLog.size() > 50 ){
					x = HostApp::m_vecLog.size() - 50;
				}

				//FIXME:GUI XPLMDebugString log:  pull line wrap from error message dialog.
				for( ; x < HostApp::m_vecLog.size(); ++x ){
					const auto& line = HostApp::m_vecLog[x];
					const std::string log = std::to_string(x) + ":" + line;
					ImGui::TextUnformatted( log.c_str() );
				}


				// Automatically scroll to the last item
				if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
					ImGui::SetScrollHereY(1.0f);  // Scroll to bottom
				}

				// End the child region
				ImGui::EndChild();
			}
			ImGui::End();
		}


		GuiTextures::draw();

		GuiMemory::draw();



		if( GuiRecentProjects::m_bDraw ){
//			ImGui::OpenPopup("Recent Projects");
			auto lam_project_loader = [this]( std::string filename ){
				load_project( filename );
			};

			auto lam_plugin_loader = [this]( std::string filename ){
				load_plugin( filename );
			};

			GuiRecentProjects::draw( lam_project_loader, lam_plugin_loader );
		}


		if ( GuiPluginMessages::m_bDraw ) {
			GuiPluginMessages::draw();
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
	OnDraw();


	//viewport reset here?

	glPopMatrix();

	/* Swap front and back buffers */
	glfwSwapBuffers(m_winh);

}

