#include "include/imgui/imgui.h"
#include "include/imgui/imgui-SFML.h"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <iostream>

#include "../include/Common.hpp"
#include "../include/SceneManager.hpp"
#include "../include/Launcher.hpp"

///temporary workaround
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


int main(){
    Common::loadFonts();
    srand(time(NULL));
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Tadzik", sf::Style::Default, settings);
    //sf::RenderWindow window(sf::VideoMode(1280, 960), "Tadzik", sf::Style::Fullscreen, settings);
    //sf::RenderWindow window(sf::VideoMode(1366, 768), "Tadzik", sf::Style::Fullscreen, settings);
    //sf::RenderWindow window(sf::VideoMode(1920, 1080), "Tadzik", sf::Style::Fullscreen, settings);
    //sf::RenderWindow window(sf::VideoMode(640, 480), "Tadzik", sf::Style::Default, settings);
    //window.setFramerateLimit(400);
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);
    window.setVerticalSyncEnabled(true);
    ImGui::SFML::Init(window);
    SceneManager sceneManager(&window);

    sceneManager.registerScene<LAUNCHER>("LAUNCHER", &window);
    sceneManager.registerScene<TREX>("TREX", &window);
    //sceneManager.registerScene<CLICKER>("CLICKER", &window);
    sceneManager.registerScene<JUMPER>("JUMPER", &window);
    sceneManager.registerScene<SYNTH3D>("SYNTH3D", &window);
    sceneManager.registerScene<MARIO2>("MARIO2", &window);
    sceneManager.registerScene<RPG>("RPG", &window);
    sceneManager.registerScene<SHOOTER2D>("SHOOTER2D", &window);
    sceneManager.registerScene<ISAYPARTY>("ISAYPARTY", &window);

    sceneManager.setActiveScene("LAUNCHER");

    sf::Clock deltaClock;
    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            ImGui::SFML::ProcessEvent(event);
            if(event.type == sf::Event::Closed )
                window.close();
            else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                if (sceneManager.getActiveScene()->getName()=="LAUNCHER") {
                    window.close();
                }
                sceneManager.getActiveScene()->setPaused(!sceneManager.getActiveScene()->isPaused());
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

        sf::Time dT = deltaClock.getElapsedTime();
        deltaClock.restart();
        ImGui::SFML::Update(window, dT);
        if(sceneManager.getActiveScene()->isPaused() && sceneManager.getActiveSceneName() != "LAUNCHER"){
            ImGui::Begin("Paused");
            if(ImGui::Button("Resume")){
                sceneManager.getActiveScene()->setPaused(false);
                sceneManager.setActiveScene(sceneManager.getActiveSceneName());
            }
            if(ImGui::Button("Menu")){
                sceneManager.getActiveScene()->setPaused(false);
                sceneManager.setActiveScene("LAUNCHER");
            }
            ImGui::End();
        }
        else
            sceneManager.runSceneFrame(dT);

        sceneManager.getFPS(dT);

        window.resetGLStates();
        ImGui::Render();
        window.display();
    }
    ImGui::SFML::Shutdown();
    return 0;
}
