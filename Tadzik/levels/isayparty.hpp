#ifndef ISAYPARTY_HPP
#define ISAYPARTY_HPP

#include "Scene.hpp"
#include "AnimatedSprite.hpp"
#include "Utils.hpp"

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>

struct StepPose{
    StepPose(int tempo, int k)
    :tempoPosition(tempo), key(k)
    {}
    int tempoPosition;
    int key;
};

struct DanceStep{
    DanceStep(){}
    DanceStep(AnimatedSprite* sprite, const std::initializer_list<int>& tempoList, const std::initializer_list<int>& keyList)
    :animation(sprite)
    {
        std::vector<int> vTempo, vKey;
        for(int tmp: tempoList)vTempo.push_back(tmp);
        for(int tmp: keyList)vKey.push_back(tmp);
        vecPoses.clear();
        for(int i = 0; i < vKey.size(); i++){
            vecPoses.push_back(StepPose(vTempo[i], vKey[i]));
        }
    }
    AnimatedSprite* animation;
    std::vector<StepPose> vecPoses;
};

class ISAYPARTY: public Scene{
public:
    ISAYPARTY(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
    :Scene(_name, mgr, w)
    {}

    virtual void onSceneLoadToMemory(){
        double dist = window->getSize().x/(double)(tempo+1.0);
        for(int i = 0; i < tempo; i++){
            sf::CircleShape shp(10);
            shp.setFillColor(sf::Color::Blue);
            shp.setPosition(dist+i*dist, 30);
            vCircles.push_back(shp);
        }
        circlePointer = sf::CircleShape(10);
        circlePointer.setFillColor(sf::Color::Red);

        step1 = DanceStep(nullptr, {3, 6}, {1, 2});
        step2 = DanceStep(nullptr, {1, 2, 5, 8}, {3, 2, 4, 4});
    }

    virtual void onSceneActivate(){
        window->setTitle("Tadzik ~ISAyPartYyouSAYparTyDisOdisCOPartYparTy");
    }

    void deliverEvent(sf::Event& event){
        if (event.type == sf::Event::KeyReleased) {
            if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down){

            }
        }
    }

    virtual void draw(double deltaTime){
        tmpTime += deltaTime;
        double percent = (tmpTime/1000.0)/(60.0/bpm);
        if(tmpTime/1000.0 > 60.0/bpm){
            tmpTime=0.0;
        }

        window->clear(sf::Color::Black);
        for(int i = 0; i < vCircles.size(); i++){
            bool isStepPose=false;
            for(int j = 0; j < actStep->vecPoses.size(); j++){
                if(actStep->vecPoses[j].tempoPosition-1 == i){
                    isStepPose=true;
                }
            }
            if(isStepPose)
                vCircles[i].setFillColor(sf::Color::Blue);
            else
                vCircles[i].setFillColor(sf::Color::Green);

            window->draw(vCircles[i]);
        }
        circlePointer.setPosition(window->getSize().x*percent, 30);
        window->draw(circlePointer);
    }

protected:
    int tempo = 8;
    double bpm=40;          //uderzenia na minute
    double tmpTime=0.0;     //w sekundach
    std::vector<sf::CircleShape> vCircles;
    sf::CircleShape circlePointer;

    DanceStep step1;
    DanceStep step2;
    DanceStep* actStep = &step1;
};

#endif // ISAYPARTY_HPP





