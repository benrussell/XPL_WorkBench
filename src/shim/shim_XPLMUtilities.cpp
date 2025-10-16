//
// Created by br on 2/8/24.
//

#include "shim_XPLMUtilities.h"

#include <iostream>
#include <string>

#include "../Plugin.h"

#include "../XPHost.h"



//class Plugin;
extern Plugin* global_target_plugin;



[[maybe_unused]] void XPLMGetSystemPath( char* outBuff ){
	//	std::cout << "!XPLMGetSystemPath..\n";
	// snprintf( outBuff, 512, "/fake/x-plane/path" ); //FIXME:
	snprintf( outBuff, 512, "/media/br/MintyData/br/X-Plane 12" ); //FIXME:
}


void XPLMDebugString( const char* msg ){
	std::cout << "xwb/ XPLMDebugString: [" << msg << "]\n";
	XPHost::m_vecLog.push_back( msg );
}


int XPLMFindPluginBySignature( char* sig ){
	std::cout<<"XPLMFindPluginBySignature: [" << std::string(sig) << "] ";

	for ( auto plugin: XPHost::m_vecPlugins ) {
		if( plugin->m_pluginSig == sig ){
			std::cout << " found plugin: " << plugin->m_plugin_id << "\n";
			return plugin->m_plugin_id;
		}
	}

	std::cout << " 404\n";
	//returning 0 will route all msgs into the host.
	//returning -1 will probably set a flag on the querying plugin side.
	return 0;//-1;
}


// this can send from any to any
void ex_XPLMSendMessageToPlugin( int from, int to, int message, void* param ){

	char msg[256];
	snprintf(msg, 256, "ex_XPLMSendMessageToPlugin: from: %d, to: %d, msg: 0x%08x, param: %p",
			from, to, message, param);
	std::string str_msg(msg);

	//this is a plugin registration msg, decode it.
	if ( message == 0x01000000 ) {
		std::string payload = std::string( (char*)param );
		XPHost::m_vecPluginMessages.push_back(payload);
		std::cout << payload << "\n";
	}


	//log for host gui
	XPHost::m_vecPluginMessages.push_back(msg);
	std::cout << msg << "\n";

	if ( to == 0 ) {
		//msg is to host
		//std::cout << "  msg is to host\n";
		return;
	}


	size_t target_id = to - 1;
	if( target_id >= XPHost::m_vecPlugins.size() ){
		std::cerr << "ex_XPLMSendMessageToPlugin:  target_id out of range: " << target_id << "\n";
		return;
	}

	auto target = XPHost::m_vecPlugins[target_id];
	target->send_xpl_message( from, message, param );

}


//this can only send from a plugin to another plugin or the host.
//this version is the SDK compliant version
void XPLMSendMessageToPlugin( int to, int message, void* param ){

	int sender_id = global_target_plugin->m_plugin_id;
	ex_XPLMSendMessageToPlugin( sender_id, to, message, param );

	// std::cout<<"XPLMSendMessageToPlugin: " << std::to_string((size_t)to) <<
	// 				", msg:" << std::to_string(message) << ", param:" << param << "\n";

}


XPLMCommandRef XPLMFindCommand(
		const char *         inName){
	std::cout<<"xwb/ XPLMFindCommand:[" << inName << "]\n";

	for( const auto& p: XPHost::m_vecPlugins ){
		for( const auto& cmd: p->m_vecCommands ){
			if( cmd->m_name == inName ){
//				std::cout << "found cmd\n";
				return cmd;
			}
		}
	}

#define AUTO_CMD_CREATE 1
#if AUTO_CMD_CREATE
	auto cmd = new xpCmdCustom( inName, "inDescription" );

	if ( global_target_plugin ) {
		std::cout << "auto created cmd, attempting to assing to global_target_plugin\n";
		global_target_plugin->m_vecCommands.push_back( cmd );

	}else {
		std::cerr << "global_target_plugin is nullptr\n";
	}

	std::cout << "attempting to return new auto created cmd handle\n";
	return cmd;
#endif



	std::cout << "XPLMFindCommand:[" + std::string(inName) + "] 404\n";
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

	if( global_target_plugin ){
		auto cmd = new xpCmdCustom( inName, inDescription );
		global_target_plugin->m_vecCommands.push_back( cmd );

		return cmd;

	}else{
		std::cerr << "  global_target_plugin is nullptr\n";
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
	std::cout<<"!XPLMUnregisterCommandHandler - NOOP\n"; //FIXME:
};


void XPLMReloadScenery() {
	std::cout<<"wxb/ XPLMReloadScenery - NOOP!\n"; //FIXME
};


void XPLMFindLastNavAidOfType() {
	std::cout<<"wxb/ XPLMFindLastNavAidOfType - NOOP!\n"; //FIXME
}


void XPLMGetDirectoryContents(const char* inDirectoryPath,
                             int inFirstReturn,
                             char* outFileNames,
                             int inFileNameBufSize,
                             char** outIndices,
                             int inIndexCount,
                             int* outTotalFiles,
                             int* outReturnedFiles) {

	std::cout<<"wxb/ XPLMGetDirectoryContents: ";
	std::cout<<"  inDirectoryPath: [" << inDirectoryPath << "]\n";
	// std::cout<<"  inFirstReturn: " << inFirstReturn << "\n";
	// std::cout<<"  outFileNames: " << outFileNames << "\n";
	// std::cout<<"  inFileNameBufSize: " << inFileNameBufSize << "\n";
	// std::cout<<"  outIndices: " << outIndices << "\n";
	// std::cout<<"  inIndexCount: " << inIndexCount << "\n";
	// std::cout<<"  outTotalFiles: " << outTotalFiles << "\n";
	// std::cout<<"  outReturnedFiles: " << outReturnedFiles << "\n";


#if 1
	snprintf(outFileNames, inFileNameBufSize, "");
	*outTotalFiles = 0;
	*outReturnedFiles = 0;
	return;
#endif



    if (!inDirectoryPath || !outFileNames || !outReturnedFiles) {
        if (outReturnedFiles) *outReturnedFiles = 0;
        if (outTotalFiles) *outTotalFiles = 0;
        return;
    }

    // Use C++17 filesystem to get directory contents
    std::vector<std::string> files;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(inDirectoryPath)) {
            files.push_back(entry.path().filename().string());
        }
    } catch (const std::exception& e) {
        if (outReturnedFiles) *outReturnedFiles = 0;
        if (outTotalFiles) *outTotalFiles = 0;
        return;
    }

    // Set total number of files if requested
    if (outTotalFiles) {
        *outTotalFiles = static_cast<int>(files.size());
    }

    // Calculate how many files we can return
    int availableSpace = inFileNameBufSize;
    int filesReturned = 0;
    int currentPos = 0;

    // Skip files before inFirstReturn
    size_t startIndex = static_cast<size_t>(inFirstReturn);

    // Process files
    for (size_t i = startIndex; i < files.size() && availableSpace > 0; ++i) {
        const std::string& filename = files[i];

        // Check if we have space for this filename + null terminator
        if (availableSpace >= (filename.length() + 1)) {
            // Copy filename to output buffer
            strcpy(outFileNames + currentPos, filename.c_str());

            // Update indices if requested
            if (outIndices && filesReturned < inIndexCount) {
                outIndices[filesReturned] = outFileNames + currentPos;
            }

            currentPos += filename.length() + 1;
            availableSpace -= filename.length() + 1;
            filesReturned++;
        } else {
            break;
        }
    }

    // Set number of files actually returned
    *outReturnedFiles = filesReturned;
}


int XPLMIsPluginEnabled( int plugin_id ) {
	std::cout<<"wxb/ XPLMIsPluginEnabled( plugin_id:" << plugin_id << " )\n";

	if ( plugin_id == 0 ) {
		// host is always enabled.
		return 1;
	}

	size_t target_id = plugin_id-1;
	if( target_id >= XPHost::m_vecPlugins.size() ){
		std::cerr << "XPLMIsPluginEnabled:  target_id out of range: " << target_id << "\n";
		return 0;
	}
	auto target = XPHost::m_vecPlugins[target_id];

	return target->m_plugin_is_enabled;
}

