#include "include/imgui/imgui.h"
#include "include/imgui/imgui-SFML.h"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <iostream>

#include "../include/Common.hpp"

//******
//temporary workaround
namespace sf{
class KeyboardHacked: public sf::Keyboard{
public:
    static bool isKeyPressed (Key key){
        if(ImGui::GetIO().WantCaptureKeyboard){
            return false;
        }
        return sf::Keyboard::isKeyPressed(key);
    }
};
}
#define Keyboard KeyboardHacked
//*****

#include "include/SceneManager.hpp"
#include "levels/trex.hpp"
#include "levels/clicker.hpp"
#include "levels/jumper.hpp"
#include "levels/synth3d.hpp"
#include "levels/mario.hpp"
#include "levels/levelselect.hpp"
#include "levels/shooter2D.hpp"
#include "levels/rpg.hpp"
#include "levels/isayparty.hpp"
#include "levels/frogger.hpp"

int main(){
    Common::loadFonts();
    srand(time(NULL));
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Tadzik", sf::Style::Default, settings);
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);
    ImGui::SFML::Init(window);
    SceneManager sceneManager(&window);
    //sceneManager.registerScene<TREX>("TREX", &window);
    //sceneManager.registerScene<CLICKER>("CLICKER", &window);
    //sceneManager.registerScene<JUMPER>("JUMPER", &window);
    //sceneManager.registerScene<SYNTH3D>("SYNTH3D", &window);
    //sceneManager.registerScene<MARIO>("MARIO", &window);
    //sceneManager.registerScene<LEVELSELECT>("LEVELSELECT", &window);
   sceneManager.registerScene<RPG>("RPG", &window);
    //sceneManager.registerScene<SHOOTER2D>("SHOOTER2D", &window);
    //sceneManager.registerScene<ISAYPARTY>("ISAYPARTY", &window);

   //sceneManager.setActiveScene("TREX");
    //sceneManager.setActiveScene("JUMPER");
    //sceneManager.setActiveScene("CLICKER");
    //sceneManager.setActiveScene("SYNTH3D");
    //sceneManager.setActiveScene("MARIO");
    //sceneManager.setActiveScene("LEVELSELECT");
   // sceneManager.setActiveScene("SHOOTER2D");
sceneManager.setActiveScene("RPG");
    //sceneManager.setActiveScene("ISAYPARTY");


    sf::Clock deltaClock;
    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            ImGui::SFML::ProcessEvent(event);
            if(event.type == sf::Event::Closed )
                window.close();
            else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
            else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F12) {
                window.capture().saveToFile("screenshots/"+Utils::getDate()+".png");
            }
            else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tilde) {
                sceneManager.toogleCMD();
            }
            else{
                sceneManager.deliverEvent(event);
            }
        }
        ImGui::SFML::Update(window, deltaClock.getElapsedTime());
		sceneManager.runSceneFrame(deltaClock.getElapsedTime().asMilliseconds());
        sceneManager.getFPS(deltaClock.getElapsedTime());

        deltaClock.restart();
        window.resetGLStates();
        ImGui::Render();
        window.display();
    }
    ImGui::SFML::Shutdown();
    return 0;
}
