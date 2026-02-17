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
// Created by br on 11/06/25.
//

#include "GuiDatarefs.h"

#include <XPLM.h>





void GuiDatarefs::build_group_filter() {


    //reset the group -> group items map
    for (auto& [key, value] : m_map_dref_groups){
        // 'value' is the .second element (ValueType)
        value.clear();
    }

    m_map_dref_group_consumer_counts.clear();
    m_map_dref_group_item_counts.clear();



    //group filtering logic
    {
        std::cout << "xwb/ filtering dref groups..\n";

        for( auto dr: *HostApp::m_dref_pool ) {
            // Find the second '/' to isolate "sim/group"
            // The dref name is "sim/group/node"
            size_t first_slash = dr->drefName.find('/');
            if (first_slash != std::string::npos) {
                size_t second_slash = dr->drefName.find('/', first_slash + 1);

                if (second_slash != std::string::npos) {
                    // Extract the substring up to the second '/' (e.g., "sim/group")
                    const std::string& group_key = dr->drefName.substr(0, second_slash);

                    // find the parent group for this dref
                    auto& vec_group = m_map_dref_groups[group_key];
                    //save this dref into the parent group
                    vec_group.push_back( dr );

                    //this is pretty ugly and should prob be moved to some kind of struct in the first map
                    m_map_dref_group_consumer_counts[group_key] += dr->m_vecPluginConsumers.size();

                    if ( m_bShowAll ) {
                        m_map_dref_group_item_counts[group_key]++;
                    }else {
                            // if this dref has consumers we need to count it twd group content total
                            if ( dr->m_vecPluginConsumers.size() > 0 ) {
                                m_map_dref_group_item_counts[group_key]++;
                            }

                    }

                } //second slash
            } //first slash
        } //loop and filter

    } //group filtering logic

}


void GuiDatarefs::draw() {
    if( ! win_open ){
        return;
    }

    ImGui::SetNextWindowPos(ImVec2(0,390), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(320,360), ImGuiCond_FirstUseEver );

    const std::string devDesc = "DataRefs";
    ImGui::Begin(devDesc.c_str(), &win_open );

    ImGui::InputText("Filter", &m_filterString);

    // ImGui::SameLine();
    //static bool bShowAll = false;
    if(ImGui::Checkbox("All", &m_bShowAll)) {
        build_group_filter();
    }

    ImGui::SameLine();
    //this will force a re-index of all the sim/foo/groupings
    if (ImGui::Button("Refresh") ) {
        build_group_filter();
    } //refresh btn

    if ( HostApp::m_dref_pool == nullptr ) {
        ImGui::Text("dref pool is nullptr");
        ImGui::End();

        HostApp::m_dref_pool = (std::vector<xp_dref*>*)FXPLM_DrefPoolPtr();
        return;
    }


    //startup init
    if ( m_init_groups_filter ) {
        build_group_filter();
        m_init_groups_filter = false;
    }



    // Calculate the size of the child area (10 pixels smaller on each side)
    ImVec2 windowSize = ImGui::GetContentRegionAvail();
    ImVec2 childSize(windowSize.x - 20, windowSize.y - 20);

    ImGui::BeginChild("Child Area", childSize, true);
    { //begin layout container

        if(HostApp::m_dref_pool->empty()) {
            ImGui::Text("Empty DataRefs list.");
        }

        constexpr size_t max_label = 512;
        char caLabel[max_label]{}; //gui label
        size_t wid=0; //imgui widget id ctr because we have so many similar items, used for id
        bool bShowThisItem = false; //filter by drefs with consumers or force-all

        //iterate groups and display two level tree
        for (const auto& pair : m_map_dref_groups) {

            const size_t group_consumer_count = m_map_dref_group_consumer_counts[pair.first];
            const size_t group_item_count = m_map_dref_group_item_counts[pair.first];

            //not a lot of value in filtering the small number of parent groups
            const bool bShowThisGroup = group_consumer_count || m_bShowAll;

            if (bShowThisGroup) {

                //{group_name} {leaf_count}
                snprintf(caLabel,max_label,"%s  (%li)",
                    pair.first.c_str(),
                    group_item_count
                    );

                if (ImGui::TreeNode(caLabel)) {
                    auto vec_drefs = pair.second;


                    //m_map_dref_group_item_counts[pair.first] = 0;

                    //list of drefs for parent group shortcut
                    for ( auto dr : vec_drefs ) {

                        bShowThisItem = (!dr->m_vecPluginConsumers.empty()) || m_bShowAll;

                        if (
                            search_partial(dr->drefName.c_str(), m_filterString.c_str())
                            && bShowThisItem
                            ){

                            //FIXME: this doesnt work because its only calculated once the branch is opened.
                            //m_map_dref_group_item_counts[pair.first]++;

                            //FIXME: it would be nice to be able to pin drefs
                            // do this by adding a checkbox that adds the host.app plugin 0 to the list of consumers

    #define XWB_USE_XWB_DEV_LABEL 0
    #if XWB_USE_XWB_DEV_LABEL
                            snprintf(caLabel, max_label, "%s  '%s'  (%s)",
                                                         dr->drefName.c_str(), // "sim/foo/name"
                                                         dr->drefTypeName.c_str(), //data type: "int", "float", "float[4]", "byte", etc.
                                                         dr->typeName().c_str() // the XWB logical type name, affects code in host app
                                                );
    #else
                            snprintf(caLabel, max_label, "%s  r'%s'  %s  %li  %li",
                                                         dr->drefName.c_str(), // "sim/foo/name"
                                                         dr->drefTypeName_Raw.c_str(), //data type: "int", "float", "float[4]", "byte", etc.
                                                         dr->drefTypeName.c_str(),
                                                         dr->m_elements,
                                                         dr->m_blob_size

                                                         //dr->typeName().c_str() // the XWB logical type name, affects code in host app
                                                );
    #endif
    #undef XWB_USE_XWB_DEV_LABEL

                                if (ImGui::TreeNode(caLabel)) {
                                    // value edit
                                    char buff[max_label];
                                    if ( dr->drefTypeName == "float" ) {


                                        if ( dr->m_elements > 0 ) {
                                            float* fptr;
                                            for ( size_t ex=0; ex<dr->m_elements; ++ex ) {
                                                fptr = (float*)(dr->m_blob) + ex;
                                                std::string widget_id = "##_slider_" + std::to_string(wid) + "_" + std::to_string(ex);
                                                ImGui::SliderFloat(widget_id.c_str(), fptr, -360, 360);

                                                ImGui::SameLine();
                                                std::string widget_id_txt = "##_text_" + std::to_string(wid) + "_" + std::to_string(ex);
                                                ImGui::InputFloat(widget_id_txt.c_str(), fptr);

                                                //no call to dr->set because we're editing the blob store directly.
                                            }
                                        }else {
                                            //single element
                                            float fTmp = dr->getFloat();
                                            std::string widget_id = "##_slider_" + std::to_string(wid);
                                            ImGui::SliderFloat(widget_id.c_str(), &fTmp, -360, 360);

                                            ImGui::SameLine();
                                            std::string widget_id_txt = "##_text_" + std::to_string(wid);
                                            ImGui::InputFloat(widget_id_txt.c_str(), &fTmp);

                                            dr->setFloat(fTmp);
                                        }


                                    }else if ( dr->drefTypeName == "int" ) {
                                        int iTmp = dr->getInt();
                                        std::string widget_id = "##_slider_" + std::to_string(wid);
                                        ImGui::SliderInt(widget_id.c_str(), &iTmp, -100, 100);

                                        ImGui::SameLine();
                                        std::string widget_id_txt = "##_text_" + std::to_string(wid);
                                        ImGui::InputInt(widget_id_txt.c_str(), &iTmp);

                                        dr->setInt(iTmp);

                                    }else{

                                        ImGui::Text("Unknown data type.");

                                    }


                                    //consumers list
									#if 1
                                    snprintf( buff, max_label, "consumers[%li]", dr->m_vecPluginConsumers.size() );
                                    if (ImGui::TreeNode(buff)) {
                                        for ( auto plugin: dr->m_vecPluginConsumers ) {
                                            ImGui::Text("%s", plugin->m_pluginSig.c_str() );
                                        }
                                        ImGui::TreePop();
                                    }
									#endif


                                    ImGui::TreePop(); //plugin details node
                                } //edit leaf

                            ++wid;
                            }//dref name leaf

                    } // loop drefs in group

                    ImGui::TreePop();
                }
            }

        } // loop groups
    } // layout container
    ImGui::EndChild();


    ImGui::End();
}
