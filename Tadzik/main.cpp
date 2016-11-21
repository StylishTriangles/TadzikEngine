#include <SFML/Graphics.hpp>
#include <cmath>

#include "SceneManager.hpp"
#include "levels/trex.hpp"
#include "levels/clicker.hpp"
#include "levels/jumper.hpp"
#include "levels/synth3d.hpp"
#include "levels/mario.hpp"

#include <cstdlib>

int main(){
    srand(0);
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Tadzik");
    window.setFramerateLimit(60);
    SceneManager sceneManager(&window);
    sceneManager.registerScene<TREX>("TREX", &window);
    sceneManager.registerScene<CLICKER>("CLICKER", &window);
    sceneManager.registerScene<JUMPER>("JUMPER", &window);
    sceneManager.registerScene<SYNTH3D>("SYNTH3D", &window);
    sceneManager.registerScene<MARIO>("MARIO", &window);
    //sceneManager.setActiveScene("TREX");
    //sceneManager.setActiveScene("JUMPER");
    //sceneManager.setActiveScene("CLICKER");
    sceneManager.setActiveScene("SYNTH3D");
    //sceneManager.setActiveScene("MARIO");
    sf::Clock deltaClock;
    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed )
                 window.close();
            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
            else{
                sceneManager.deliverEvent(event);
            }
        }

		sceneManager.runSceneFrame(deltaClock.getElapsedTime().asMilliseconds());
		deltaClock.restart();
        window.display();
    }
    return 0;
}
