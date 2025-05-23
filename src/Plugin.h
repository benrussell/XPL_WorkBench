//
// Created by Ben on 3/8/2024.
//

#ifndef GLFW_XP_HOST_PLUGIN_H
#define GLFW_XP_HOST_PLUGIN_H

#include <GL/glew.h>

#include <string>
#include <iostream>
#include <dlfcn.h>

#include "../xp_sdk_shim/shim_XPLMDataAccess.h"
#include "../xp_sdk_shim/shim_XPLMProcessing.h"

#include "AvionicsHost.h"
#include "GuiAvionicsDevice.h"
#include "xpCmdCustom.h"

//#include "XPHost.h"

#include <filesystem>

#include "../third_party/timer/src/Timer.h"


class Plugin;
extern Plugin* target_plugin;


class Plugin {
private:
	void* dlh;

public:

	std::string m_workingFolder{};
	std::string m_workingFolder_BeforeContextSwitch{};

	std::string m_pluginName{};
	std::string m_pluginDesc{};
	std::string m_pluginSig{};

	std::string m_pluginFilename{};

	std::vector<AvionicsHost*> m_vecAvionicsHost;
	std::vector<GuiAvionicsDevice*> m_vecGuiAv;

	struct cb_params_t;
	std::vector<cb_params_t> m_vecFlightLoops; //FIXME should be *

	std::vector<xpCmdCustom*> m_vecCommands;

	std::vector<int> m_vecTextures;

	size_t m_plugin_id;


	Timer m_timer;


	Plugin( std::string fname ){
//		++XPHost::m_plugin_id_ctr;
//		m_plugin_id = XPHost::m_plugin_id_ctr;

		std::cout<<"xplwb/ Plugin constructor: saving cwd\n";
		namespace fs = std::filesystem;

		m_workingFolder = fs::current_path();
		m_pluginFilename = fname;


		printf("\nxplwb/ Plugin ctor/ Plugin* addr: %p\n", this);
		std::cout<<"xplwb/ Loading:["<< fname << "]\n";


		m_timer.start();


		dlerror(); //clear errors.
		//linux?
//		dlh = dlopen(fname.c_str(), RTLD_NOW | RTLD_GLOBAL);

		//target_plugin = this;
		std::cout<<"xplwb/ save plugin ctx\n";
		this->takeContext();

		//works ok on mac
		std::cout<<"xplwb/ ---calling dlopen\n";
		dlh = dlopen(fname.c_str(), RTLD_NOW);

		if( dlh == nullptr ){
			std::string sLoadError = dlerror();
//			std::cout << " error:[" << dlerror() << "\n";
//			fprintf(stderr, "Error loading %s: %s\n", fname.c_str(), dlerror());

			throw std::runtime_error( sLoadError );

		}else{
			printf("xplwb/  loaded plugin, instance addr: %p\n", this);

			char name[512];
			char desc[512];
			char sig[512];

			std::cout<<"xplwb/ ---Calling XPluginStart()\n";
			int (*fptr_start)(char*,char*,char*);
			fptr_start = (int (*)(char*,char*,char*))dlsym( dlh, "XPluginStart" );
			if( fptr_start ) {
				(*fptr_start)(name, sig, desc);
				std::cout << "xplwb/ \tname: " << name << "\n";
				std::cout << "xplwb/ \t sig: " << sig << "\n";
				std::cout << "xplwb/ \tdesc: " << desc << "\n";

				m_pluginName = name;
				m_pluginSig = sig;
				m_pluginDesc = desc;


				std::cout << "xplwb/ Calling XPluginEnable()\n";
				int (*fptr_enable)();
				fptr_enable = (int (*)()) dlsym(dlh, "XPluginEnable");
				(*fptr_enable)();
			}else{
				std::string msg = "[" + fname + "]\n" + "Could not find XPluginStart";
				throw std::runtime_error( msg );
			}

			//reset to null when we're done calling into the plugin
			//target_plugin = nullptr;
			this->releaseContext();

		} //dlopen worked

	} //constructor


	~Plugin(){
//		--XPHost::m_plugin_id_ctr;

		std::cout<<"xplwb/ ~Plugin()\n";

		//target_plugin = this;
		this->takeContext();

		std::cout<<"xplwb/ Calling XPluginDisable()\n";
		int (*fptr_disable)();
		fptr_disable = (int (*)())dlsym( dlh, "XPluginDisable" );
		(*fptr_disable)();

		std::cout<<"xplwb/ Calling XPluginStop()\n";
		int (*fptr_stop)();
		fptr_stop = (int (*)())dlsym( dlh, "XPluginStop" );
		(*fptr_stop)();

		//target_plugin = nullptr;
		this->releaseContext();


		for( auto host: m_vecAvionicsHost ){
			delete host;
		}

		for( auto gui: m_vecGuiAv ){
			delete gui;
		}

//		for( auto flcb: m_vecFlightLoops ){
//			delete flcb;
//		}

		for( auto cmd: m_vecCommands ){
			delete cmd;
		}

		std::cout << "xplwb/ Calling dlclose\n\n";
		dlclose( dlh );

	}



	void takeContext(){
		target_plugin = this; // inside takeContext()

		m_workingFolder_BeforeContextSwitch = std::filesystem::current_path();
		std::filesystem::current_path(m_workingFolder);

		//std::cout<<"***** takeCtx: " << m_workingFolder_BeforeContextSwitch << "\n";


	}

	void releaseContext(){
		target_plugin = nullptr; //inside releaseContext()
		std::filesystem::current_path(m_workingFolder_BeforeContextSwitch);
		//std::cout<<"###### release Ctx: " << m_workingFolder_BeforeContextSwitch << "\n";
	}




	// flight loop struct
	struct cb_params_t{
		XPLMCreateFlightLoop_t params{};
		float interval_secs=0.f; //FIXME: duplicated? avail in params?
		int relative_to_now=1;

		double interval_millis=0.0;

		double m_lastTimerMillis=0.0;
		size_t m_callCounter=0;
		double m_lastDelta=0.0;

		double profile_ms=0.0;
		std::string name{};

		bool m_paused=false;
	};


	void run_flcbs(){
		//target_plugin = this;
		this->takeContext();

//		std::cout<<"plugin->run_flcbs()\n";
		for( auto& cb: m_vecFlightLoops ){
			double now = m_timer.getElapsedTimeInMilliSec();
			double delta = now - cb.m_lastTimerMillis;
//			std::cout<<"  now: " << delta << "\n";
//			std::cout<<"  delta: " << delta << "\n";
//			std::cout<<"  interval: " << cb.interval_millis << "\n";


			if( cb.interval_secs > 0.0 && ! cb.m_paused ){
				//flcb intervals are decimal seconds
				//timer data is decimal millis
				if( delta > cb.interval_millis ){
					//cb.m_lastTimerMillis = now;
					cb.m_lastDelta = delta;
//					std::cout<<"  fire!\n";


					//std::cout << "flcb fn addr: " << (size_t)cb.params.callbackFunc << "\n";
					//printf("flcb_fn addr: %p\n", cb.params.callbackFunc);

					auto params = cb.params;
//					XPLMFlightLoop_f)(
//						float                inElapsedSinceLastCall,
//						float                inElapsedTimeSinceLastFlightLoop,
//						int                  inCounter,
//						void *               inRefcon);
					auto xp_delta = (float)(delta / 1000.0); //x-plane SDK specifies decimal seconds.

					const double dFLCBStart = m_timer.getElapsedTimeInMilliSec();
					const float retVal = (*params.callbackFunc)( xp_delta, 0.0, cb.m_callCounter, params.refcon );
					const double dFLCBStop = m_timer.getElapsedTimeInMilliSec();

					cb.profile_ms = dFLCBStop - dFLCBStart;
					//std::cout << "XDbg: retval from clcb_f: " << retVal << "\n";
					cb.interval_secs = retVal;
					cb.interval_millis = (double)cb.interval_secs * 1000.0;

					++cb.m_callCounter;

					//EXPERIMENTAL
					cb.m_lastTimerMillis = m_timer.getElapsedTimeInMilliSec();

				}

			}else
			if( cb.interval_secs < 0.0 ){
				std::cout<<"xplwb/ cb.interval in frames - not called.\n";
				//FIXME: need a flag to turn this one off.


			}else{
				//std::cout<<"cb.interval 0\n";
				// these are ignored.
			}




		} //loop flcb vec for this plugin

		//target_plugin = nullptr;
		this->releaseContext();
	}


	void flcb_set( XPLMFlightLoopID id, float interval, int rel_now ){

		auto target = (size_t)id-1;
		std::cout<< "xplwb/ flcb_set: " << target << "\n";
		std::cout<<"xplwb/     size: "<< m_vecFlightLoops.size() << "\n";
		std::cout<<"xplwb/   size-1: "<< m_vecFlightLoops.size()-1 << "\n";
		if( target > m_vecFlightLoops.size()-1 ){
			std::cout<< "xplwb/ flcb_set: invalid target\n";
			return;
		}else{
			std::cout << "xplwb/   flcb_set, found target\n";

			auto cb_params = m_vecFlightLoops[target];

			std::cout << "xplwb/ Plugin::setr_flcb: \n";
			//std::cout << " p: " << std::to_string((size_t)&cb_params.params) << "\n";
			printf("xplwb/  params: %p\n", &cb_params.params);
			std::cout << "xplwb/  int: " << std::to_string(cb_params.interval_secs) << "\n";
			std::cout << "xplwb/  rel: " << std::to_string(cb_params.relative_to_now) << "\n";

			cb_params.interval_secs = interval;
			cb_params.interval_millis = (double)cb_params.interval_secs * 1000.0;
			cb_params.relative_to_now = rel_now;

			std::cout<<"xplwb/   push cb_params back into vec\n";
			m_vecFlightLoops[target] = cb_params;
		}

		std::cout<<"xplwb/ flcb_set returning..\n";
	}


	size_t register_flcb( XPLMCreateFlightLoop_t p ){

		std::cout << "xplwb/ Plugin::register_flcb: \n";
		printf("xplwb/  p: %p\n", &p);

		cb_params_t flcb;
		flcb.params = p;
		flcb.interval_secs=-99.f;
		flcb.interval_millis = flcb.interval_secs * 1000.0;
		flcb.relative_to_now=9;

		m_vecFlightLoops.push_back( flcb );
		return m_vecFlightLoops.size();
	}


};


#endif //GLFW_XP_HOST_PLUGIN_H
