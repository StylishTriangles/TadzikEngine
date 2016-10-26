#include <SFML/Graphics.hpp>
#include <cmath>

#include "SceneManager.hpp"
#include "levels/examples1.hpp"
#include "levels/examples2.hpp"
#include "levels/example_animated_sprite.hpp"
#include "levels/trex.hpp"

int main(){
    sf::RenderWindow window(sf::VideoMode(320, 240), "Tadzik");
    window.setFramerateLimit(60);
    SceneManager sceneManager(&window);
    sceneManager.registerScene<ExampleScene1>("Example1", &window);
    sceneManager.registerScene<ExampleScene2>("Example2", &window);
    sceneManager.registerScene<AnimatedSpriteScene>("AnimSprite", &window);
    sceneManager.registerScene<TREX>("TREX", &window);
    sceneManager.setActiveScene("TREX");
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
