#ifndef SCENE_HPP
#define SCENE_HPP

#include <string>
#include <iostream>

class SceneManager;

class Scene{
public:
    Scene(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
    :name(_name)
    {
        setSceneManager(mgr);
        setSceneWindow(w);
    }

    virtual ~Scene(){}

    virtual void onSceneLoadToMemory(){};
    virtual void onSceneActivate(){}
    virtual void draw(double deltaTime){}
    virtual void deliverEvent(sf::Event&){}
    virtual void onSceneDeactivate(){}
    virtual bool onConsoleUpdate(std::vector<std::string> args){}

    std::string getName(){
        return name;
    }

    void setSceneManager(SceneManager* mgr){
        sceneManager = mgr;
    }

    void setSceneWindow(sf::RenderWindow* w){
        window = w;
    }

    void clearWindow(float r, float g, float b, float a){

    }

    ImVector<char*> getSceneCommands() {
        return consoleCommands;
    }

    void printToConsole(std::string s) {
        char* tmp = &s[0u];
        consoleBuff.push_back(tmp);
    }

    ImVector <char*> getBuffer() {
        ImVector <char*> tmp = consoleBuff;
        consoleBuff.clear();
        return tmp;
    }

protected:
    std::string name;
    SceneManager* sceneManager=nullptr;
    sf::RenderWindow* window;
    ImVector <char*> consoleCommands;
    ImVector <char*> consoleBuff;
};

#endif // SCENE_HPP
