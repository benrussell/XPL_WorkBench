//
// Created by Ben on 6/8/2024.
//

#include "GuiPlugins.h"



void GuiPlugins::draw(){
	if( ! win_open ){
		return;
	}



	ImGui::SetNextWindowPos(ImVec2(0,30), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(320,360), ImGuiCond_FirstUseEver);
	ImGui::Begin("Plugins", &win_open);
	if( XPHost::m_vecPlugins.empty() ){
		ImGui::Text("No plugins loaded.");
	}

	for( auto p: XPHost::m_vecPlugins ){

		const std::string sNodeLabel = p->m_pluginSig;
		const std::string sLabPtr = "P: " + std::to_string((size_t)p);
		if( ImGui::TreeNodeEx( sNodeLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen ) ){

			ImGui::Text("id: %zu", p->m_plugin_id);

			ImGui::Text("ptr: %p", (size_t)p);
			ImGui::Text("name: %s", p->m_pluginName.c_str());
			ImGui::Text("desc: %s", p->m_pluginDesc.c_str());
			ImGui::Text(" sig: %s", p->m_pluginSig.c_str());

			ImGui::Text(" working: %s", p->m_workingFolder.c_str());
			ImGui::Text("    from: %s", p->m_workingFolder_BeforeContextSwitch.c_str());

			const std::string sLabCmds = "cmds [" + std::to_string(p->m_vecCommands.size()) + "]";
			int flags_open = ImGuiTreeNodeFlags_DefaultOpen;
			int flags = 0;
			if( ImGui::TreeNodeEx(sLabCmds.c_str(), flags) ){
				for( auto cmd: p->m_vecCommands ){

//					ImGui::SameLine();
					std::string sBtnLabel_Begin = "Begin##_rnd_" + cmd->m_name;
					if( ImGui::Button(sBtnLabel_Begin.c_str()) ){
						cmd->callBegin();
					}

					// counter intuitive, requires a bunch of extra work. begin/end good enough to build gauges.
//							if (ImGui::IsItemActive()) {
//								// Button is being held down
//								ImGui::Text("Button is being held down!");
//								cmd->callHold();
//							}

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


				}

				ImGui::TreePop();
			}


			const std::string sLabAvDevs = "av_devs [" + std::to_string(p->m_vecAvionicsHost.size()) + "]";
			if( ImGui::TreeNode(sLabAvDevs.c_str()) ){

				for( auto dev: p->m_vecAvionicsHost ){
					ImGui::Text( "id: %s", dev->m_deviceId.c_str() );
					ImGui::Text( "name: %s", dev->m_deviceName.c_str() );
					//ImGui::Text( "ptr: %zu", (size_t)dev );
					//ImGui::Text( "%s", dev->m_deviceId );

					ImGui::Text( "composite offset: %i, %i", dev->m_params->screenOffsetX, dev->m_params->screenOffsetY );


					auto lam_drawFboSummaryBranch = [this](const std::string& label, gz_fbo* fbo_h, const float cost ){

						if(ImGui::TreeNode( label.c_str() )){
							ImGui::ColorEdit4( "cls", fbo_h->m_FboClearColorRGBA );
							ImGui::Text( "tex_id: %i", fbo_h->m_tex );
							ImGui::SameLine();
							ImGui::Button("show");

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

			const std::string sLabAvGuis = "av_gui [" + std::to_string(p->m_vecGuiAv.size()) + "]";
			if( ImGui::TreeNode(sLabAvGuis.c_str()) ){

				for( auto av_gui: p->m_vecGuiAv ){
					//ImGui::Text( "id: %s", dev->m_deviceId.c_str() );
					ImGui::Text( "dev_ptr: %zu", (size_t)av_gui->m_dev );

				}

				ImGui::TreePop();
			}


			const std::string sLabFlcbs = "flcbs [" + std::to_string(p->m_vecFlightLoops.size()) + "]";
			if( ImGui::TreeNode(sLabFlcbs.c_str()) ){

				size_t fl_ctr=0;
				for( auto& flcb: p->m_vecFlightLoops ){

					std::string id = "_id" + std::to_string(fl_ctr);
					std::string flcb_lab = "[" + std::to_string(fl_ctr) + "] " + std::to_string((size_t)flcb.params.callbackFunc) + ": " + std::to_string(flcb.profile_ms);
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



			ImGui::TreePop();
		}


	}
	ImGui::End(); //plugins gui



}