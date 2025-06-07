//
// Created by br on 2/8/24.
//

#ifndef GLFW_XP_HOST_SHIM_XPLMUTILITIES_H
#define GLFW_XP_HOST_SHIM_XPLMUTILITIES_H


#include <iostream>


extern "C" {


[[maybe_unused]] void XPLMDebugString(const char *msg);

void* XPLMFindPluginBySignature( char* sig );

void ex_XPLMSendMessageToPlugin( void* from, void* to, int message, int param );
void XPLMSendMessageToPlugin( void* to, int message, int param );




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

#endif //GLFW_XP_HOST_SHIM_XPLMUTILITIES_H
