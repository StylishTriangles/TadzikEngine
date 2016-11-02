#include <SFML/Graphics.hpp>
#include <cmath>

#include "SceneManager.hpp"
#include "levels/examples1.hpp"
#include "levels/examples2.hpp"
#include "levels/example_animated_sprite.hpp"
#include "levels/trex.hpp"
#include "levels/clicker.hpp"
#include "levels/jumper.hpp"

#include <cstdlib>

int main(){
    srand(0);
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Tadzik");
    window.setFramerateLimit(60);
    SceneManager sceneManager(&window);
    sceneManager.registerScene<ExampleScene1>("Example1", &window);
    sceneManager.registerScene<ExampleScene2>("Example2", &window);
    sceneManager.registerScene<AnimatedSpriteScene>("AnimSprite", &window);
    sceneManager.registerScene<TREX>("TREX", &window);
    sceneManager.registerScene<CLICKER>("CLICKER", &window);
    sceneManager.registerScene<JUMPER>("JUMPER", &window);
    //sceneManager.setActiveScene("TREX");
    sceneManager.setActiveScene("JUMPER");
    //sceneManager.setActiveScene("CLICKER");
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
