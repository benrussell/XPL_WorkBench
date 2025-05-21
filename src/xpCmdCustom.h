//
// Created by Ben on 6/8/2024.
//

#ifndef GLFW_XP_HOST_XPCMDCUSTOM_H
#define GLFW_XP_HOST_XPCMDCUSTOM_H

#include <iostream>

#include "../xp_sdk_shim/shim_XPLMUtilities.h"


class xpCmdCustom {
public:
	xpCmdCustom( const std::string& name, const std::string& desc ){
		m_name = name;
		m_description = desc;

	}

	void setHandlerFn(
			XPLMCommandCallback_f fn,
			int wants_before,
			void* refcon
			){

		m_cbf = fn;
		m_wants_before = wants_before;
		m_refcon = refcon;

	}

	void callBegin(){
		std::cout<<"cmd->callBegin():" << this->m_name << "\n";
		if( m_cbf ){
			m_cbf( this, XPLMCommandPhase::xplm_CommandBegin, this->m_refcon );
		}else{
			//throw std::runtime_error("cmd cbf is nullptr");
			std::cout << "cmd cbf is nullptr\n";
		}
	}


	void callHold(){
		std::cout<<"cmd->callHold():" << this->m_name << "\n";
		if( m_cbf ){
			m_cbf( this, XPLMCommandPhase::xplm_CommandContinue, this->m_refcon );
		}else{
			//throw std::runtime_error("cmd cbf is nullptr");
			std::cout << "cmd cbf is nullptr\n";
		}
	}


	void callEnd(){
		std::cout<<"cmd->callEnd():" << this->m_name << "\n";
		if( m_cbf ){
			m_cbf( this, XPLMCommandPhase::xplm_CommandEnd, this->m_refcon );
		}else{
			//throw std::runtime_error("cmd cbf is nullptr");
			std::cout << "cmd cbf is nullptr\n";
		}
	}

	XPLMCommandCallback_f m_cbf{};
	int m_wants_before=0;
	void* m_refcon{};

	std::string m_name{};
	std::string m_description{};

};


#endif //GLFW_XP_HOST_XPCMDCUSTOM_H
