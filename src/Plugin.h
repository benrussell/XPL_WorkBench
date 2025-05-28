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
extern Plugin* global_target_plugin;


class Plugin {
private:
	void* m_dlh;

	public:
	static size_t plugin_id_ctr;

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
	int m_plugin_start_ret_val=0;
	int m_plugin_enable_ret_val=0;

	bool m_plugin_is_enabled = false;
	

	Timer m_timer;




	Plugin( std::string fname ){
		m_plugin_id = ++plugin_id_ctr;

		//std::cout<<"xwb/ Plugin constructor: saving cwd\n";
		namespace fs = std::filesystem;
		m_workingFolder = fs::current_path();
		m_pluginFilename = fname;

		m_timer.start();

		//global_target_plugin = this;
		//std::cout<<"xwb/ Plugin->takeContext()\n"; //this switches folders and ...stuff?
		this->takeContext();

		//works ok on mac
		std::cout<<"xwb/ calling dlopen(" << fname << ")\n";
		std::cout<<"xwb/ ---begin xpl static init---\n";
		dlerror(); //clear errors.
		//m_dlh = dlopen(fname.c_str(), RTLD_NOW | RTLD_GLOBAL); //FIXME: linux? reopen cleaner?
		m_dlh = dlopen(fname.c_str(), RTLD_NOW);

		if( m_dlh == nullptr ){
			std::string sLoadError = dlerror();
			throw std::runtime_error( sLoadError ); //we capture this for GUI display

		}else{
			std::cout<<"xwb/ ----end xpl static init---\n";
			printf("xwb/  loaded dylib; m_dlh: %p\n", m_dlh);

			char name[512]; //FIXME: x-plane SDK docs say 256??
			char desc[512];
			char sig[512];

			snprintf( name, 256, "XPL_WB Name" );
			snprintf( desc, 256, "XPL_WB Description" );
			snprintf( sig, 256, "XPL_WB Signature" );
			

			std::cout<<"xwb/ m_dlh["<< m_plugin_id <<"]->XPluginStart()\n";
			int (*fptr_start)(char*,char*,char*);
			fptr_start = (int (*)(char*,char*,char*))dlsym( m_dlh, "XPluginStart" ); //FIXME: replace with fn sig typedef
			if( fptr_start ) {
				int plugin_started = (*fptr_start)(name, sig, desc);
				std::cout << "xwb/ \tret name: " << name << "\n";
				std::cout << "xwb/ \tret sig: " << sig << "\n";
				std::cout << "xwb/ \tret desc: " << desc << "\n";

				m_pluginName = name;
				m_pluginSig = sig;
				m_pluginDesc = desc;

				m_plugin_start_ret_val = plugin_started;

				if( plugin_started ){
					std::cout << "xwb/ m_dlh["<< m_plugin_id <<"/" << m_pluginSig << "]->XPluginEnable()\n";
					int (*fptr_enable)();
					fptr_enable = (int (*)()) dlsym(m_dlh, "XPluginEnable"); //FIXME: replace with fn sig typedef
					
					int plugin_enabled = (*fptr_enable)();
					m_plugin_enable_ret_val = plugin_enabled;

					if( ! plugin_enabled ){
						// update Plugin* (this) status vars to show that plugin is disabled.
						std::cout << "xwb/ m_dlh->XPluginEnable Error: Plugin refused to enable and returned 0.\n";
						m_plugin_is_enabled = false;

					}else{
						m_plugin_is_enabled = true;

					}

				}else{
					// update Plugin* (this) status vars to show that plugin refused to start.
					std::cout << "xwb/ m_dlh->XPluginStart Error: Plugin refused to start and returned 0.\n";
				}
				
			}else{
				std::string msg = "[" + fname + "]\n" + "Could not find XPluginStart";
				throw std::runtime_error( msg );
			}

			//reset to null when we're done calling into the plugin
			//global_target_plugin = nullptr;
			this->releaseContext();

		} //dlopen worked

	} //constructor


	~Plugin(){
//		--XPHost::m_plugin_id_ctr;

		std::cout<<"xwb/ ~Plugin()\n";

		//global_target_plugin = this;
		this->takeContext();


		if( m_plugin_is_enabled ){
			std::cout<<"xwb/ m_dlh["<< m_plugin_id <<"/" << m_pluginSig << "]->XPluginDisable()\n";
			void (*fptr_disable)();
			fptr_disable = (void (*)())dlsym( m_dlh, "XPluginDisable" ); //FIXME: replace with fn sig typedef
			(*fptr_disable)();
		}
		

		if( m_plugin_start_ret_val ){
			std::cout<<"xwb/ m_dlh["<< m_plugin_id <<"/" << m_pluginSig << "]->XPluginStop()\n";
			void (*fptr_stop)();
			fptr_stop = (void (*)())dlsym( m_dlh, "XPluginStop" ); //FIXME: replace with fn sig typedef
			(*fptr_stop)();
		}else{
			std::cout<<"xwb/ m_dlh["<< m_plugin_id <<"/" << m_pluginSig << "]->XPluginStop - skipped. Plugin refused to start.\n";
		}
		

		//global_target_plugin = nullptr;
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

		std::cout << "xwb/ Calling dlclose\n\n";
		dlclose( m_dlh );

	}



	void takeContext(){
		global_target_plugin = this; // inside takeContext()

		m_workingFolder_BeforeContextSwitch = std::filesystem::current_path();
		std::filesystem::current_path(m_workingFolder);

		//std::cout<<"***** takeCtx: " << m_workingFolder_BeforeContextSwitch << "\n";


	}

	void releaseContext(){
		global_target_plugin = nullptr; //inside releaseContext()
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
		//global_target_plugin = this;
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
					const float retVal = (*params.callbackFunc)( 
						xp_delta, 
						0.0, //FIXME: this needs to be fixed.
						cb.m_callCounter, 
						params.refcon 
						);
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
				std::cout<<"xwb/ cb.interval in frames - not called.\n";
				//FIXME: need a flag to turn this one off.


			}else{
				//std::cout<<"cb.interval 0\n";
				// these are ignored.
			}




		} //loop flcb vec for this plugin

		//global_target_plugin = nullptr;
		this->releaseContext();
	}


	void flcb_set( XPLMFlightLoopID id, float interval, int rel_now ){

		auto target = (size_t)id-1;
		// std::cout<< "xwb/ flcb_set: " << target << "\n";
		// std::cout<<"xwb/     size: "<< m_vecFlightLoops.size() << "\n";
		// std::cout<<"xwb/   size-1: "<< m_vecFlightLoops.size()-1 << "\n";
		if( target > m_vecFlightLoops.size()-1 ){
			std::cout<< "xwb/ flcb_set: invalid target\n";
			return;
		}else{
			// std::cout << "xwb/   flcb_set, found target\n";
			auto cb_params = m_vecFlightLoops[target];

			//std::cout << " p: " << std::to_string((size_t)&cb_params.params) << "\n";
			// printf("xwb/  params: %p\n", &cb_params.params);
			// std::cout << "xwb/    interval: " << std::to_string(cb_params.interval_secs) << "\n";
			// std::cout << "xwb/  rel to now: " << std::to_string(cb_params.relative_to_now) << "\n";

			cb_params.interval_secs = interval;
			cb_params.interval_millis = (double)cb_params.interval_secs * 1000.0;
			cb_params.relative_to_now = rel_now;

			// /std::cout<<"xwb/   push cb_params back into vec\n";
			m_vecFlightLoops[target] = cb_params;
		}

		//std::cout<<"xwb/ flcb_set returning..\n";
	}


	size_t register_flcb( XPLMCreateFlightLoop_t p ){

		//std::cout << "xwb/ Plugin::register_flcb:\n";
		//printf("xwb/  param ptr: %p\n", &p);

		cb_params_t flcb;
		flcb.params = p;
		flcb.interval_secs=0.f;
		flcb.interval_millis = flcb.interval_secs * 1000.0;
		flcb.relative_to_now=0;

		m_vecFlightLoops.push_back( flcb );
		return m_vecFlightLoops.size();
	}


};






#endif //GLFW_XP_HOST_PLUGIN_H
