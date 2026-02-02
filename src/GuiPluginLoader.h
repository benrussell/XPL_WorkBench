//
// Created by Ben on 1/12/2025.
//

#ifndef GUIPLUGINLOADER_H
#define GUIPLUGINLOADER_H


#include <vector>
#include <string>
#include <sstream>

#include "imgui.h"
#include "imgui_common.h"

#include "HostApp.h"

#include "imfilebrowser.h"



class GuiPluginLoader {
public:

    bool m_bDraw;

    ImGui::FileBrowser* m_pFileBrowser;
    ImGui::FileBrowser* m_pFolderBrowser;

    std::string m_filename;
    std::string m_folder;


    GuiPluginLoader() {
        m_bDraw = false;
        m_pFileBrowser = new ImGui::FileBrowser();
        m_pFolderBrowser = new ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
    };


    ~GuiPluginLoader() {
        delete m_pFileBrowser;
        delete m_pFolderBrowser;
    };


    //	if( m_bDrawRecent )
    void draw(
        // const std::function<void(std::string)>& cbf_load_project, //lambda
        const std::function<void(std::string)>& cbf_load_plugin //lambda
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


        if( m_folder.empty() ){
			m_folder = std::filesystem::current_path();
        }



        ImGui::Begin("Plugin Loader", &m_bDraw );
        {

            ImGui::InputText("plugin", &m_filename);
            ImGui::SameLine();
            if( ImGui::Button("...##browse_file") ){

                m_pFileBrowser->SetTitle("Choose X-Plane Plugin");
                m_pFileBrowser->SetTypeFilters({ ".xpl",".so",".dylib",".dll" });

                // std::string newCwd = "/Users/br/Downloads/xp/"; //FIXME: what working folder should we put us in?
                // m_pFileBrowser->SetPwd(newCwd);
                m_pFileBrowser->Open();

            }


            ImGui::InputText("folder", &m_folder);
            ImGui::SameLine();
            if( ImGui::Button("...##browse_folder") ){

                m_pFolderBrowser->SetTitle("Choose Working Folder");
                // std::string newCwd = "/Users/br/Downloads/xp/";
                // m_pFileBrowser->SetPwd(newCwd);
                m_pFolderBrowser->Open();

            }


            m_pFileBrowser->Display();
            if( m_pFileBrowser->HasSelected() ){
                // const std::string fname = m_pFileBrowser->GetSelected().string();
                m_filename = m_pFileBrowser->GetSelected().string();
                m_pFileBrowser->ClearSelected();
                // std::cout << "xwb/ plugin loader/ select xpl/ [" << fname << "]\n";
                //load_plugin( fname );

            } //deal with plugin open dialog

            m_pFolderBrowser->Display();
            if( m_pFolderBrowser->HasSelected() ){
                // const std::string fname = m_pFolderBrowser->GetSelected().string();
                m_folder = m_pFolderBrowser->GetSelected().string();
                m_pFolderBrowser->ClearSelected();
                // std::cout << "xwb/ plugin loader/ select folder/ [" << fname << "]\n";
                //load_plugin( fname );

            } //deal with plugin open dialog



            if( ImGui::Button("Load") ){
                const auto path_return = std::filesystem::current_path();
                std::cout << "xwb/ plugin loader/ load/ working folder [" << m_filename << "]\n";
                std::filesystem::current_path( m_folder );

                    std::cout << "  plugin loader/ calling lambda to load plugin: [" << m_filename << "]\n";
                    cbf_load_plugin( m_filename );

                std::filesystem::current_path( path_return );
            }

            if( ImGui::Button("Cancel") ){
                std::cout << "Cancel plugin loader " << std::endl;
                m_bDraw = false;
            }


            ImGui::Separator();







        }
        ImGui::End();

    }

};



#endif //GUIPLUGINLOADER_H
