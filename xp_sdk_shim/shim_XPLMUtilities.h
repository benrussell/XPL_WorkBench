//
// Created by br on 2/8/24.
//

#ifndef GLFW_XP_HOST_SHIM_XPLM_UTILITIES_H
#define GLFW_XP_HOST_SHIM_XPLM_UTILITIES_H



extern "C" {

	void XPLMReloadScenery();

	void XPLMFindLastNavAidOfType();

	void XPLMGetDirectoryContents(const char *         inDirectoryPath,
						 int                  inFirstReturn,
						 char *               outFileNames,
						 int                  inFileNameBufSize,
						 char **              outIndices,    /* Can be NULL */
						 int                  inIndexCount,
						 int *                outTotalFiles,    /* Can be NULL */
						 int *                outReturnedFiles);


	void XPLMIsPluginEnabled();


[[maybe_unused]] void XPLMDebugString(const char *msg);

int XPLMFindPluginBySignature( char* sig );

void ex_XPLMSendMessageToPlugin( int from, int to, int message, void* param );
void XPLMSendMessageToPlugin( int to, int message, void* param );




enum XPLMCommandPhase{
	xplm_CommandBegin,
	xplm_CommandContinue,
	xplm_CommandEnd,
};


typedef void* XPLMCommandRef;

typedef int (* XPLMCommandCallback_f)(
		XPLMCommandRef       inCommand,
		XPLMCommandPhase     inPhase,
		void *               inRefcon);


XPLMCommandRef XPLMFindCommand(
		const char *         inName);


void       XPLMCommandBegin(
		XPLMCommandRef       inCommand);


void       XPLMCommandEnd(
		XPLMCommandRef       inCommand);


void       XPLMCommandOnce(
		XPLMCommandRef       inCommand);


XPLMCommandRef XPLMCreateCommand(
		const char *         inName,
		const char *         inDescription);


void       XPLMRegisterCommandHandler(
		XPLMCommandRef       inComand,
		XPLMCommandCallback_f inHandler,
		int                  inBefore,
		void *               inRefcon);


void       XPLMUnregisterCommandHandler(
		XPLMCommandRef       inComand,
		XPLMCommandCallback_f inHandler,
		int                  inBefore,
		void *               inRefcon);




} //extern

#endif //GLFW_XP_HOST_SHIM_XPLM_UTILITIES_H
