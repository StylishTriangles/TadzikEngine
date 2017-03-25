#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include "Scene.hpp"
#include "Utils.hpp"
#include "Common.hpp"
#include "Console.hpp"

#include <stdlib.h>
#include <unordered_map>
#include <string>
#include <iostream>

class SceneManager{
public:
    SceneManager(sf::RenderWindow* w)
    :window(w)
    {
        fpsCounter.setFont(Common::Font::Digital_7);
        fpsCounter.setString("60");
        fpsCounter.setOrigin(0, 30);
        fpsCounter.setPosition(20, window->getSize().y - 10);
        fpsCounter.setFillColor(sf::Color::Green);
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

    template<typename T>
    void justLaunchIt(std::string nameId) {
        registerScene<T>(nameId, window);
        setActiveScene(nameId);
    }

    std::string getActiveSceneName(){
        return actScene->getName();
    }
    void deliverEvent(sf::Event& e){
        actScene->deliverEvent(e);
    }
    void runSceneFrame(sf::Time delta){
        actScene->draw(delta);
        if(cmdEnabled){
            gameConsole.Draw("Tadzik CMD", 0);
            std::vector <std::string> tmp = gameConsole.passToSceneManager();
            if (tmp.size()>0)
                if (!coreEval(tmp))
                    gameConsole.AddLog("[error] Command not found\n");
                else
                    gameConsole.AddLog("Command executed succesfully\n");
        }
    }
    void toogleCMD(){
        cmdEnabled = !cmdEnabled;
    }

    bool coreEval(std::vector<std::string> v){
        if(v.size()==1 && v[0]=="fps") {
            showFps = !showFps;
            return true;
        }
        else if(v.size()==2){
            if(v[0]=="loadLvl" || v[0]=="ll"){
                setActiveScene(v[1]);
                return true;
            }
        }
        return false;
    }

    void getFPS(sf::Time delta) {
        if (showFps) {
            float smoothing = 0.95;
            fps = (fps*smoothing)+(sf::seconds(1)/delta*(1.0-smoothing));
            if (fps>60)
                fpsCounter.setFillColor(sf::Color::Green);
            else if (fps>30)
                fpsCounter.setFillColor(sf::Color::Yellow);
            else
                fpsCounter.setFillColor(sf::Color::Red);
            fpsCounter.setString(Utils::stringify((int)fps));
            window->draw(fpsCounter);
        }
    }

private:
    std::unordered_map<std::string, Scene*> scenes;
    Scene* actScene= nullptr;
    sf::RenderWindow* window = nullptr;

    sf::Text fpsCounter;
    bool showFps = true;
    float fps = 60;

    bool cmdEnabled=false;
    AppConsole gameConsole = AppConsole (&actScene);
};

#endif // SCENEMANAGER_HPP
