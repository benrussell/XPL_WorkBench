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

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <iostream>
#include <dlfcn.h>

// #include "shim/xp_sdk_shim.h"
// #include "shim/shim_XPLMDataAccess.h"


// #include "Plugin.h"
#include "HostApp.h"
#include "WinContent.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#include "FXPLM.h"

//#include <iostream>
#include <filesystem>

#include "CommandsTxtParse.h"
//#include "DataRefsTxtParse.h"



std::vector<WinBox*> window_pool;

std::string global_path_when_started;




const char* glob_recent_projects_filename = "recent_projects.json";
const char* glob_recent_plugins_filename = "recent_plugins.json";
const char* glob_xp_list_filename = "xp_list.json";



void load_recent_plugins_list() {
    // std::cout << "xwb/ Loading recent plugins list.. \n";

	// namespace fs = std::filesystem;
	// const std::string cwd = fs::current_path();
	// std::cout << "xwb/ working folder: [" << cwd << "]\n";

    std::ifstream f(glob_recent_plugins_filename);
    if( f ){
        nlohmann::json data = nlohmann::json::parse(f);
    	std::map<std::string, int> map_seen;

        for (const auto& pfname: data["recent"]) {
        	if ( map_seen.contains(pfname) == false ) {
        		HostApp::m_vecRecentPlugins.push_back( pfname );
        		map_seen[pfname] = 1;
        	}
        } //loop recents
    }else{
    	std::cout << "xwb/ Loading recent plugins list:404: ["<< glob_recent_plugins_filename  <<"] \n";
    }

	std::cout<<"xwb/ loaded recent plugins list: " <<  HostApp::m_vecRecentPlugins.size() << "\n";
}


void load_recent_projects_list() {
	// std::cout << "xwb/ Loading recent projects list.. \n";

	// namespace fs = std::filesystem;
	// const std::string cwd = fs::current_path();
	// std::cout << "xwb/ working folder: [" << cwd << "]\n";

	std::ifstream f(glob_recent_projects_filename);
	if( f ){
		nlohmann::json data = nlohmann::json::parse(f);
		for (const auto& pfname: data["recent"]) {
			HostApp::m_vecRecentProjects.push_back( pfname );
		} //loop recents
	}else{
		std::cout << "xwb/ Loading recent projects list:404: ["<< glob_recent_projects_filename  <<"] \n";
	}

	std::cout<<"xwb/ loaded recent projects list: " <<  HostApp::m_vecRecentProjects.size() << "\n";
}


void load_xplane_folder_list() {
	std::cout << "xwb/ Loading xplane folder list.. \n";

	// namespace fs = std::filesystem;
	// const std::string cwd = fs::current_path();
	// std::cout << "xwb/ working folder: [" << cwd << "]\n";

	std::ifstream f(glob_xp_list_filename);
	if( f ){
		nlohmann::json data = nlohmann::json::parse(f);
		for (const auto& pfname: data["xp_folders"]) {
			HostApp::m_vecXPlaneFolders.push_back( pfname );
			std::cout << "xp folder: ["<< pfname <<"]\n";
		} //loop recents
	}
}



void save_recent_plugins(){
	// return;
    std::cout << "xwb/ Saving recent plugins list..\n";

	namespace fs = std::filesystem;
	const std::string cwd = fs::current_path();
	std::cout << "     working folder: [" << cwd << "]\n";

    nlohmann::json json_arr = nlohmann::json::array();

    for( const auto& pfname: HostApp::m_vecRecentPlugins ){
        json_arr.push_back( pfname );
    }
	if ( json_arr.empty() ) {
		std::cout<<".    plugins: count 0, not saving.";
		return;
	}

    //blueprint record
    nlohmann::json json;
    json["recent"] = json_arr;

    std::ofstream myfile;
    myfile.open(glob_recent_plugins_filename);
    myfile << json.dump(1,'\t');
    myfile.close();

	std::cout << "xwb/ Saved recent plugins list: " << json_arr.size() << "\n";

}


void save_recent_projects(){
	return;
	std::cout << "xwb/ Saving recent projects list..\n";

	namespace fs = std::filesystem;
	const std::string cwd = fs::current_path();
	std::cout << "xwb/ working folder: [" << cwd << "]\n";

	nlohmann::json json;
	nlohmann::json json_arr = nlohmann::json::array();

	for( auto pfname: HostApp::m_vecRecentProjects ){
		json_arr.push_back( pfname );
	}

	//blueprint record
	json["recent"] = json_arr;

	std::ofstream myfile;
	myfile.open(glob_recent_projects_filename);
	myfile << json.dump(1,'\t');
	myfile.close();
}





#if APL
#include <CoreFoundation/CoreFoundation.h>
#include <string>

std::string GetBundleResourcePath() {
	CFBundleRef bundle = CFBundleGetMainBundle();
	if (!bundle) return {};

	CFURLRef url = CFBundleCopyResourcesDirectoryURL(bundle);
	if (!url) return {};

	char path[PATH_MAX];
	Boolean ok = CFURLGetFileSystemRepresentation(url, true,
												  (UInt8*)path,
												  sizeof(path));
	CFRelease(url);
	if (!ok) return {};

	return {path};
}
#endif



int main(int argc, char** argv)
{

	{
		namespace fs = std::filesystem;

#if APL
		const auto bundle_res_path = GetBundleResourcePath();
		fs::current_path(bundle_res_path);
#endif

		try {
			// report and store the cwd where we loaded from.
			std::cout << "xwb/ Boot Folder: [" << fs::current_path() << "]\n";
			global_path_when_started = fs::current_path();
			// std::cout << "xwb/ Current working directory: " << global_path_when_started << std::endl;
		}
		catch (const fs::filesystem_error &e) {
			std::cerr << "Filesystem error: " << e.what() << std::endl;
		}
		catch (const std::exception &e) {
			std::cerr << "General error: " << e.what() << std::endl;
		}



#if 1
#if APL
		std::cout << "xwb/ apple cwd ../../..\n";
		fs::current_path("../../..");
		const std::string cwd = fs::current_path();
		std::cout << "xwb/ working folder: [" << cwd << "]\n";

#endif
#endif


	}//scope for working folder jumps


	load_recent_projects_list();
    load_recent_plugins_list();

	load_xplane_folder_list();


#if 0 //GL v3.3 or 2.1 - X-Plane on macOS is stuck w/ 2.1
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
	glfwWindowHint(GLFW_STENCIL_BITS, 8); // Request a stencil buffer with 8 bits
#endif

	/* Initialize the library */
	if (!glfwInit()){
		return -1;
	}


	// this is our GL context creation window. it will be immediately destroyed.

	// std::cout << " Get Monitor Bounds..\n";
	auto monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	/* Create a windowed mode window and its OpenGL context */
	int os_win_width = mode->width;// - 50;
	int os_win_height = mode->height;// - 50;
	std::cout << "xwb/ Creating window on primary monitor: ";
	std::cout << " size: " << os_win_width << "," << os_win_height << "\n";


	int width=640;
	int height=480;
	auto mwinh = glfwCreateWindow( width, height, "XPL_WorkBench GL Context", nullptr, nullptr);

		if (!mwinh)
		{
			glfwTerminate();
			throw std::runtime_error("xwb/ glfw Window Create Failed.");
		}
		glfwMakeContextCurrent( mwinh );


		glewInit();
		glewExperimental = GL_TRUE; // Ensure modern OpenGL functions are loaded
		if (glewInit() != GLEW_OK) {
			std::cerr << "xwb/ Failed to initialize GLEW" << std::endl;
			exit(EXIT_FAILURE);
		}


	// std::cout<<"xwb/ FXPLM init called after first glewInit(); **************\n";
	//must be after glew init?
	//FXPLM::load_xplm();
	char name[256]="foo_name";
	char sig[256]="foo_sig";
	char desc[256]="foo_desc";

	FXPLM::call_xplm_init(nullptr,name,sig,desc);

	FXPLM::load_xpwidgets();


		namespace fs = std::filesystem;
    	// report and store the cwd where we loaded from.
    	const std::string folder_now = fs::current_path();
    	std::cout << "xwb/ wd after load_xpwidgets: " << folder_now << std::endl;


	//this must be called AFTER FXPLM_Init()
	//we have created a GL context, we can drop this now.
	glfwDestroyWindow( mwinh );





	// this MUST be after GL init.
	auto box = new WinBox(os_win_width,os_win_height);
	box->m_globalStartupFolder = global_path_when_started;
	// multiple windows can be created.
	window_pool.push_back(box);



	// Pull argv into a vector<string>
	std::vector<std::string> args;
	args.resize(argc);
	for (int i = 0; i < argc; ++i) {
		args.emplace_back(argv[i]);
	}

#if 0
	// we review our args list after we have loaded all our gui and assets, etc
	if( args.size() > 1 ){
		std::cout << "argc: " << args.size() << std::endl;
		// loop all args and print to cout
		for ( int x=0; x < args.size(); x++ ) {
			std::cout << "[" << x << "] [" << args[x] << "]\n";
		}
		std::cout << "xwb/ cmd line xpl: " << args[1] << "\n";
		box->load_plugin( args[1] );
	}
#endif

	FXPLM_TestGL();

	
	//nanovg doesnt load yet........ does this app use it? plugins?


	//remove fps limits/lock?
	//glfwSwapInterval(0);


	//windows that dont get closed get put into this vector
	std::vector<WinBox*> keepers;
	//loop will exit when all windows are closed.
	while(true){

		FXPLM::call_run_flcbs();


		//loop over child windows and drive their events
		for( auto window: window_pool ){
			auto win_h = window->m_winh;

				window->OnCallDraw(); //FIXME: this event call sig is meh.

			/* Poll for and process events */
			glfwPollEvents();

			//detect window close
			if( glfwWindowShouldClose(win_h) ){
				glfwDestroyWindow(win_h);
			}else{
				keepers.push_back(window);
			}

		} //loop window pool

		//cleanup window instances
		window_pool.swap(keepers);
		keepers.clear();


		//check if all windows have been closed
		if( window_pool.empty() ){
			std::cout << "xwb/ Window pool is empty; cleanup and exit.\n";

			save_recent_projects();
            save_recent_plugins();

			//std::cout<<"call: glfwTerminate()\n";
			glfwTerminate();
			//std::cout<<"done: glfwTerminate()\n";

			// std::exit(0);
			// exit(0);
			return 0;
		}

	} //event loop - never exits.

} //main
