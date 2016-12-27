#include "../glad/glad.h"

#include "include/imgui/imgui.h"
#include "include/imgui/imgui-SFML.h"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <ctime>


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
#include "levels/arrrr.hpp"

#include <cstdlib>
#include <iostream>

int main(){
    srand(time(NULL));
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 8;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Tadzik", sf::Style::Default, settings);
    if(!gladLoadGL()){
        std::cout << "Kurwa spock\n";
    }
    else{
        int v1=31337, v2=1337;
        glGetIntegerv(GL_MAJOR_VERSION, &v1);
        glGetIntegerv(GL_MINOR_VERSION, &v2);
        std::cout << "versions vol1: " << v1 << " " << v2 << "\n";
        std::cout << "OpenGL Version: " << GLVersion.major << ", " << GLVersion.minor << "\n";
    }

    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);
    ImGui::SFML::Init(window);
    int v1=31337, v2=1337;
    glGetIntegerv(GL_MAJOR_VERSION, &v1);
    glGetIntegerv(GL_MINOR_VERSION, &v2);
    std::cout << "versions (imGUI): " << v1 << " " << v2 << "\n";
    std::cout << "OpenGL Version (imGUI): " << GLVersion.major << ", " << GLVersion.minor << "\n";

    SceneManager sceneManager(&window);
    sceneManager.registerScene<TREX>("TREX", &window);
    sceneManager.registerScene<CLICKER>("CLICKER", &window);
    sceneManager.registerScene<JUMPER>("JUMPER", &window);
    sceneManager.registerScene<SYNTH3D>("SYNTH3D", &window);
    sceneManager.registerScene<MARIO>("MARIO", &window);
    sceneManager.registerScene<LEVELSELECT>("LEVELSELECT", &window);
    sceneManager.registerScene<RPG>("RPG", &window);
    sceneManager.registerScene<SHOOTER2D>("SHOOTER2D", &window);
    sceneManager.registerScene<ISAYPARTY>("ISAYPARTY", &window);
    sceneManager.registerScene<ARRRR>("ARRRR", &window);
    //sceneManager.setActiveScene("JUMPER");
    //sceneManager.setActiveScene("CLICKER");
    //sceneManager.setActiveScene("SYNTH3D");
    //sceneManager.setActiveScene("MARIO");
    //sceneManager.setActiveScene("LEVELSELECT");
    //sceneManager.setActiveScene("SHOOTER2D");
    sceneManager.setActiveScene("ARRRR");
    sf::Clock deltaClock;
    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            ImGui::SFML::ProcessEvent(event);
            if(event.type == sf::Event::Closed )
                 window.close();
            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tilde) {
                sceneManager.toogleCMD();
            }
            else{
                sceneManager.deliverEvent(event);
            }
        }
        ImGui::SFML::Update(window, deltaClock.getElapsedTime());
		sceneManager.runSceneFrame(deltaClock.getElapsedTime().asMilliseconds());
        deltaClock.restart();

        window.resetGLStates();
        ImGui::Render();
        window.display();

        //std::cout << "fps: " << sceneManager.getFPS() << "\n";
    }
    ImGui::SFML::Shutdown();
    return 0;
}
