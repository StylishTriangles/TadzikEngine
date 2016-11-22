#ifndef SYNTH3D_HPP
#define SYNTH3D_HPP

#pragma once

#include "Scene.hpp"
#include "Utils.hpp"
#include <vector>
using namespace Utils;

struct wall
{
    std::vector <sf::Vector3f*> coord;
    sf::Color color=sf::Color::Green;
    bool trans=0;
    void push_back(sf::Vector3f &c)
    {
        coord.push_back(&c);
    }
    unsigned int size()
    {
        return coord.size();
    }
};

struct object
{
    std::vector <wall> wallie;
    void push_back(wall w)
    {
        wallie.push_back(w);
    }
    unsigned int size()
    {
        return wallie.size();
    }
};

class SYNTH3D;

class Camera
{
public:
    Camera(SYNTH3D* parent);
    void display();
protected:
    sf::Vertex lina[2];
    SYNTH3D* p;
    sf::Vector3f center;
    sf::Vector3f side;
    sf::Vector3f top;
    sf::Vector3f eye;
    sf::Vector3f vPlaneTop;
    sf::Vector3f vPlaneSide;
    void calcPlane();
    sf::Vector3f planeCross(sf::Vector3f* point);
    sf::Vector2f process(sf::Vector3f* point);
};

class SYNTH3D: public Scene
{
public:
    friend class Camera;
    SYNTH3D(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
        :Scene(_name,mgr,w), c(this)
    {}

    virtual void onSceneLoadToMemory()
    {
        if (!font.loadFromFile("files/Carnevalee_Freakshow.ttf"))
        {
            std::cout << "cannot load font\n";
        } //                             X   Y   Z
        terrain.push_back(sf::Vector3f(-100,100,100)); // 0
        terrain.push_back(sf::Vector3f(100,100,100));  // 1
        terrain.push_back(sf::Vector3f(100,-100,100)); // 2
        terrain.push_back(sf::Vector3f(-100,-100,100));// 3
        terrain.push_back(sf::Vector3f(-100,100,300)); // 4
        terrain.push_back(sf::Vector3f(100,100,300));  // 5
        terrain.push_back(sf::Vector3f(100,-100,300)); // 6
        terrain.push_back(sf::Vector3f(-100,-100,300));// 7
        object cube;
        wall wallie[6]; // 0 - Rear Wall    |   1 - Right Wall     |   2 - Upper Wall     |   3 - Front Wall      | 4 - Left Wall     |   5 - Bottom Wall

        wallie[0].push_back(terrain[4]);
        wallie[0].push_back(terrain[5]);
        wallie[0].push_back(terrain[6]);
        wallie[0].push_back(terrain[7]);

        wallie[1].push_back(terrain[1]);
        wallie[1].push_back(terrain[2]);
        wallie[1].push_back(terrain[6]);
        wallie[1].push_back(terrain[5]);

        wallie[2].push_back(terrain[0]);
        wallie[2].push_back(terrain[1]);
        wallie[2].push_back(terrain[5]);
        wallie[2].push_back(terrain[4]);

        wallie[3].push_back(terrain[0]);
        wallie[3].push_back(terrain[1]);
        wallie[3].push_back(terrain[2]);
        wallie[3].push_back(terrain[3]);

        wallie[4].push_back(terrain[0]);
        wallie[4].push_back(terrain[3]);
        wallie[4].push_back(terrain[7]);
        wallie[4].push_back(terrain[4]);

        wallie[5].push_back(terrain[2]);
        wallie[5].push_back(terrain[3]);
        wallie[5].push_back(terrain[7]);
        wallie[5].push_back(terrain[6]);

        for(int i=0;i<6;i++)
            cube.push_back(wallie[i]);
        world.push_back(cube);
    }
    virtual void onSceneActivate() {}
    virtual void draw(double dt)
    {
        c.display();
    }
protected:
    sf::Font font;
    std::vector <sf::Vector3f> terrain;
    std::vector <object> world;
    Camera c;
};
Camera::Camera(SYNTH3D* parent):
    p(parent),
    center({0,0,0}),
    eye({0,0,-1}),
    side({1,0,0}),
    top({0,1,0})
{}

sf::Vector3f Camera::planeCross(sf::Vector3f* point)
{
    sf::Vector3f temp = *point - eye;
    return eye + temp*det3f(vPlaneTop, vPlaneSide, eye)/det3f(vPlaneTop,vPlaneSide, -temp);
}

void  Camera::calcPlane()
{
    vPlaneTop = top - center;
    vPlaneSide = side - center;
}

sf::Vector2f Camera::process(sf::Vector3f* point)
{
    float scale = 100, movement = 300;
    return {planeCross(point).x*scale + movement, planeCross(point).y*scale + movement};
}

void Camera::display()
{
    p->window->clear();
    calcPlane();
    for(int i=0; i<p->world.size();i++)
    {
        for(int j=0; j< p->world[i].size(); j++)
            {
                int mod = p->world[i].wallie[j].size();
                for(int k=0; k< p->world[i].wallie[j].size(); k++)
                {
                    lina[0] = sf::Vertex(process(p->world[i].wallie[j].coord[k]), p->world[i].wallie[j].color);
                    lina[1] = sf::Vertex(process(p->world[i].wallie[j].coord[(k+1)%mod]), p->world[i].wallie[j].color);
                    //lina[0] = sf::Vertex(sf::Vector2f( p->world[i].wallie[j].coord[k]->x, p->world[i].wallie[j].coord[k]->y));
                    //lina[1] = sf::Vertex(sf::Vector2f( p->world[i].wallie[j].coord[(k+1)%mod]->x, p->world[i].wallie[j].coord[(k+1)%mod]->y));
                    p->window->draw(lina, 2, sf::Lines);
                }
            }
    }
}

#endif //SYNTH3D_HPP
