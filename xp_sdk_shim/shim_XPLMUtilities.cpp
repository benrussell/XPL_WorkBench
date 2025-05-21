//
// Created by br on 2/8/24.
//

#include "shim_XPLMUtilities.h"

#include <iostream>
#include <string>

#include "../src/Plugin.h"

#include "../src/XPHost.h"



//class Plugin;
extern Plugin* target_plugin;


void XPLMDebugString( const char* msg ){
//    std::cout << "dbg: " << msg;
	XPHost::m_vecLog.push_back( msg );
}



void* XPLMFindPluginBySignature( char* sig ){
	std::cout<<"!XPLMFindPluginBySignature: [" << std::string(sig) << "]\n";
	return nullptr; //FIXME
}


void XPLMSendMessageToPlugin( void* to, int message, int param ){

	std::cout<<"XPLMSendMessageToPlugin: " << std::to_string((size_t)to) <<
					", msg:" << std::to_string(message) << ", param:" << param << "\n";

}







XPLMCommandRef XPLMFindCommand(
		const char *         inName){
	std::cout<<"XPLMFindCommand:[" << inName << "]\n";

	for( const auto& p: XPHost::m_vecPlugins ){
		for( const auto& cmd: p->m_vecCommands ){
			if( cmd->m_name == inName ){
//				std::cout << "found cmd\n";
				return cmd;
			}
		}
	}

	std::cerr << "XPLMFindCommand: cmd 404:[" + std::string(inName) + "]\n";

	return nullptr;
};


void       XPLMCommandBegin(
		XPLMCommandRef       inCommand){
//	std::cout<<"XPLMCommandBegin\n";
	if( inCommand ) {
//		std::cout<<"got cmd ptr..\n";
		auto cmd = (xpCmdCustom *) inCommand;
		cmd->callBegin();
	}
};


void       XPLMCommandEnd(
		XPLMCommandRef       inCommand){
//	std::cout<<"XPLMCommandEnd\n";
	if( inCommand ) {
//		std::cout<<"got cmd ptr..\n";
		auto cmd = (xpCmdCustom *) inCommand;
		cmd->callEnd();
	}
};


void       XPLMCommandOnce(
		XPLMCommandRef       inCommand){
//	std::cout<<"XPLMCommandOnce\n";

	if( inCommand ){
//		std::cout<<"got cmd ptr..\n";
		auto cmd = (xpCmdCustom*)inCommand;
		cmd->callBegin();
		cmd->callEnd();

//		std::cout<<"ran cmd.\n";
	}else{
//		throw std::runtime_error("XPLMCommandOnce: cmd id is nullptr");
		std::cerr << "XPLMCommandOnce: cmd id is nullptr\n";
	}

};


XPLMCommandRef XPLMCreateCommand(
		const char *         inName,
		const char *         inDescription){
	std::cout<<"XPLMCreateCommand:[" << inName << "]\n"; // [" << inDescription << "]\n";

	if( target_plugin ){
		auto cmd = new xpCmdCustom( inName, inDescription );
		target_plugin->m_vecCommands.push_back( cmd );

		return cmd;

	}else{
		std::cerr << "  target_plugin is nullptr\n";
	}

	return nullptr;

};


void       XPLMRegisterCommandHandler(
		XPLMCommandRef       inComand,
		XPLMCommandCallback_f inHandler,
		int                  inBefore,
		void *               inRefcon){
//	std::cout<<"XPLMRegisterCommandHandler:\n";

	auto cmd = (xpCmdCustom*)inComand;
	if( cmd ){

		cmd->m_cbf = inHandler;
		cmd->m_wants_before = inBefore;
		cmd->m_refcon = inRefcon;
//		std::cout<<"  xpCmdCustom params updated.\n";

	}else{
		std::cout<<"  XPLMRegisterCommandHandler: bad command ref\n";
	}

};



void       XPLMUnregisterCommandHandler(
		XPLMCommandRef       inComand,
		XPLMCommandCallback_f inHandler,
		int                  inBefore,
		void *               inRefcon){
	std::cout<<"!XPLMUnregisterCommandHandler\n"; //FIXME:
};











