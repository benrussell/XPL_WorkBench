//
// Created by Ben on 6/8/2024.
//

#include "GuiPlugins.h"

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

	char caName[256]{};
	char caPath[256]{};
	char caSig[256]{};
	char caDesc[256]{};



#if 1 // render plugin list
	if( plugin_count == 0 ){
		ImGui::Text("No plugins loaded.");
	}

	for ( int x=1; x<=plugin_count; x++ ) {
		XPLMGetPluginInfo(x, caName, caPath, caSig, caDesc);

		auto p = (Plugin*)FXPLM_PluginInstPtr(x);

		const std::string sNodeLabel = std::to_string(x) + ":" + caSig;
		const std::string sLabPtr = "P: " + std::to_string((size_t)x);
		if( ImGui::TreeNodeEx( sNodeLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen ) ){

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

			ImGui::Text("ptr: %p", (void*)p);
			ImGui::Text("name: %s", caName);
			ImGui::Text("desc: %s", caDesc);
			ImGui::Text(" sig: %s", caSig);

			ImGui::Text(" working: %s", p->m_workingFolder.c_str());
			ImGui::Text("    from: %s", p->m_workingFolder_BeforeContextSwitch.c_str());




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
						cmd->callBegin();
						cmd->callEnd();
					}

					ImGui::SameLine();
					ImGui::Text( "%s", cmd->m_name.c_str() );

				} //loop cmds

				ImGui::TreePop();
			} //cmd tree root




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
						ImGui::Text("60fps budget %%: %0.3f", (cb_cost / 16.f) * 100.f );


						lam_drawFboSummaryBranch("screen fbo", drawcb->m_screen_fbo, cb_cost);

						ImGui::Text( "before: %i", drawcb->m_before );
						ImGui::Text( "phase: %i", drawcb->m_phase );
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
			}


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


			ImGui::TreePop();
		}


	}

#endif
	ImGui::End(); //plugins gui



}