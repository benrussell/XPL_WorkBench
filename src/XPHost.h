//
// Created by Ben on 3/8/2024.
//

#ifndef GLFW_XP_HOST_XPHOST_H
#define GLFW_XP_HOST_XPHOST_H

#include <GL/glew.h>

#include <vector>
#include <string>
#include <map>
#include "shim/shim_XPLMProcessing.h"
#include "AvionicsHost.h"
#include "GuiAvionicsDevice.h"
#include "GuiDatarefs.h"


class xp_dref;
#include "shim/xp_dref.h"


class GuiPlugins;
#include "GuiPlugins.h"

#include "../third_party/timer/src/Timer.h"


#include "Plugin.h"
class Plugin;
extern Plugin* global_target_plugin;

class GuiDatarefs;



class XPHost {
public:

	static float fps;

	static Timer m_timer;

	static std::vector<xp_dref*> m_dref_pool;


	//all loaded plugins
	static size_t m_plugin_id_ctr;
	static std::vector<Plugin*> m_vecPlugins;

	//datarefs tree view
	static GuiDatarefs gui_Datarefs;

	//plugins tree view
	static GuiPlugins gui_Plugins;

	static std::vector<std::string> m_vecRecentProjects;
    static std::vector<std::string> m_vecRecentPlugins;

	//content from XPLM IPC messages?
	static std::vector<std::string> m_vecPluginMessages;

	//general log window for XPLMDebugString?
    static std::vector<std::string> m_vecLog;


private:


};





class dref_factory{
public:
	static size_t search_ctr;


	static xp_dref* saveDref( const std::string& name ){
		//std::cout<<"dref_factory::saveDref: " << name << "\n";

		//		bool need = true;
		for( auto dr: XPHost::m_dref_pool ){
			++search_ctr;
			if( dr->drefName == name ){
				//need=false;
				std::cout << "  dref found: ret existing.\n";
				return dr;
			}
		}

		//		if(need){
		auto drTmp = new xp_dref( name, xp_dref_type::dref_Generic );
		XPHost::m_dref_pool.push_back(drTmp);
		//std::cout << "  dref created: ret new.\n";
		return drTmp;
		//		}

	}; //saveDref



	static void init(){

		//		auto dr_frp = new xp_dref_frp( "sim/time/framerate_period", xp_dref_type::dref_FrameRatePeriod );
		//		dref_pool.push_back(dr_frp);

		// ReSharper disable once CppDFAMemoryLeak
		auto dr_mvm = new xp_dref( "sim/graphics/view/modelview_matrix", xp_dref_type::dref_ModelViewMatrix );
		XPHost::m_dref_pool.push_back(dr_mvm);

		// ReSharper disable once CppDFAMemoryLeak
		auto dr_pm = new xp_dref( "sim/graphics/view/projection_matrix", xp_dref_type::dref_ProjectionMatrix );
		XPHost::m_dref_pool.push_back(dr_pm);

		// ReSharper disable once CppDFAMemoryLeak
		auto dr_vp = new xp_dref( "sim/graphics/view/viewport", xp_dref_type::dref_Viewport );
		XPHost::m_dref_pool.push_back(dr_vp);

		// ReSharper disable once CppDFAMemoryLeak
		auto dr_vr = new xp_dref( "sim/graphics/VR/enabled", xp_dref_type::dref_VREnabled );
		XPHost::m_dref_pool.push_back(dr_vr);

	};

	static void cleanup() {
		for ( const auto ptr: XPHost::m_dref_pool ) {
			delete ptr;
		}
	}


};





#endif //GLFW_XP_HOST_XPHOST_H
