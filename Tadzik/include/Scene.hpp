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
    virtual std::string printToConsole(){std::string s; return s;}

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

protected:
    std::string name;
    SceneManager* sceneManager=nullptr;
    sf::RenderWindow* window;
    ImVector <char*> consoleCommands;
};

#endif // SCENE_HPP
