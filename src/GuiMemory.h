//
// Created by br on 9/06/25.
//

#ifndef GUIMEMORY_H
#define GUIMEMORY_H


#include <vector>
#include <string>
#include <sstream>

#include "imgui.h"

#include "XPHost.h"
#include <functional>



class GuiMemory {
public:

    static bool m_bDraw;

    static size_t m_offset;
    static size_t m_block_size;


    //	if( m_bDrawRecent )
    static void draw(
        // const std::function<void(std::string)>& cbf_load_project, //lambda
        // const std::function<void(std::string)>& cbf_load_plugin //lambda
        ){

        if( ! m_bDraw ) {
            return;
        }

        auto lam_splitString = [](const std::string& str, char delimiter) -> std::vector<std::string>{
            std::vector<std::string> tokens;
            std::string token;
            std::istringstream tokenStream(str);

            while (std::getline(tokenStream, token, delimiter)) {
                tokens.push_back(token);
            }

            return tokens;
        };



        ImGui::Begin("Memory", &m_bDraw );
        //		if( ImGui::BeginPopupModal("Recent Projects") )
        {

            //ImGui::InputInt("sender", &m_msg_sender_plugin_id);
            ImGui::InputScalar("offset", ImGuiDataType_U64, &m_offset, nullptr, nullptr, "%p");
            //ImGui::InputInt("target", &m_msg_target_plugin_id);
            ImGui::InputScalar("block size", ImGuiDataType_U64, &m_block_size, nullptr, nullptr, "%p");

            if( ImGui::Button("Apply") ){

            }

            ImGui::Separator();


            auto lam_hexBytesToString = [](const unsigned char* data, size_t length) -> std::string {
                std::ostringstream oss;

                oss << std::dec << std::setw(3) << length << "      "
                    << std::hex << std::setw(8) << reinterpret_cast<uintptr_t>(data) << ": ";

                for (size_t i = 0; i < length; ++i) {
                    oss << std::hex << std::setw(2) << std::setfill('0')
                        << static_cast<int>(data[i]);

                    if (i % 2 == 1) {
                        oss << " ";
                    }
                }

                return oss.str();
            };


            size_t default_ptr = 0x55ab2ee05f50;
            std::string chunk = "ptrs from app only";//lam_hexBytesToString( (unsigned char*)default_ptr, 128 );
            ImGui::Text( "%s", chunk.c_str() );


            for( const auto& msg: XPHost::m_vecPluginMessages ){
                // if( fn.empty() ){
                //     continue;
                // }

                // auto tokens = lam_splitString( fn, '/' );
                // std::string fn_clean = tokens[ tokens.size() - 1 ];
                //
                // if(ImGui::Button( ("Open [" + fn_clean +  "]##_btn_" + fn).c_str() )){
                //     //m_bDraw = false;
                //     //ImGui::CloseCurrentPopup();
                //     cbf_load_plugin( fn );
                // }
                // //ImGui::SameLine();

                ImGui::TextUnformatted( msg.c_str() );
            }


        }
        ImGui::End();

    }

};




#endif //GUIMEMORY_H
