/*
 * XPL_WorkBench - An X-Plane Plugin host.
 * Copyright (C) 2024-2026 Ben Russell - br@x-plugins.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 */

//
// Created by Ben on 2/11/2025.
//

#ifndef FXPLM_H
#define FXPLM_H

#include <iostream>
#include <filesystem>

#include <dlfcn.h>
#include <XPLM.h>

class FXPLM {
public:


	static inline void* m_xpwidgets_dlh; //dylib handle


	static void load_xpwidgets() {
		std::cout << "xwb/ FXPLM::load_xpwidgets()\n";


		std::cout << "xwb/ cwd:[" << std::filesystem::current_path() << "]\n";

#if APL
		std::string fname = "@executable_path/../../../Resources/plugins/XPWidgets.framework/XPWidgets";
#elif LIN
		std::filesystem::current_path("Dev/XPL_WorkBench/cmake-build-debug/Resources/plugins");
		std::cout << "xwb/ cwd2:[" << std::filesystem::current_path() << "]\n";

		std::string fname = "XPWidgets_64.so";
#endif

		//works ok on mac
		std::cout<<"xwb/ calling dlopen(" << fname << ") RTLD_NOW | RTLD_GLOBAL\n";
		std::cout<<"xwb/ --- xplm static init / begin ---\n";
		dlerror(); //clear errors.

		void* dlh = dlopen(fname.c_str(), RTLD_NOW | RTLD_GLOBAL );
		FXPLM::m_xpwidgets_dlh = dlh;

		if( dlh == nullptr ){
			std::string sLoadError = dlerror();
			throw std::runtime_error( sLoadError ); //we capture this for GUI display

		}else{
			std::cout<<"xwb/ --- xplm static init / end   ---\n";
			printf("xwb/  loaded dylib; dlh: %p\n", dlh);

		} //dlopen worked


#if APL
#elif LIN
		std::cout << "xwb/ cwd back to base app folder..\n";
		std::filesystem::current_path("../..");
		std::cout << "xwb/ cwd now: " << std::filesystem::current_path() << "\n";
#endif

		//FIXME: need to tweak the build so that we can dlopen from the current folder or
		// some other useful search path.
		// apparently i want this: gcc main.c -Wl,-rpath,'.' -ldl
		// Note: Using '.' works for the literal CWD. Using '$ORIGIN' is generally better practice as it refers to the folder containing the library/executable, regardless of where you launch it from.

	} //load_xp_widgets()

};



#endif //FXPLM_H
