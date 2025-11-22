//
// Created by Ben on 2/11/2025.
//

#ifndef FXPLM_H
#define FXPLM_H

#include <iostream>
#include <filesystem>

#include <dlfcn.h>

class FXPLM {
public:


	static inline void* m_xplm_dlh;

	static void load_xplm() {
		std::cout << "xwb/ FXPLM::load_xplm()\n";

		std::string fname = "@executable_path/../../../Resources/plugins/XPLM.framework/XPLM";

		//works ok on mac
		std::cout<<"xwb/ calling dlopen(" << fname << ") RTLD_NOW | RTLD_GLOBAL\n";
		std::cout<<"xwb/ --- xplm static init / begin ---\n";
		dlerror(); //clear errors.

		void* dlh = dlopen(fname.c_str(), RTLD_NOW | RTLD_GLOBAL );
		FXPLM::m_xplm_dlh = dlh;

		if( dlh == nullptr ){
			std::string sLoadError = dlerror();
			throw std::runtime_error( sLoadError ); //we capture this for GUI display

		}else{
			std::cout<<"xwb/ --- xplm static init / end   ---\n";
			printf("xwb/  loaded dylib; dlh: %p\n", dlh);

		} //dlopen worked

		char caName[256] = "init_name";
		char caSig[256] = "init_sig";
		char caDesc[256] = "init_desc";
		call_xplm_init( dlh, caName, caSig, caDesc );

	}




	static void load_xpwidgets() {
		std::cout << "xwb/ FXPLM::load_xpwidgets()\n";

		std::string fname = "@executable_path/../../../Resources/plugins/XPWidgets.framework/XPWidgets";

		//works ok on mac
		std::cout<<"xwb/ calling dlopen(" << fname << ") RTLD_NOW | RTLD_GLOBAL\n";
		std::cout<<"xwb/ --- xplm static init / begin ---\n";
		dlerror(); //clear errors.

		void* dlh = dlopen(fname.c_str(), RTLD_NOW | RTLD_GLOBAL );
		FXPLM::m_xplm_dlh = dlh;

		if( dlh == nullptr ){
			std::string sLoadError = dlerror();
			throw std::runtime_error( sLoadError ); //we capture this for GUI display

		}else{
			std::cout<<"xwb/ --- xplm static init / end   ---\n";
			printf("xwb/  loaded dylib; dlh: %p\n", dlh);

		} //dlopen worked


		// char caName[256] = "init_name";
		// char caSig[256] = "init_sig";
		// char caDesc[256] = "init_desc";
		// call_xplm_init( dlh, caName, caSig, caDesc );


	}




	static int call_xplm_init( void* dlh, char* name, char* sig, char* desc ) {

		int init_success = 0;

		// this->takeContext();
		std::cout<<"xwb/ FXPLM::call_xplm_init()\n";
		int (*fptr_start)(char*,char*,char*);
		fptr_start = (int (*)(char*,char*,char*))dlsym( dlh, "FXPLM_Init" ); //FIXME: replace with fn sig typedef
		if( fptr_start ) {
			init_success = (*fptr_start)(name, sig, desc);
			std::cout << "xwb/ \tret name: " << name << "\n";
			std::cout << "xwb/ \tret sig: " << sig << "\n";
			std::cout << "xwb/ \tret desc: " << desc << "\n";

			// m_pluginName = name;
			// m_pluginSig = sig;
			// m_pluginDesc = desc;
			//
			// m_plugin_start_ret_val = plugin_started;
			//
			// this->releaseContext();

		}else{
			//this->releaseContext();
			std::string msg = "Could not find FXPLM_Init";
			throw std::runtime_error( msg );
		}


		if( ! init_success ) {
			throw std::runtime_error( "FXPLM_Init failed, ret value is 0\n" );
		}


		return init_success;
	}




	static int call_load_plugin( const char* fname ) {
		std::cout << "xwb/ FXPLM::call_load_plugin([" << fname << "])\n";

		void* dlh = FXPLM::m_xplm_dlh;

		int init_success = 0;



		// this->takeContext();
		// std::cout<<"xwb/ FXPLM::load_plugin()\n";
		int (*fptr_start)(const char*);
		fptr_start = (int (*)(const char*))dlsym( dlh, "FXPLM_LoadPlugin" ); //FIXME: replace with fn sig typedef
		if( fptr_start ) {
			init_success = (*fptr_start)(fname);

		}else{
			//this->releaseContext();
			const std::string msg = "Could not find FXPLM_LoadPlugin";
			throw std::runtime_error( msg );
		}


		if( ! init_success ) {
			throw std::runtime_error( "FXPLM_LoadPlugin failed, ret value is 0\n" );
		}


		return init_success;
	}









	static int call_run_flcbs() {
		//std::cout << "xwb/ FXPLM::call_run_flcbs([" << "])\n";

		void* dlh = FXPLM::m_xplm_dlh;

		int init_success = 0;

		// this->takeContext();
		// std::cout<<"xwb/ FXPLM::load_plugin()\n";
		int (*fptr_start)();
		fptr_start = (int (*)())dlsym( dlh, "FXPLM_RunFLCBS" ); //FIXME: replace with fn sig typedef
		if( fptr_start ) {
			init_success = (*fptr_start)();

		}else{
			//this->releaseContext();
			const std::string msg = "Could not find FXPLM_RunFLCBS";
			throw std::runtime_error( msg );
		}


		if( 0 > init_success ) {
			throw std::runtime_error( "FXPLM_RunFLCBS failed, ret value is 0\n" );
		}


		return init_success;
	}




	static int call_draw_cbs() {
		//std::cout << "xwb/ FXPLM::call_run_flcbs([" << "])\n";

		void* dlh = FXPLM::m_xplm_dlh;

		int init_success = 0;

		// this->takeContext();
		// std::cout<<"xwb/ FXPLM::load_plugin()\n";
		int (*fptr_start)();
		fptr_start = (int (*)())dlsym( dlh, "FXPLM_DrawCBS" ); //FIXME: replace with fn sig typedef
		if( fptr_start ) {
			init_success = (*fptr_start)();

		}else{
			//this->releaseContext();
			const std::string msg = "Could not find FXPLM_DrawCBS";
			throw std::runtime_error( msg );
		}


		if( 0 > init_success ) {
			throw std::runtime_error( "FXPLM_DrawCBS failed, ret value is 0\n" );
		}


		return init_success;
	}




};



#endif //FXPLM_H
