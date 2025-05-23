#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <iostream>
#include <dlfcn.h>

#include "../xp_sdk_shim/xp_sdk_shim.h"
#include "../xp_sdk_shim/shim_XPLMDataAccess.h"


#include "Plugin.h"
#include "XPHost.h"
#include "WinContent.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"






// void load_xpl( std::string plug_name ){
//  // This fn is broken because it forces a cwd change.
//  // README: Use WinBox->load_plugin() defined in WinContent.h //FIXME: refac these filenames and class names.

// 	std::cout << "Looking for xpl\n";
// 	//const std::string root_folder = "/Users/br/Dev/gz_core/cmake-build-debug/src/examples/";
//     const std::string root_folder = "/home/br/Dev/gz_core/cmake-build-debug/src/examples/";

// 	std::string fname = root_folder + plug_name;

// 	std::cout << "------ loading xpl -----\n";

// 	Plugin* p = new Plugin(fname);
// 	XPHost::m_vecPlugins.push_back(p);

// 	std::cout << "New Plugin* at addr: " << std::to_string((size_t)p) << "\n";

// }





#include <iostream>
#include <filesystem>


std::vector<WinBox*> window_pool;

std::string global_path_when_started;




const char* glob_recent_projects_filename = "recent_projects.json";

void load_recent_projects() {
	std::cout << "Loading recent projects..\n";
	
	std::ifstream f(glob_recent_projects_filename);
	if( f ){
		nlohmann::json data = nlohmann::json::parse(f);
		for (auto pfname: data["recent"]) {
			XPHost::m_vecRecentProjects.push_back( pfname );

		} //loop recents

	}


}


void save_recent_projects(){
	std::cout << "Saving recent projects list..\n";

	
	nlohmann::json json;
	nlohmann::json json_arr = nlohmann::json::array();

	for( auto pfname: XPHost::m_vecRecentProjects ){
		json_arr.push_back( pfname );
	}

	//blueprint record
	json["recent"] = json_arr;


	std::ofstream myfile;
	myfile.open(glob_recent_projects_filename);
	myfile << json.dump(1,'\t');
	myfile.close();

}





int main(int argc, char** argv)
{


	// We need to call this code AFTER ImGui init so that the .ini file
	// is loaded correctly from same folder as XPDbg
	{
		namespace fs = std::filesystem;

		// cwd into ~/.XPL_PluginWorkBench   // FIXME: Windows?
		try {
			const char* home_env = std::getenv("HOME");
			if (!home_env) {
				std::cerr << "Error: HOME environment variable not set." << std::endl;
				return -1;
			}
			const std::string folder = std::string(home_env) + "/.XPL_WorkBench";
			std::cout << "Config folder: ["<< folder <<"]\n";

			fs::current_path(folder);
		//	std::cout << "Changed working directory to: " << folder << std::endl;

		} catch (const fs::filesystem_error &e) {
			std::cerr << "Error changing working directory: " << e.what() << std::endl;
			return -1;
		}



		try {
			// report and store the cwd where we loaded from.
			global_path_when_started = fs::current_path();
			std::cout << "Current working directory: " << global_path_when_started << std::endl;
		}
		catch (const fs::filesystem_error &e) {
			std::cerr << "Filesystem error: " << e.what() << std::endl;
		}
		catch (const std::exception &e) {
			std::cerr << "General error: " << e.what() << std::endl;
		}
	}




	// Pull argv into a vector<string>
	std::vector<std::string> args;
	for (int i = 0; i < argc; ++i) {
		args.emplace_back(argv[i]);
	}

	if( args.size() > 1 ){
		std::cout << "Loading xpl: " << args[1] << "\n";
	}






	load_recent_projects();


	drp::init(); //dref pool


#if 0
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
	if (!glfwInit())
		return -1;


// this is our base gui window.

	std::cout << " Get Monitor Bounds..\n";
	auto monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	std::cout << " Creating window on primary monitor.\n";
	/* Create a windowed mode window and its OpenGL context */

	int os_win_width = mode->width;// - 50;
	int os_win_height = mode->height;// - 50;
	std::cout << "  size: " << os_win_width << "," << os_win_height << "\n";


	int width=640;
	int height=480;
	auto mwinh = glfwCreateWindow( width, height, "XPL_WorkBench", NULL, NULL);

	if (!mwinh)
	{
		glfwTerminate();
		throw std::runtime_error("glfw Window Create Failed.");
	}

	glfwMakeContextCurrent( mwinh );



	glewInit();
	glewExperimental = GL_TRUE; // Ensure modern OpenGL functions are loaded
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}


	//we have created a GL context, we can drop this now.
	glfwDestroyWindow( mwinh );


	// this MUST be after GL init.
	auto box = new WinBox(os_win_width,os_win_height);
	window_pool.push_back(box);

/*
	// this MUST be after GL init.
	auto box = new WinBox(os_win_width,os_win_height);
	window_pool.push_back(box);
*/




	if( args.size() > 1 ){
		std::cout << "Loading xpl: " << args[1] << "\n";
		box->load_plugin( args[1] );
	}


	//nvg doesnt load yet



//	glfwSwapInterval(0);

	//windows that dont get closed get put into this vector
	std::vector<WinBox*> keepers;

	while(true){
		//loop will exit when all windows are closed.


		//std::cout << "running flcbs\n";
		for( const auto p: XPHost::m_vecPlugins ){
			p->run_flcbs();
		}




		for( auto window: window_pool ){
			auto win_h = window->m_winh;

				window->OnCallDraw();

			/* Poll for and process events */
			glfwPollEvents();

			//detect window close
			if( glfwWindowShouldClose(win_h) ){
				glfwDestroyWindow(win_h);
			}else{
				keepers.push_back(window);
			}

		} //loop window pool

		window_pool.swap(keepers);
		keepers.clear();


		//check if all windows have been closed
		if( window_pool.empty() ){
			std::cout << "Window pool is empty.\n";

			save_recent_projects();

			glfwTerminate();

			exit(0);
		}


	} //event loop - never exits.


} //main
