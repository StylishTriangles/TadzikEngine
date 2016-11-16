#pragma once

#include "Scene.hpp"
class camera
{
    //Tutaj lecisz
};
class synth_3d: public Scene
{
public:
    synth_3d(std::string _name, SceneManager* mgr, sf::RenderWindow* w):
        Scene(_name,mgr,w){}
    virtual void onSceneLoadToMemory() {
        if (!font.loadFromFile("files/Carnevalee_Freakshow.ttf")){
            std::cout << "cannot load font\n";
        }
    }
    virtual void onSceneActivate(){}
    virtual void draw(double dt){
    //to jest Twoj main, jedna klatka
    }
protected:
    // int a; etc
    // sf::Window to po prostu window
};

