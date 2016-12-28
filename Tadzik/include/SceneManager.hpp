#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include "Scene.hpp"
#include "Utils.hpp"

#include <unordered_map>
#include <string>
#include <iostream>

class SceneManager{
public:
    SceneManager(sf::RenderWindow* w, sf::Font* f)
    :window(w)
    {
        fpsCounter.setFont(*f);
        fpsCounter.setOrigin(0, 40);
        fpsCounter.setPosition(0, window->getSize().y);
        fpsCounter.setColor(sf::Color::Green);
        //tadzikCMD = new TadzikCMD(this);
    }

    virtual ~SceneManager(){
        std::cout << "Destructor of SceneManager{\n";
        std::cout << scenes.size() << "\n";
        auto itr = scenes.begin();
        while (itr != scenes.end()) {
            delete itr->second;
            itr = scenes.erase(itr);
        }
        std::cout << scenes.size() << "\n";
        std::cout << "}\n";
    }
    template<typename T>
    void registerScene(std::string name, sf::RenderWindow* w){
        scenes[name] = new T(name, this, w);
        scenes[name]->setSceneWindow(window);
        scenes[name]->onSceneLoadToMemory();
    }
    void unregisterScene(std::string nameId){
        if(scenes.find(nameId) != scenes.end()){
            delete scenes[nameId];
            scenes.erase(nameId);
        }
    }
    void setActiveScene(std::string nameId){
        if(actScene != nullptr){
            actScene->onSceneDeactivate();
        }
        actScene = scenes[nameId];
        actScene->onSceneActivate();
    }

    std::string getActiveSceneName(){
        return actScene->getName();
    }
    void deliverEvent(sf::Event& e){
        actScene->deliverEvent(e);
    }
    void runSceneFrame(double delta){
        actScene->draw(delta);
        if(cmdEnabled){
            ImGui::SetNextWindowPos(sf::Vector2f(0,0));
            ImGui::Begin("Tadzik CMD");
            ImGui::PushItemWidth(200);
            bool textInputted = ImGui::InputText("", cmdBuffer, 1024, ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::PopItemWidth();
            ImGui::End();
            if(textInputted){
                std::vector<std::string> v = eval();
                bool foundCommand = coreEval(v);
                foundCommand |= actScene->onConsoleUpdate(v);
                if(!foundCommand){
                    //to be implemented...
                }
            }
        }
    }
    void toogleCMD(){
        cmdEnabled = !cmdEnabled;
    }
    std::vector<std::string> eval(){
        std::string cmd = std::string(cmdBuffer);
        std::string tmp="";
        std::vector<std::string> vParsed;
        for(unsigned int i = 0; i < cmd.size(); i++){
            if(std::isspace(static_cast<unsigned char>(cmd[i]))){
                if(tmp != "" )
                    vParsed.push_back(tmp);
                tmp="";
            }
            else{
                tmp += cmd[i];
            }
        }
        if(tmp != "" )
            vParsed.push_back(tmp);

        return vParsed;
    }

    bool coreEval(std::vector<std::string> v){
        if(v.size()==0)
            return false;
        if(v.size()==2){
            if(v[0]=="loadLvl" || v[0]=="ll"){
                setActiveScene(v[1]);
                return true;
            }
        }
        if(v.size()==1 && v[0]=="fps") {
            showFps=!showFps;
        }
        return false;
    }

    void updateFpsCounter(double delta) {
        if (showFps) {
            if ((int)1000.0d/delta>100) fpsCounter.setColor(sf::Color::Green);
            else if ((int)1000.0d/delta>60) fpsCounter.setColor(sf::Color::Yellow);
            else if ((int)1000.0d/delta>30) fpsCounter.setColor(sf::Color::White);
            else fpsCounter.setColor(sf::Color::Red);
            fpsCounter.setString(Utils::stringify((int)1000.0d/delta));
            window->draw(fpsCounter);
        }
    }

private:
    std::unordered_map<std::string, Scene*> scenes;
    Scene* actScene= nullptr;
    sf::RenderWindow* window = nullptr;
    bool cmdEnabled=false;
    char cmdBuffer[1024];
    sf::Text fpsCounter;
    bool showFps = false;
};

#endif // SCENEMANAGER_HPP
