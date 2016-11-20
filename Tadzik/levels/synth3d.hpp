#ifndef SYNTH3D_HPP
#define SYNTH3D_HPP

#pragma once

#include "Scene.hpp"
#include "Utils.hpp"
#include <vector>

class camera
{
    public:
        camera(sf::RenderWindow* jack);
        void display();
        void wczyt(std::vector <sf::Vector3f> t1, std::vector <std::pair <int, int> > l1){
            terrain = t1; links = l1;
        }
    protected:
        std::vector <sf::Vertex> DrLines;
        sf::Vertex lina[2];
        std::vector <sf::Vector3f> terrain;
        std::vector <std::pair <int, int> > links;
        sf::RenderWindow* johnson;
};
camera::camera(sf::RenderWindow* jackson):
    johnson (jackson)
{}
void camera::display()
{
    for(int i=0;i<links.size();i++)
    {
        lina[0] = sf::Vertex(sf::Vector2f(0,140));
        lina[0] = sf::Vertex(sf::Vector2f(terrain[links[i].first].x, terrain[links[i].first].y));
        lina[1] = sf::Vertex(sf::Vector2f(terrain[links[i].second].x, terrain[links[i].second].y));
        johnson->draw(lina, 2, sf::Lines);
    }
}
class SYNTH3D: public Scene
{
public:
    SYNTH3D(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
    :Scene(_name,mgr,w),
    c(w)
    {}

    virtual void onSceneLoadToMemory() {
        if (!font.loadFromFile("files/Carnevalee_Freakshow.ttf")){
            std::cout << "cannot load font\n";
        }
        terrain.push_back(sf::Vector3f(-100,100,100)); // 0
        terrain.push_back(sf::Vector3f(100,100,100));  // 1
        terrain.push_back(sf::Vector3f(100,-100,100)); // 2
        terrain.push_back(sf::Vector3f(-100,-100,100));// 3
        terrain.push_back(sf::Vector3f(-100,100,300)); // 4
        terrain.push_back(sf::Vector3f(100,100,300));  // 5
        terrain.push_back(sf::Vector3f(100,-100,300)); // 6
        terrain.push_back(sf::Vector3f(-100,-100,300));// 7
        links.push_back({0,1});
        links.push_back({1,2});
        links.push_back({2,3});
        links.push_back({3,0});
        links.push_back({4,5});
        links.push_back({5,6});
        links.push_back({6,7});
        links.push_back({7,4});
        links.push_back({0,4});
        links.push_back({1,5});
        links.push_back({2,6});
        links.push_back({3,7});
        c.wczyt(terrain, links);
    }
    void line(int x1, int y1, int x2, int y2) {
        sf::Vertex v1[2] = {
            sf::Vertex (sf::Vector2f(x1, y1), sf::Color::Green),
            sf::Vertex (sf::Vector2f(x2, y2), sf::Color::Green)
        };
        window->draw(v1, 2, sf::Lines);
    }
    void point(int x, int y) {
        sf::Vertex v(sf::Vector2f(x, y), sf::Color::Green);
        //pkty.push_back(v);
        window->draw(&v, 1, sf::Points);
    }
    virtual void onSceneActivate(){}
    virtual void draw(double dt){
        //                              X   Y   Z

        c.display();
        //window->draw(&pkty[0], pkty.size(), sf::Points);
    }
protected:
    sf::Font font;
    std::vector <sf::Vector3f> terrain;
    std::vector <std::pair <int, int> > links;
    //std::vector <sf::Vertex> pkty;
    camera c;
};
#endif //SYNTH3D_HPP
