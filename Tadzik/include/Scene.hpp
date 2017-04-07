#ifndef SCENE_HPP
#define SCENE_HPP

#include <string>
#include <iostream>

#include "Common.hpp"

struct coloredText
{
    coloredText(char* t, sf::Color c = sf::Color::White) {
        text = t;
        color = c;
    }
    char* text;
    sf::Color color = sf::Color::White;
};

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

    virtual void onSceneLoadToMemory(){}
    virtual void onSceneActivate(){}
    virtual void draw(sf::Time deltaTime){}
    virtual void deliverEvent(sf::Event&){}
    virtual void onSceneDeactivate(){}
    virtual bool onConsoleUpdate(std::vector<std::string> args) {return false;}

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

    ImVector<char*> getSceneCommands() const {
        return consoleCommands;
    }

    void printToConsole(std::string s, sf::Color color = sf::Color::White) {
        char* tmp = &s[0u];
        consoleBuff.push_back(coloredText(Strdup(tmp), color));
    }

    ImVector <coloredText>* getBuffer() {
        return &consoleBuff;
    }

    void clearBuffer() {
        consoleBuff.clear();
    }

    bool isUnlocked(){
        return unlocked;
    }

    void setLocked(bool l){
        unlocked = l;
    }

    bool isPaused(){
        return paused;
    }

    void setPaused(bool l){
        paused = l;
    }

    sf::RenderWindow* window;
protected:
    std::string name;
    SceneManager* sceneManager=nullptr;
    ImVector <char*> consoleCommands;
    ImVector <coloredText> consoleBuff;
    bool unlocked=false;
    bool paused=false;
};

#endif // SCENE_HPP
