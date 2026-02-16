##XPL_WorkBench - An X-Plane Plugin host.
Copyright (C) 2024-2026 Ben Russell - br@x-plugins.com

XPL_WorkBench is an experimental hack that has grown into something useful.

The aim is to create a host-app and SDK API layer that is capable of loading and running existing third-party X-Plane plugins.

The project consists two major parts:
- XPL_WorkBench - the GUI host app. This project.
- [FXPLM](https://github.com/benrussell/FXPLM) - a re-impl of the X-Plane plugin SDK layer.


Coding standards within are loose and free.

Make it exist first. Make it good later.

Look away if you're easily offended.

PR's welcome.

## OS Support
- ✅ macOS
- ✅ Linux (Mint)
- ❌ Windows - should work once the dlopen/dlsym code is patched.

## Supported SDK Features
- Basic font rendering w/ XPLMDrawString - quite broken but readable, uses stbtt.
- Mouse-Down click events for the window stack.
  - Need to add mouse-up, mouse-drag, right click, etc.
- Basic z-ordering for the window stack.
- Custom Avionics Devices
- Custom Commands
  - no sim/ commands are loaded yet.
- Basic support for datarefs loaded from DataRefs_XWB.txt
- Draw Callbacks are weakly supported but they're also sandboxes in their own FBO.
- Flight Loop callbacks for processing tasks.
- Buggy support for the Menu API, both Aircraft and Sim menus.

## Compatible Plugins
- SASLv3 will load and display it's dev console in a buggy state.
  - Need to explore the GL stack more.
- DataRefTool will load and display broken but readable GUI windows.
  - Need to explore the GL stack more.
- xLua bare project.
- xLua + Laminar C172 scripts.
- PluginAdmin from an X-Plane install loads.



## Licensing
This project is licensed under the **GNU General Public License v2.0 only**.
See the [LICENSE](./LICENSE) file for the full license text.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 2 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, see <https://www.gnu.org/licenses/>.

https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
