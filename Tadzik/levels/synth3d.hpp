#ifndef SYNTH3D_HPP
#define SYNTH3D_HPP

#pragma once

#include "Scene.hpp"
#include "Utils.hpp"
#include <vector>
using namespace Utils;

# define M_PI           3.14159265358979323846  /* pi */

struct wall
{
    std::vector <sf::Vector3f*> coord;
    sf::Color color=sf::Color::Green;
    bool trans=0;
    float pSize=500;
    void push_back(sf::Vector3f &c)
    {
        coord.push_back(&c);
    }
    unsigned int size() const
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
    void display();                     //Wyswietlanie
    void setPos(sf::Vector3f posGiven); //Ustaw pozycje kamery
    void setAngle(sf::Vector3f aGiven); //Ustaw kat skierowania kamery
    void setEyeDistance(float distance);//Ustaw odleglosc oka od plaszczyzny
protected:
    sf::Vertex lina[2];
    SYNTH3D* p;
    float screenSize, viewAngle, eye;
    sf::Vector3f position;      //Pozycja kamery
    sf::Vector3f angle;
    sf::Vector3f planeCross(sf::Vector3f* point); //Policz przeciecie punktu z ekranem
    sf::Vector2f process(sf::Vector3f* point);    //Podaj wspolrzedne na ekranie
    sf::Vector3f vecTransform(sf::Vector3f vec);
    void drawLine(sf::Vector2f vec1, sf::Vector2f vec2, float size1, float size2, sf::Color color);
    void drawDot(sf::Vector2f spot, float size, sf::Color color);
    void drawWall(wall const& wallie);
    float dotSize(float dot, sf::Vector3f vec);
};

class SYNTH3D: public Scene
{
public:
    friend class Camera;
    SYNTH3D(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
        :Scene(_name,mgr,w), c(this), cameraPos({0, 0, -200}), cameraAngle({0, 0, 0}), eyeDistance(-10)
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
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                cameraPos.x-=1;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                cameraPos.x+=1;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
                cameraPos.z+=1;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                cameraPos.z-=1;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
                cameraPos.y+=1;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
                cameraPos.y-=1;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                cameraAngle.y+=0.5;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                cameraAngle.y-=0.5;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                cameraAngle.x-=0.5;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                cameraAngle.x+=0.5;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
                cameraAngle.z-=0.5;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
                cameraAngle.z+=0.5;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
            {
                for(int i=0; i<8;i++)
                    terrain[i].z--;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::K))
            {
                for(int i=0; i<8;i++)
                    terrain[i].x++;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
                eyeDistance+=0.1;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
                eyeDistance-=0.1;

            c.setPos(cameraPos);
            c.setAngle(cameraAngle);
            c.setEyeDistance(eyeDistance);
            c.display();
    }
protected:
    sf::Font font;
    std::vector <sf::Vector3f> terrain;
    std::vector <object> world;
    Camera c;
    sf::Vector3f cameraPos;
    sf::Vector3f cameraAngle;
    float eyeDistance;
};
Camera::Camera(SYNTH3D* parent):
    p(parent),
    position({0, 0, -400}),
    eye(-80),
    angle({0,0,0}),
    screenSize(3)
{}

sf::Vector3f Camera::planeCross(sf::Vector3f* point)
{
    sf::Vector3f temp = vecTransform(*point - position), vEye = {0, 0, eye};
    return vEye + temp*((-eye)/(temp.z - eye));
}

sf::Vector2f Camera::process(sf::Vector3f* point)
{
    float scale = 100;
    sf::Vector3f tempPlane = planeCross(point);
    return {tempPlane.x*scale + p->window->getSize().x/2, tempPlane.y*scale + p->window->getSize().y/2};
}

void Camera::setPos(sf::Vector3f posGiven)
{
    position = posGiven;
}

void Camera::setAngle(sf::Vector3f aGiven)
{
    angle.x = aGiven.x*(M_PI/180);
    angle.y = aGiven.y*(M_PI/180);
    angle.z = aGiven.z*(M_PI/180);
}

sf::Vector3f Camera::vecTransform(sf::Vector3f vec)
{
    sf::Vector3f target = vec;
    //Yaw: (y axis)
    target.x = vec.x*cosf(angle.y) + vec.z*sinf(angle.y);
    target.z = vec.z*cosf(angle.y) - vec.x*sinf(angle.y);
    //Pitch; (x axis)
    vec = target;
    target.y = vec.y*cosf(angle.x) - vec.z*sinf(angle.x);
    target.z = vec.y*sinf(angle.x) + vec.z*cosf(angle.x);
    //Roll: (z axis)
    vec = target;
    target.x = vec.x*cosf(angle.z) - vec.y*sinf(angle.z);
    target.y = vec.x*sinf(angle.z) + vec.y*cosf(angle.z);
    return target;
}

void Camera::setEyeDistance(float distance)
{
    eye = distance;
}

float Camera::dotSize(float dot, sf::Vector3f vec)
{
    return ((-eye)/vecTransform(vec - position).z)*dot;
}

void Camera::drawLine(sf::Vector2f vec1, sf::Vector2f vec2, float size1, float size2, sf::Color color)
{
    sf::ConvexShape line;
    line.setPointCount(4);
    sf::Vector2f norm;
    float temp, lenght;

    norm = (vec1 - vec2);
    lenght = sqrt(norm.x*norm.x + norm.y*norm.y);
    norm.x /= lenght, norm.y /= lenght;
    temp = norm.x, norm.x = -norm.y, norm.y = temp;

    line.setPoint(0, vec1 + norm*size1);
    line.setPoint(1, vec2 + norm*size2);
    line.setPoint(2, vec2 - norm*size2);
    line.setPoint(3, vec1 - norm*size1);
    line.setFillColor(color);
    p->window->draw(line);
}

void Camera::drawDot(sf::Vector2f spot, float size, sf::Color color)
{
    sf::CircleShape circle(size);
        circle.setFillColor(color);
        spot.x -= size, spot.y -= size;
        circle.setPosition(spot);
        p->window->draw(circle);
}

void Camera::drawWall(wall const& wallie)
{
    /*auto drawDot = [](sf::Vector2f spot, float size, sf::Color color) -> void
    {};*/

    for(int i=0;i<wallie.size();i++)
    {
        //drawLine() od process od punktow
        drawLine(process(wallie.coord[i]),
                 process(wallie.coord[(i+1)%wallie.size()]),
                 dotSize(wallie.pSize, *wallie.coord[i]),
                 dotSize(wallie.pSize, *wallie.coord[(i+1)%wallie.size()]),
                 //wallie.pSize,
                 //wallie.pSize,
                 wallie.color);
        drawDot(process(wallie.coord[i]), dotSize(wallie.pSize, *wallie.coord[i]), wallie.color);
        //drawDot(process(wallie.coord[i]), wallie.pSize, wallie.color);
    }
}

void Camera::display()
{
    p->window->clear();
    std::cout << "X: " << position.x << " Y: " << position.y << " Z: " << position.z << " Yaw: " << angle.y*180/M_PI << " Pitch: " << angle.x*180/M_PI << "\n" << " Eye: " << eye;
    for(int i=0; i<p->world.size();i++)
    {
        for(int j=0; j< p->world[i].size(); j++)
            {

                drawWall(p->world[i].wallie[j]);

                /*sf::ConvexShape line;
                line.setPointCount(4);
                for(int k=0; k< p->world[i].wallie[j].size(); k++)
                    line.setPoint(k, process(p->world[i].wallie[j].coord[k]));
                //line.setFillColor(sf::Color(j*100, j*73, j*22));
                line.setFillColor(sf::Color::Transparent);
                line.setOutlineThickness(-15);
                p->window->draw(line);
                //--------------------
                int mod = p->world[i].wallie[j].size();
                for(int k=0; k< p->world[i].wallie[j].size(); k++)
                {
                    lina[0] = sf::Vertex(process(p->world[i].wallie[j].coord[k]), p->world[i].wallie[j].color);
                    lina[1] = sf::Vertex(process(p->world[i].wallie[j].coord[(k+1)%mod]), p->world[i].wallie[j].color);
                    p->window->draw(lina, 2, sf::Lines);
                }*/
            }
    }
}

#endif //SYNTH3D_HPP
