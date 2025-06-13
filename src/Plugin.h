//
// Created by Ben on 3/8/2024.
//

#ifndef GLFW_XP_HOST_PLUGIN_H
#define GLFW_XP_HOST_PLUGIN_H

#include <GL/glew.h>

#include <string>
#include <iostream>
#include <dlfcn.h>

#include "shim/shim_XPLMDataAccess.h"
#include "shim/shim_XPLMProcessing.h"

#include "AvionicsHost.h"
#include "DrawCallbackHost.h"
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

	std::vector<DrawCallbackHost*> m_vecDrawCallbackHost;

	struct cb_params_t;
	std::vector<cb_params_t> m_vecFlightLoops; //FIXME should be *

	std::vector<xpCmdCustom*> m_vecCommands;


	std::vector<int> m_vecTextures;

	size_t m_plugin_id;
	int m_plugin_start_ret_val=0;
	int m_plugin_enable_ret_val=0;

	bool m_plugin_is_enabled = false;
	

	Timer m_timer;




	Plugin( std::string fname );

	~Plugin();



	void takeContext();

	void releaseContext();



	void run_flcbs();

	void flcb_set( XPLMFlightLoopID id, float interval, int rel_now );

	size_t register_flcb( XPLMCreateFlightLoop_t p );

	void send_xpl_message( int from, int message, void* param );

	void call_disable();

	int call_enable();

	int call_start( char*, char*, char* );

	void call_stop();


};





// flight loop struct
struct Plugin::cb_params_t{
	XPLMCreateFlightLoop_t params{};
	float interval_secs=0.f; //FIXME: duplicated? avail in params?
	int relative_to_now=1;

	float frames_since_last_call=0.f;

	double interval_millis=0.0;

	double m_lastTimerMillis=0.0;
	size_t m_callCounter=0;
	double m_lastDelta=0.0;

	double profile_ms=0.0;
	std::string name{};

	bool m_paused=false;
};




#endif //GLFW_XP_HOST_PLUGIN_H
