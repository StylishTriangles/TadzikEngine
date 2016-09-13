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

    virtual void onSceneActivate(){}
    virtual void draw(sf::Time& deltaTime){}
    virtual void deliverEvent(sf::Event&){}
    virtual void onSceneDeactivate(){}

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

protected:
    std::string name;
    SceneManager* sceneManager=nullptr;
    sf::RenderWindow* window;
};

#endif // SCENE_HPP

