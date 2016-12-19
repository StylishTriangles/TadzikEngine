#include <SFML/Graphics.hpp>
#include <cmath>
#include <ctime>

#include "SceneManager.hpp"
#include "levels/trex.hpp"
#include "levels/clicker.hpp"
#include "levels/jumper.hpp"
#include "levels/synth3d.hpp"
#include "levels/mario.hpp"
#include "levels/levelselect.hpp"
#include "levels/shooter2D.hpp"
#include "levels/rpg.hpp"
#include "levels/isayparty.hpp"

#include <cstdlib>

int main(){
    srand(time(NULL));
    sf::ContextSettings settings;
    //settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Tadzik", sf::Style::Default, settings);
    sf::Image screenshot;
    screenshot.create(1280, 720, sf::Color::White);
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);
    SceneManager sceneManager(&window);
    //sceneManager.registerScene<TREX>("TREX", &window);
    //sceneManager.registerScene<CLICKER>("CLICKER", &window);
    //sceneManager.registerScene<JUMPER>("JUMPER", &window);
    //sceneManager.registerScene<SYNTH3D>("SYNTH3D", &window);
    //sceneManager.registerScene<MARIO>("MARIO", &window);
    //sceneManager.registerScene<LEVELSELECT>("LEVELSELECT", &window);
    //sceneManager.registerScene<RPG>("RPG", &window);
    sceneManager.registerScene<SHOOTER2D>("SHOOTER2D", &window);
    //sceneManager.registerScene<ISAYPARTY>("ISAYPARTY", &window);
    //sceneManager.setActiveScene("TREX");
    //sceneManager.setActiveScene("JUMPER");
    //sceneManager.setActiveScene("CLICKER");
    //sceneManager.setActiveScene("SYNTH3D");
    //sceneManager.setActiveScene("MARIO");
    //sceneManager.setActiveScene("LEVELSELECT");
    sceneManager.setActiveScene("SHOOTER2D");
    //sceneManager.setActiveScene("RPG");
    //sceneManager.setActiveScene("ISAYPARTY");
    sf::Clock deltaClock;
    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed )
                window.close();
            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F12) {
                screenshot = window.capture();
                screenshot.saveToFile("screenshots/screenshot.png");
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
