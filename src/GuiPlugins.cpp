//
// Created by Ben on 6/8/2024.
//

#include "GuiPlugins.h"

#include <XPLM.h>
#include <XPLMPlugin.h>
#include <XPLMUtilities.h>
#include <glue_CmdCustom.hpp>

#include <glue_Plugin.hpp>

std::function<void(int)> GuiPlugins::openImageInspector;

std::function<void(AvionicsHost*)> GuiPlugins::openAvionicsInspector;



void GuiPlugins::draw(){
	if( ! win_open ){
		return;
	}



	ImGui::SetNextWindowPos(ImVec2(0,30), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(320,360), ImGuiCond_FirstUseEver);
	ImGui::Begin("Plugins", &win_open);
	// ImGui::Text("FXPLM fix needed");

	const int plugin_count = XPLMCountPlugins();

	// char caName[256]{};
	// char caPath[256]{};
	// char caSig[256]{};
	// char caDesc[256]{};


#if 1 // render plugin list
	if( plugin_count == 0 ){
		ImGui::Text("No plugins loaded.");
	}

	for ( int x=0; x<plugin_count; x++ ) {
		// XPLMGetPluginInfo(x, caName, caPath, caSig, caDesc);

		auto p = (Plugin*)FXPLM_PluginInstPtr(x);

		const std::string sNodeLabel = std::to_string(x) + ":" + p->m_pluginSig.c_str();
		const std::string sLabPtr = "P: " + std::to_string((size_t)x);

		auto tree_options = ImGuiTreeNodeFlags_DefaultOpen;
		if ( x == 0 ) {
			// first item in the list is our host-app plugin, hide the details as theyre fake anyway
			tree_options = ImGuiTreeNodeFlags_None;
		}

		if( ImGui::TreeNodeEx( sNodeLabel.c_str(), tree_options ) ){


			if (p->m_plugin_start_ret_val) {
				bool plugin_enabled = XPLMIsPluginEnabled( x );

				if(ImGui::Checkbox("enabled", &plugin_enabled)) {
					if ( ! plugin_enabled ) {
						if( XPLMIsPluginEnabled( x ) ) {
							XPLMDisablePlugin( x );
						}
					}

					if ( plugin_enabled ) {
						if( ! XPLMIsPluginEnabled( x ) ) {
							XPLMEnablePlugin( x );
						}
					}
				}

			}else {
				//the plugin refused to start and cannot be enable/disabled
				ImGui::Text("Plugin failed to start.");
			}


			ImGui::Text("ptr: %p", (void*)p);
			ImGui::Text("name: %s", p->m_pluginName.c_str());
			ImGui::Text("desc: %s", p->m_pluginDesc.c_str());
			ImGui::Text(" sig: %s", p->m_pluginSig.c_str());

			ImGui::Text(" working: %s", p->m_workingFolder.c_str());
			ImGui::Text("    from: %s", p->m_workingFolder_BeforeContextSwitch.c_str());
			ImGui::Text("      fn: %s", p->m_pluginFilename.c_str() );




			const int cmd_count = FXPLM_CommandCountForPluginID( x );
			char caLabCmds[256];
			snprintf( caLabCmds, 256, "cmd [%i]", cmd_count );

			int flags_open = ImGuiTreeNodeFlags_DefaultOpen;
			int flags = 0;

			if( ImGui::TreeNodeEx(caLabCmds, flags) ) {

				for ( int cmd_x=0; cmd_x<cmd_count; cmd_x++ ) {
					//FIXME: probably shouldnt do this across dylib boundaries. dont care. add version lock between dylib and consumer app.
					auto cmd = (xpCmdCustom*)FXPLM_CommandInfo( x, cmd_x );
					// ImGui::Text(" cmd [%s]", caCmdName);

					std::string sBtnLabel_Begin = "Begin##_rnd_" + cmd->m_name;
					if( ImGui::Button(sBtnLabel_Begin.c_str()) ){
						cmd->callBegin();
					}

					ImGui::SameLine();
					std::string sBtnLabel_End = "End##_rnd_" + cmd->m_name;
					if( ImGui::Button(sBtnLabel_End.c_str()) ){
						cmd->callEnd();
					}

					ImGui::SameLine();
					std::string sBtnLabel_Once = "Once##_rnd_" + cmd->m_name;
					if( ImGui::Button(sBtnLabel_Once.c_str()) ){
						p->takeContext();
						cmd->callBegin();
						cmd->callEnd();
						p->releaseContext();
					}

					ImGui::SameLine();
					ImGui::Text( "%s", cmd->m_name.c_str() );

				} //loop cmds

				ImGui::TreePop();
			} //cmd tree root



			const std::string sLabDrefs = "drefs [" + std::to_string(p->m_vecDrefs.size()) + "]";
			if( ImGui::TreeNode(sLabDrefs.c_str()) ) {
				for (auto dref: p->m_vecDrefs ) {
					ImGui::Text("%s", dref->drefName.c_str() );
				}

				ImGui::TreePop();
			}


#if 1 // render a list of av_devs that a plugin owns
			const std::string sLabAvDevs = "av_devs [" + std::to_string(p->m_vecAvionicsHost.size()) + "]";
			if( ImGui::TreeNode(sLabAvDevs.c_str()) ){

				for( auto dev: p->m_vecAvionicsHost ){
					ImGui::Text( "id: %s", dev->m_deviceId.c_str() );
					ImGui::Text( "name: %s", dev->m_deviceName.c_str() );

					if (ImGui::Button("inspect")) {
						std::cout<<"XWB/ open avionics inspector...\n";
						openAvionicsInspector(dev);
					}


					//ImGui::Text( "ptr: %zu", (size_t)dev );
					//ImGui::Text( "%s", dev->m_deviceId );

					ImGui::Text( "composite offset: %i, %i", dev->m_params->screenOffsetX, dev->m_params->screenOffsetY );


					auto lam_drawFboSummaryBranch = [this](const std::string& label, gz_fbo* fbo_h, const float cost ){

						if(ImGui::TreeNode( label.c_str() )){
							ImGui::ColorEdit4( "cls", fbo_h->m_FboClearColorRGBA );
							ImGui::Text( "tex_id: %i", fbo_h->m_tex );
							ImGui::SameLine();
							if(ImGui::Button("show")) {
								if(openImageInspector) {
									openImageInspector( fbo_h->m_tex );
								}
							}


							ImGui::Text( "cost: %0.3f ms", cost );

							ImGui::Text( "w,h: %i,%i", fbo_h->m_width, fbo_h->m_height );

							ImGui::Text( "fbo_h: %i", fbo_h->m_fbo );
							ImGui::Text( "rbo_h: %i", fbo_h->m_rbo );

							ImGui::TreePop();
						}
					};


					const float f_dt_bezel = (float)(dev->m_bakeStop_Bezel - dev->m_bakeStart_Bezel);
					const float f_dt_screen = (float)(dev->m_bakeStop_Screen - dev->m_bakeStart_Screen);

					const std::string lab_bezel = "bezel"; // [" + std::to_string(f_dt_bezel) + "]";
					const std::string lab_screen = "screen"; // [" + std::to_string(f_dt_screen) + "]";

					lam_drawFboSummaryBranch( lab_bezel.c_str(), dev->m_bezel_fbo, f_dt_bezel );
					lam_drawFboSummaryBranch( lab_screen.c_str(), dev->m_screen_fbo, f_dt_screen );
					lam_drawFboSummaryBranch( "composite", dev->m_composite_fbo, -1.0 );


				} //loop avionics host entities.


				ImGui::TreePop();
			}
#endif //av devs list per plugin



			//FIXME: FXPLM: we dont have a concept of an av gui instance on the plugin side?
			// we could do a ptr compare and match agaisnt the infocus av_dev gui and then
			// display stats for that if !null
#if 0
			const std::string sLabAvGuis = "av_gui [" + std::to_string(p->m_vecGuiAv.size()) + "]";
			if( ImGui::TreeNode(sLabAvGuis.c_str()) ){

				for( auto av_gui: p->m_vecGuiAv ){
					//ImGui::Text( "id: %s", dev->m_deviceId.c_str() );
					ImGui::Text( "dev_ptr: %zu", (size_t)av_gui->m_dev );

				}

				ImGui::TreePop();
			}
#endif
#if 1
			const std::string sLabDrawCbs = "draw_cbs [" + std::to_string(p->m_vecDrawCallbackHost.size()) + "]";
			if( ImGui::TreeNode(sLabDrawCbs.c_str()) ){

				//duplicated code.
				auto lam_drawFboSummaryBranch = [this](const std::string& label, gz_fbo* fbo_h, const float cost ){

					if(ImGui::TreeNode( label.c_str() )){
						ImGui::ColorEdit4( "cls", fbo_h->m_FboClearColorRGBA );
						ImGui::Text( "tex_id: %i", fbo_h->m_tex );
						ImGui::SameLine();
						if(ImGui::Button("show")) {
							if(openImageInspector) {
								openImageInspector( fbo_h->m_tex );
							}
						}

						//ImGui::Text( "cost: %0.3f ms", cost );

						ImGui::Text( "w,h: %i,%i", fbo_h->m_width, fbo_h->m_height );

						ImGui::Text( "fbo_h: %i", fbo_h->m_fbo );
						ImGui::Text( "rbo_h: %i", fbo_h->m_rbo );

						ImGui::TreePop();
					}

				};


				auto lam_drawDrawCallbackHost = [this, lam_drawFboSummaryBranch](const std::string& label, DrawCallbackHost* drawcb) {

					if(ImGui::TreeNode( label.c_str() )) {
						ImGui::Text( "name: [%s]", drawcb->m_deviceName.c_str() );

						ImGui::Checkbox( "disabled", &drawcb->m_xplw_disabled );

						double cb_cost = drawcb->m_bakeStop_Screen - drawcb->m_bakeStart_Screen;
						ImGui::Text( "cost: %0.3f ms", cb_cost );

						auto fps_cap = (float)(1000.0 / cb_cost);
						fps_cap = std::min(9999.f, fps_cap);
						ImGui::Text("fps cap: %0.3f", fps_cap );
						ImGui::Text("60fps budget: %0.3f%%", (cb_cost / 16.f) * 100.f );


						lam_drawFboSummaryBranch("screen fbo", drawcb->m_screen_fbo, cb_cost);

						ImGui::Text( "before: %i", drawcb->m_before );

						//ImGui::Text( "phase: %i", drawcb->m_phase );
						std::string phase_label;
						switch( drawcb->m_phase ){
							case 25:
								phase_label = "Airplanes"; break;
							case 40:
								phase_label = "Panel"; break;
							case 45:
								phase_label = "Window"; break;
							default:
								phase_label = "unk"; break; //FIXME: report numeric
						};
						ImGui::Text( "phase: %s", phase_label.c_str() );

						ImGui::Text( "refcon: %p", drawcb->m_refcon );

						ImGui::TreePop();
					}

				};


				size_t host_counter = 0;
				for( auto drawcb: p->m_vecDrawCallbackHost ){
					lam_drawDrawCallbackHost( "draw_cb_" + std::to_string(host_counter), drawcb );
					++host_counter;
				}

				ImGui::TreePop();
			} //drawing cbs
#endif



#if 1 //flcbs debug

			const std::string sLabFlcbs = "flcbs [" + std::to_string(p->m_vecFlightLoops.size()) + "]";
			if( ImGui::TreeNode(sLabFlcbs.c_str()) ){

				size_t fl_ctr=0;
				for( auto& flcb: p->m_vecFlightLoops ){

					std::string id = "_id" + std::to_string(fl_ctr);
					char flcbLabel[256];
					std::snprintf(flcbLabel, sizeof(flcbLabel), "flcb_ptr[%zu] fnptr:%p rc:%p: %f", fl_ctr, flcb.params.callbackFunc, flcb.params.refcon, flcb.profile_ms);
					std::string flcb_lab = flcbLabel;
					if(ImGui::TreeNodeEx(id.c_str(), 0, "%s",flcb_lab.c_str())){
						ImGui::Checkbox("pause", &flcb.m_paused);

						ImGui::Text("cb cost: %0.3f ms", flcb.profile_ms );
						auto fps_cap = (float)(1000.0 / flcb.profile_ms);
						fps_cap = std::min(9999.f, fps_cap);
						ImGui::Text("fps cap: %0.3f", fps_cap );
						ImGui::Text("60fps budget %%: %0.3f", (flcb.profile_ms / 16.f) * 100.f );

						ImGui::Text("cb interval: %0.3f sec / %0.3f hz", flcb.interval_secs, (1.f / flcb.interval_secs) );
						ImGui::Text("cb last delta: %0.1f ms", flcb.m_lastDelta );
						ImGui::Text("cb actual rate: %0.1f hz", 1000.0 / flcb.m_lastDelta );


						ImGui::Text("cb relative: %i", flcb.relative_to_now );
						ImGui::Text("cb call ctr: %zu", flcb.m_callCounter );

						ImGui::TreePop();
					}
//					ImGui::PopID();

					++fl_ctr;
				}

				ImGui::TreePop();
			}



			const std::string sLabTextures = "textures [" + std::to_string(p->m_vecTextures.size()) + "]";
			if( ImGui::TreeNode(sLabTextures.c_str()) ){

				for( auto tex_id: p->m_vecTextures ){
					//ImGui::Text( "id: %s", dev->m_deviceId.c_str() );
					ImGui::Text( "tex_id: %i", tex_id );

				}

				ImGui::TreePop();
			}
#endif


#if 1 // DrawWindowEx structures vec
			const std::string sLabWindows = "windows [" + std::to_string(p->m_vecWindowHandles.size()) + "]";
			if( ImGui::TreeNode(sLabWindows.c_str()) ){

				//iterate window backing handles
				char caLabel[128];
				for( auto win: p->m_vecWindowHandles ){

					snprintf(caLabel,128,"%p [%s]", win, win->m_sWindowTitle.c_str());
					if( ImGui::TreeNode(caLabel) ){
						//details for a window handle
						bool vis = win->m_params->visible;
						if(ImGui::Checkbox("visible", &vis)) {
							//invert the value because it was clicked.
							win->m_params->visible = ! win->m_params->visible;
						}

						//ImGui::Text( "visible:%i", win->m_params->visible );

						ImGui::Text( "refcon:%p", win->m_params->refcon );
						ImGui::Text( "z_index:%zu", win->z_index );
						ImGui::Text( "draw_fn:%p", win->m_params->drawWindowFunc );


						double cb_cost = win->m_bakeStop - win->m_bakeStart;
						ImGui::Text( " cost:%0.3fms", cb_cost );
						auto fps_cap = (float)(1000.0 / cb_cost);
						fps_cap = std::min(9999.f, fps_cap);
						ImGui::Text("fps cap: %0.3f", fps_cap );
						ImGui::Text("60fps budget: %0.3f%%", (cb_cost / 16.f) * 100.f );


						ImGui::Text( "click_fn:%p", win->m_params->handleMouseClickFunc );
						ImGui::Text( "key_fn:%p", win->m_params->handleKeyFunc );

						ImGui::InputInt("left:", &win->m_params->left );
						ImGui::InputInt("top:", &win->m_params->top );
						ImGui::InputInt("right:", &win->m_params->right );
						ImGui::InputInt("bottom:", &win->m_params->bottom );
//						ImGui::Text( "left:%i", win->m_params->left );
//						ImGui::Text( "top:%i", win->m_params->top );
//						ImGui::Text( "right:%i", win->m_params->right );
//						ImGui::Text( "bott:%i", win->m_params->bottom );

						ImGui::TreePop();
					}

				}

				ImGui::TreePop();
			}
#endif



			ImGui::TreePop();
		}


	}

#endif
	ImGui::End(); //plugins gui



}