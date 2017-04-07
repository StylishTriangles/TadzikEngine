#ifndef LAUNCHER_HPP
#define LAUNCHER_HPP

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <sstream>

#include "../include/Scene.hpp"
#include "../include/AnimatedSprite.hpp"
#include "../include/Utils.hpp"
#include "../include/Common.hpp"

#include "../levels/trex.hpp"
#include "../levels/clicker.hpp"
#include "../levels/jumper.hpp"
#include "../levels/synth3d.hpp"
#include "../levels/marioRewritten.hpp"
#include "../levels/levelselect.hpp"
#include "../levels/shooter2D.hpp"
#include "../levels/rpg.hpp"
#include "../levels/isayparty.hpp"

class LAUNCHER: public Scene{
public:
    LAUNCHER(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
    :Scene(_name, mgr, w)
    {}

    virtual void onSceneLoadToMemory() {

    }

    virtual void onSceneActivate() {
        for(auto it = sceneManager->scenes.begin(); it != sceneManager->scenes.end(); it++){
            Scene* s = it->second;
            std::cout << s->getName() << "\n";
        }
    }

    void deliverEvent(sf::Event& event){
        if (event.type == sf::Event::MouseButtonPressed) {

        }
        if (event.type == sf::Event::MouseButtonReleased) {

        }
    }

    virtual void draw(sf::Time deltaTime) {
        window->clear();
        /*ImGui::SetNextWindowPos(sf::Vector2u((p->window->getSize().x - creationWindowSize.x)/2, (p->window->getSize().y - creationWindowSize.y)/2));
        ImGui::Begin("Hello, world!");
        ImGui::Button("Look at this pretty button");
        ImGui::End();*/
        ImGui::Begin("ARCADE MODE!");
        int ctr=0;
        for(auto it = sceneManager->scenes.begin(); it != sceneManager->scenes.end(); it++){
            ImGui::SetNextWindowPos(sf::Vector2u(window->getSize().x - 100, window->getSize().y - 200+ctr*30));
            if(it->second->isUnlocked())
                ImGui::PushStyleColor(ImGuiCol_WindowBg, sfColorToImColor(sf::Color(0, 255, 0, 255)));
            else
                ImGui::PushStyleColor(ImGuiCol_WindowBg, sfColorToImColor(sf::Color(255, 255, 0, 255)));
            if(ImGui::Button(it->second->getName().c_str())){
                sceneManager->setActiveScene(it->second->getName());
            }
            ImGui::PopStyleColor();
        }
        ImGui::End();
    }

protected:

};

#endif // LAUNCHER_HPP
