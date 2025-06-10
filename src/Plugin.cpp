//
// Created by br on 8/06/25.
//


#include "Plugin.h"





	Plugin::Plugin( std::string fname ){
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
			std::cout<<"xwb/ --- xpl static init / begin ---\n";
		dlerror(); //clear errors.
		//m_dlh = dlopen(fname.c_str(), RTLD_NOW | RTLD_GLOBAL); //FIXME: linux? reopen cleaner?
		m_dlh = dlopen(fname.c_str(), RTLD_NOW);

		if( m_dlh == nullptr ){
			std::string sLoadError = dlerror();
			throw std::runtime_error( sLoadError ); //we capture this for GUI display

		}else{
			std::cout<<"xwb/ --- xpl static init / end   ---\n";
			printf("xwb/  loaded dylib; m_dlh: %p\n", m_dlh);

		} //dlopen worked

	} //constructor


	Plugin::~Plugin(){
//		--XPHost::m_plugin_id_ctr;

		std::cout<<"xwb/ ~Plugin()\n";

		this->call_disable();

		this->call_stop();


		for( auto host: m_vecDrawCallbackHost ){
			delete host;
		}


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



	void Plugin::takeContext(){
		global_target_plugin = this; // inside takeContext()

		m_workingFolder_BeforeContextSwitch = std::filesystem::current_path();
		std::filesystem::current_path(m_workingFolder);

		//std::cout<<"***** takeCtx: " << m_workingFolder_BeforeContextSwitch << "\n";


	}

	void Plugin::releaseContext(){
		global_target_plugin = nullptr; //inside releaseContext()
		std::filesystem::current_path(m_workingFolder_BeforeContextSwitch);
		//std::cout<<"###### release Ctx: " << m_workingFolder_BeforeContextSwitch << "\n";
	}






	void Plugin::run_flcbs(){
		if ( ! m_plugin_is_enabled ) {
			return;
		}

		this->takeContext();

		auto lam_runCallbackFn = [this](cb_params_t &cb, double delta) {
			//cb.m_lastTimerMillis = now;
			cb.m_lastDelta = delta;

			auto params = cb.params;
			auto xp_delta = (float)(delta / 1000.0); //x-plane SDK specifies decimal seconds.

			const double dFLCBStart = m_timer.getElapsedTimeInMilliSec();
			const float retVal = (*params.callbackFunc)(
				xp_delta, //float; elapsed since last call
				0.0, //float; elapsed time since last flight loop  //FIXME: this needs to be fixed.
				cb.m_callCounter, //int; call counter
				params.refcon //void*; refcon
				);
			const double dFLCBStop = m_timer.getElapsedTimeInMilliSec();

			cb.profile_ms = dFLCBStop - dFLCBStart;
			//std::cout << "XDbg: retval from clcb_f: " << retVal << "\n";
			cb.interval_secs = retVal;
			cb.interval_millis = (double)cb.interval_secs * 1000.0;

			++cb.m_callCounter;

			//EXPERIMENTAL
			cb.m_lastTimerMillis = m_timer.getElapsedTimeInMilliSec();
		};


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
					lam_runCallbackFn( cb, delta );
				}

			}else if( cb.interval_secs < 0.0 && ! cb.m_paused ){
				//interval is in frames
				cb.frames_since_last_call -= 1.f;
				if ( cb.interval_secs <= cb.frames_since_last_call ) {
					//std::cout << "wxb/ flcb neg interval call..\n";
					cb.frames_since_last_call = 0.f;
					lam_runCallbackFn( cb, delta );
				}

			}else{
				//std::cout<<"cb.interval 0\n";
				// interval is set to 0, registered but not scheduled. skip.
			}


		} //loop flcb vec for this plugin

		//global_target_plugin = nullptr;
		this->releaseContext();
	}


	void Plugin::flcb_set( XPLMFlightLoopID id, float interval, int rel_now ){

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


	size_t Plugin::register_flcb( XPLMCreateFlightLoop_t p ){

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



	void Plugin::send_xpl_message( int from, int message, void* param ) {

		if( m_plugin_is_enabled ){
			std::cout<<"xwb/ m_dlh["<< m_plugin_id <<"/" << m_pluginSig << "]->XPluginReceiveMessage("
			<< from << ", "
			<< message << ", "
			<< (size_t)param << ", "
			")\n";
			void (*fptr_rx_msg)(int,int,void*);
			fptr_rx_msg = (void (*)(int,int,void*))dlsym( m_dlh, "XPluginReceiveMessage" ); //FIXME: replace with fn sig typedef

			//printf("xwb/ send_xpl_msg(): fptr_rx_msg: %p\n", fptr_rx_msg);
			(*fptr_rx_msg)( from, message, param );

		}


	}



	void Plugin::call_disable() {
		this->takeContext();

		std::cout<<"xwb/ m_dlh["<< m_plugin_id <<"/" << m_pluginSig << "]->XPluginDisable()\n";
		void (*fptr_disable)();
		fptr_disable = (void (*)())dlsym( m_dlh, "XPluginDisable" ); //FIXME: replace with fn sig typedef
		(*fptr_disable)();

		this->releaseContext();
	}


	int Plugin::call_enable() {
		this->takeContext();

		std::cout<<"xwb/ m_dlh["<< m_plugin_id <<"/" << m_pluginSig << "]->XPluginEnable()\n";
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

		this->releaseContext();

		return plugin_enabled;


	}




	int Plugin::call_start( char* name, char* sig, char* desc ) {

		int plugin_started = 0;

		this->takeContext();
		std::cout<<"xwb/ m_dlh["<< m_plugin_id <<"]->XPluginStart()\n";
		int (*fptr_start)(char*,char*,char*);
		fptr_start = (int (*)(char*,char*,char*))dlsym( m_dlh, "XPluginStart" ); //FIXME: replace with fn sig typedef
		if( fptr_start ) {
			plugin_started = (*fptr_start)(name, sig, desc);
			std::cout << "xwb/ \tret name: " << name << "\n";
			std::cout << "xwb/ \tret sig: " << sig << "\n";
			std::cout << "xwb/ \tret desc: " << desc << "\n";

			m_pluginName = name;
			m_pluginSig = sig;
			m_pluginDesc = desc;

			m_plugin_start_ret_val = plugin_started;

			this->releaseContext();

		}else{
			this->releaseContext();
			std::string msg = "Could not find XPluginStart";
			throw std::runtime_error( msg );
		}


		if( plugin_started ){
			this->call_enable();

		}else{
			// update Plugin* (this) status vars to show that plugin refused to start.
			std::cout << "xwb/ m_dlh->XPluginStart Error: Plugin refused to start and returned 0.\n";
		}


		return 0;
	}



	void Plugin::call_stop() {
		if( m_plugin_start_ret_val ){
			this->takeContext();
			std::cout<<"xwb/ m_dlh["<< m_plugin_id <<"/" << m_pluginSig << "]->XPluginStop()\n";
			void (*fptr_stop)();
			fptr_stop = (void (*)())dlsym( m_dlh, "XPluginStop" ); //FIXME: replace with fn sig typedef
			(*fptr_stop)();
			this->releaseContext();
		}else{
			std::cout<<"xwb/ m_dlh["<< m_plugin_id <<"/" << m_pluginSig << "]->XPluginStop - skipped. Plugin refused to start.\n";
		}

	}






