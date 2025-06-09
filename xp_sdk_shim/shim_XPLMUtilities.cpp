//
// Created by br on 2/8/24.
//

#include "shim_XPLMUtilities.h"

#include <iostream>
#include <string>

#include "../src/Plugin.h"

#include "../src/XPHost.h"



//class Plugin;
extern Plugin* global_target_plugin;


void XPLMDebugString( const char* msg ){
	std::cout << "xwb/ XPLMDebugString: " << msg;
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
	return -1;
}



// this can send from any to any
void ex_XPLMSendMessageToPlugin( int from, int to, int message, void* param ){

	std::string msg = "ex_XPLMSendMessageToPlugin: from: " + std::to_string(from) +
				", to: " + std::to_string(to) +
				", msg: " + std::to_string(message) +
				", param: " + std::to_string((size_t)param);

	XPHost::m_vecPluginMessages.push_back(msg);
	std::cout << msg << "\n";


	size_t target_id = to - 1;
	if( target_id >= XPHost::m_vecPlugins.size() ){
		std::cerr << "ex_XPLMSendMessageToPlugin:  target_id out of range: " << target_id << "\n";
		return;
	}

	auto target = XPHost::m_vecPlugins[target_id];
	target->send_xpl_message( 0, message, param ); //FIXME: from

}


//this can only send from a plugin to another plugin or the host.
//this version is the SDK compliant version
void XPLMSendMessageToPlugin( int to, int message, void* param ){

	//global_target_plugin = (Plugin*)to;

	ex_XPLMSendMessageToPlugin( 1, to, message, param ); //FIXME: plugin id

	// std::cout<<"XPLMSendMessageToPlugin: " << std::to_string((size_t)to) <<
	// 				", msg:" << std::to_string(message) << ", param:" << param << "\n";

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

	std::cout << "XPLMFindCommand: cmd 404:[" + std::string(inName) + "]\n";

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
	std::cout<<"!XPLMUnregisterCommandHandler\n"; //FIXME:
};




void XPLMReloadScenery() {
	std::cout<<"wxb/ XPLMReloadScenery - NOOP!\n";
};


void XPLMFindLastNavAidOfType() {
	std::cout<<"wxb/ XPLMFindLastNavAidOfType - NOOP!\n";
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


#if 0
	sprintf(outFileNames, "");
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


void XPLMIsPluginEnabled() {
	std::cout<<"wxb/ XPLMIsPluginEnabled - NOOP!\n";
}
