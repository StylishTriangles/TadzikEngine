#include <SFML/Graphics.hpp>
#include <cmath>

#include "SceneManager.hpp"
#include "levels/examples1.hpp"
#include "levels/examples2.hpp"

int main(){
    sf::RenderWindow window(sf::VideoMode(320, 240), "Tadzik");
    SceneManager sceneManager(&window);
    sceneManager.registerScene<ExampleScene1>("Example1", &window);
    sceneManager.registerScene<ExampleScene2>("Example2", &window);
    sceneManager.setActiveScene("Example1");
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

		sceneManager.runSceneFrame(deltaClock.restart());
        window.display();
    }
    return 0;
}
