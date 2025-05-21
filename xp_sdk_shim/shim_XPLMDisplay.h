//
// Created by br on 1/8/24.
//

#ifndef GLFW_XP_HOST_SHIM_XPLMDISPLAY_H
#define GLFW_XP_HOST_SHIM_XPLMDISPLAY_H


#include <GLFW/glfw3.h>
#include <vector>


//struct WinBox;

typedef GLFWwindow* XPLMWindowID; //FIXME


//#include "../Plugin.h"
class Plugin;
extern Plugin* target_plugin;



typedef void* XPLMAvionicsID;
//typedef void* XPLMAvionicsBezelCallback_f;
typedef void (* XPLMAvionicsBezelCallback_f)(
		float                inAmbiantR,
		float                inAmbiantG,
		float                inAmbiantB,
		void *               inRefcon);

//typedef void* XPLMAvionicsScreenCallback_f;
typedef void (* XPLMAvionicsScreenCallback_f)(
		void *               inRefcon);


//typedef void* XPLMAvionicsMouse_f;
//typedef void* XPLMAvionicsMouse_f;
typedef void* XPLMAvionicsMouseWheel_f;
typedef void* XPLMAvionicsCursor_f;

enum XPLMMouseStatus{
	xplm_MouseDown=1,
	xplm_MouseDrag=2,
	xplm_MouseUp=3
};

//typedef void* XPLMAvionicsMouse_f;
//typedef void* XPLMAvionicsMouse_f;
typedef int (* XPLMAvionicsMouse_f)(
		int                  x,
		int                  y,
		XPLMMouseStatus      inMouse,
		void *               inRefcon);

typedef void* XPLMAvionicsMouseWheel_f;
typedef void* XPLMAvionicsCursor_f;
typedef void* XPLMAvionicsKeyboard_f;
typedef void* XPLMAvionicsBrightness_f;


typedef struct {
	// Used to inform XPLMCreateAvionicsEx() of the SDK version you compiled against; should always be set to sizeof(XPLMCreateAvionics_t)
	int                       structSize;
	// Width of the device's screen in pixels.
	int                       screenWidth;
	// Height of the device's screen in pixels.
	int                       screenHeight;
	// Width of the bezel around your device's screen for 2D pop-ups.
	int                       bezelWidth;
	// Height of the bezel around your device's screen for 2D pop-ups.
	int                       bezelHeight;
	// The screen's lateral offset into the bezel for 2D pop-ups.
	int                       screenOffsetX;
	// The screen's vertical offset into the bezel for 2D pop-ups.
	int                       screenOffsetY;
	// If set to true (1), X-Plane won't call your plugin to re-render the device's screen every frame. Instead, you should tell X-Plane you want to refresh your screen with XPLMAvionicsNeedsDrawing(), and X-Plane will call you before rendering the next simulator frame.
	int                       drawOnDemand;
	// The draw callback you will use to draw the 2D-popup bezel. This is called only when the popup window is visible, and X-Plane is about to draw the bezel in it.
	XPLMAvionicsBezelCallback_f bezelDrawCallback;
	// The draw callback you will be using to draw into the device's screen framebuffer.
	XPLMAvionicsScreenCallback_f drawCallback;
	// The mouse click callback that is called when the user clicks onto your bezel.
	XPLMAvionicsMouse_f       bezelClickCallback;
	// The mouse click callback that is called when the user clicks onto your bezel.
	XPLMAvionicsMouse_f       bezelRightClickCallback;
	// The callback that is called when the users uses the scroll wheel over your avionics' bezel.
	XPLMAvionicsMouseWheel_f  bezelScrollCallback;
	// The callback that lets you determine what cursor should be shown when the mouse is over your device's bezel.
	XPLMAvionicsCursor_f      bezelCursorCallback;
	// The mouse click callback that is called when the user clicks onto your screen.
	XPLMAvionicsMouse_f       screenTouchCallback;
	// The right mouse click callback that is called when the user clicks onto your screen.
	XPLMAvionicsMouse_f       screenRightTouchCallback;
	// The callback that is called when the users uses the scroll wheel over your avionics' screen.
	XPLMAvionicsMouseWheel_f  screenScrollCallback;
	// The callback that lets you determine what cursor should be shown when the mouse is over your device's screen.
	XPLMAvionicsCursor_f      screenCursorCallback;
	// The key callback that is called when the user types in your popup.
	XPLMAvionicsKeyboard_f    keyboardCallback;
	// The callback that is called to determine the absolute brightness of the device's screen. Set to NULL to use X-Plane's default behaviour.
	XPLMAvionicsBrightness_f  brightnessCallback;
	// A null-terminated string of maximum 64 characters to uniquely identify your cockpit device. This must be unique (you cannot re-use an ID that X-Plane or another plugin provides), and it must not contain spaces. This is the string the OBJ file must reference when marking polygons with ATTR_cockpit_device. The string is copied when you call XPLMCreateAvionicsEx, so you don't need to hold this string in memory after the call.
	char *                    deviceID;
	// A null-terminated string to give a user-readable name to your device, which can be presented in UI dialogs.
	char *                    deviceName;
	// A reference which will be passed into your draw and mouse callbacks. Use this to pass information to yourself as needed.
	void *                    refcon;
} XPLMCreateAvionics_t;












typedef void (* XPLMDrawWindow_f)(
//        XPLMWindowID         inWindowID,
		void*         inWindowID,
        void *               inRefcon);



typedef int (* XPLMHandleMouseClick_f)(
        XPLMWindowID         inWindowID,
        int                  x,
        int                  y,
        XPLMMouseStatus      inMouse,
        void *               inRefcon);


enum XPLMKeyFlags{
    //FIXME: add members
};

typedef void (* XPLMHandleKey_f)(
        XPLMWindowID         inWindowID,
        char                 inKey,
        XPLMKeyFlags         inFlags,
        char                 inVirtualKey,
        void *               inRefcon,
        int                  losingFocus);

enum XPLMCursorStatus{
    //FIXME: add members
};

typedef XPLMCursorStatus (* XPLMHandleCursor_f)(
        XPLMWindowID         inWindowID,
        int                  x,
        int                  y,
        void *               inRefcon);


typedef int (* XPLMHandleMouseWheel_f)(
        XPLMWindowID         inWindowID,
        int                  x,
        int                  y,
        int                  wheel,
        int                  clicks,
        void *               inRefcon);

enum XPLMWindowDecoration{
    //FIXME: add members
};

enum XPLMWindowLayer{
    //FIXME: Add members
};




typedef struct {
    // Used to inform XPLMCreateWindowEx() of the SDK version you compiled against; should always be set to sizeof(XPLMCreateWindow_t)
    int                       structSize;
    // Left bound, in global desktop boxels
    int                       left;
    // Top bound, in global desktop boxels
    int                       top;
    // Right bound, in global desktop boxels
    int                       right;
    // Bottom bound, in global desktop boxels
    int                       bottom;
    int                       visible;
    XPLMDrawWindow_f          drawWindowFunc;
    // A callback to handle the user left-clicking within your window (or NULL to ignore left clicks)
    XPLMHandleMouseClick_f    handleMouseClickFunc;
    XPLMHandleKey_f           handleKeyFunc;
    XPLMHandleCursor_f        handleCursorFunc;
    XPLMHandleMouseWheel_f    handleMouseWheelFunc;
    // A reference which will be passed into each of your window callbacks. Use this to pass information to yourself as needed.
    void *                    refcon;
    // Specifies the type of X-Plane 11-style "wrapper" you want around your window, if any
    XPLMWindowDecoration      decorateAsFloatingWindow;
    XPLMWindowLayer           layer;
    // A callback to handle the user right-clicking within your window (or NULL to ignore right clicks)
    XPLMHandleMouseClick_f    handleRightClickFunc;
} XPLMCreateWindow_t;


extern "C" {

[[maybe_unused]] void XPLMGetScreenBoundsGlobal(
        int *outLeft,    /* Can be NULL */
        int *outTop,    /* Can be NULL */
        int *outRight,    /* Can be NULL */
        int *outBottom /* Can be NULL */
);


[[maybe_unused]] void XPLMCreateAvionicsEx(
        void *params
);


[[maybe_unused]] void XPLMSetWindowGeometry(XPLMWindowID inWindowID, int left, int top, int right, int bottom);

[[maybe_unused]] void XPLMSetWindowPositioningMode();

[[maybe_unused]] void XPLMSetWindowResizingLimits(XPLMWindowID         inWindowID,
												  int                  inMinWidthBoxels,
												  int                  inMinHeightBoxels,
												  int                  inMaxWidthBoxels,
												  int                  inMaxHeightBoxels);

[[maybe_unused]] void XPLMSetWindowTitle(XPLMWindowID inWindowID, const char *new_title);


[[maybe_unused]] void XPLMGetWindowGeometry(XPLMWindowID inWindowID, int *left, int *top, int *right, int *bottom);

[[maybe_unused]] int XPLMGetWindowIsVisible(XPLMWindowID inWindowID);

[[maybe_unused]] void XPLMSetWindowIsVisible(XPLMWindowID inWindowID, int inIsVisible);


[[maybe_unused]] int XPLMIsWindowInFront();


[[maybe_unused]] XPLMWindowID XPLMCreateWindowEx(XPLMCreateWindow_t *params);


[[maybe_unused]] void XPLMDestroyWindow(XPLMWindowID inWindowID);


[[maybe_unused]] int XPLMHasKeyboardFocus(XPLMWindowID inWindowID);


[[maybe_unused]] void XPLMTakeKeyboardFocus(XPLMWindowID inWindowID);





void       XPLMAvionicsNeedsDrawing(
		XPLMAvionicsID       inHandle);


void       XPLMDestroyAvionics(
		XPLMAvionicsID       inHandle);



} //extern "C"

#endif //GLFW_XP_HOST_SHIM_XPLMDISPLAY_H
