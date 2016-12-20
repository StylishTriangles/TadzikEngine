#ifndef SYNTH3D_HPP
#define SYNTH3D_HPP

#pragma once

#include "Scene.hpp"
#include "Utils.hpp"
#include <vector>
#include <cmath>
using namespace Utils;

#ifndef M_PI
# define M_PI           3.14159265358979323846  /* pi */
#endif
template<typename realType>
inline realType toRad(realType angle)
{
    return angle * (realType)0.017453292519943295769;
}

struct wall
{
    std::vector <sf::Vector3f*> coord;
    sf::Color color=sf::Color::Green;
    sf::Color trans=sf::Color::Transparent;
    float pSize=100;
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
    std::vector <sf::Vertex> triangleArray;
    std::vector <sf::Vector2f> circleDef;
    void initCircleDef(int n);
    SYNTH3D* p;
    float screenSize, viewAngle, eye, scale;
    sf::Vector3f position, angle;
    sf::Vector3f planeCross(sf::Vector3f vec1, sf::Vector3f vec2);
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
        :Scene(_name,mgr,w), c(this), cameraPos({0, 0, -200}), cameraAngle({0, 0, 0}), eyeDistance(-10), waveBuffor(0), extraBit(0)
    {}

    virtual void onSceneLoadToMemory()
    {
        if (!font.loadFromFile("files/Carnevalee_Freakshow.ttf"))
        {
            std::cout << "cannot load font\n";
        }
        object cube;
        std::vector <wall> wallie;
        int scale = 100;
        for(int i=0; i<30; i++)
            for(int j=0; j<30; j++)
                terrain.push_back({i*scale, 0, j*scale});
        for(int i=0; i<29*29; i++)
        {
            if(i%30 != 29)
            {
            wall walion;
            walion.push_back(terrain[i]);
            walion.push_back(terrain[i+1]);
            walion.push_back(terrain[i+31]);
            walion.push_back(terrain[i+30]);
            wallie.push_back(walion);
            }
        }


        for(int i=0; i<wallie.size(); i++)
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

        // EXPERIMENTAL
        /*   if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
           {
               cameraPos.z+= cos(toRad(cameraAngle.y));
               cameraPos.y+= sin(toRad(cameraAngle.x));
               cameraPos.x-= sin(toRad(cameraAngle.y));
           }
           if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
           {
               cameraPos.z-= cos(toRad(cameraAngle.y));
               cameraPos.y-= sin(toRad(cameraAngle.x));
               cameraPos.x+= sin(toRad(cameraAngle.y));
           }
           if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
           {
               cameraPos.x += cos(toRad(cameraAngle.z));
               cameraPos.y -= sin(toRad(cameraAngle.z));
               cameraPos.z += sin(toRad(cameraAngle.y));
           }

        */
        // END EXPERIMENTAL
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
            for(int i=0; i<8; i++)
                terrain[i].z--;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::K))
        {
            for(int i=0; i<8; i++)
                terrain[i].x++;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
            eyeDistance+=0.1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
            eyeDistance-=0.1;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
        {
            extraBit += 0.5;
            waveBuffor += extraBit;
            for(int j=0; j<30; j++)
            {
            for(int i=0; i<30; i++)
            {
                terrain[i + j*30].y = sinf(waveBuffor/2)*(30-j) + cosf(float(j)/3)*30;
            }
            waveBuffor++;
            }
            waveBuffor = 0;
        }

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
    float eyeDistance, waveBuffor, extraBit;
};
Camera::Camera(SYNTH3D* parent):
    p(parent),
    position({0, 0, -400}),
    eye(-80),
    angle({0,0,0}),
    screenSize(3),
    scale(100)
{
    initCircleDef(16);
}

void Camera::initCircleDef(int n)
{
    sf::Vector2f target = {0, 1}, temp = target;
    for(int i=0; i<n; i++)
    {
        circleDef.push_back(target);
        target.x = temp.x*cosf(2*M_PI/n) - temp.y*sinf(2*M_PI/n);
        target.y = temp.x*sinf(2*M_PI/n) + temp.y*cosf(2*M_PI/n);
        temp = target;
    }
}

sf::Vector3f Camera::planeCross(sf::Vector3f vec1, sf::Vector3f vec2)
{
    return vec2 + (vec1-vec2)*((-vec2.z)/(vec1.z - vec2.z));
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
    return -eye*dot/(vec.z - eye);
}

void Camera::drawLine(sf::Vector2f vec1, sf::Vector2f vec2, float size1, float size2, sf::Color color)
{
    sf::Vector2f norm;
    float temp, lenght;
    norm = (vec1 - vec2);
    lenght = sqrt(norm.x*norm.x + norm.y*norm.y);
    norm.x /= lenght, norm.y /= lenght;
    temp = norm.x, norm.x = -norm.y, norm.y = temp;
    triangleArray.push_back(sf::Vertex(vec1 + norm*size1, color));
    triangleArray.push_back(sf::Vertex(vec2 + norm*size2, color));
    triangleArray.push_back(sf::Vertex(vec1 - norm*size1, color));
    triangleArray.push_back(sf::Vertex(vec2 + norm*size2, color));
    triangleArray.push_back(sf::Vertex(vec1 - norm*size1, color));
    triangleArray.push_back(sf::Vertex(vec2 - norm*size2, color));
}

void Camera::drawDot(sf::Vector2f spot, float size, sf::Color color)
{
    for(int i=0; i<circleDef.size(); i++)
    {
        triangleArray.push_back(sf::Vertex(spot, color));
        triangleArray.push_back(sf::Vertex(spot + circleDef[i]*size, color));
        triangleArray.push_back(sf::Vertex(spot + circleDef[(i+1)%circleDef.size()]*size, color));
    }
}

void Camera::drawWall(wall const& wallie)
{
    /*auto drawDot = [](sf::Vector2f spot, float size, sf::Color color) -> void
    {};*/
    sf::Vector3f tempPlane, vEye = {0, 0, eye};
    std::vector <sf::Vector2f> spot;
    std::vector <sf::Vector3f> spot3d;
    std::vector <float> dot;
    std::vector <int> usefulSpot;
    int indicator;
    for(int i=0; i<wallie.size(); i++)
    {
        spot3d.push_back(vecTransform(*wallie.coord[i] - position));
        if(spot3d[i].z >= 0)
            usefulSpot.push_back(i);
    }
    if(!usefulSpot.empty())
    {
        if(usefulSpot.size() < spot3d.size())
        {
            usefulSpot.clear();
        for(int i=0, j=0; i<2*spot3d.size(); i++)
            if(spot3d[i%spot3d.size()].z < 0 or j > 0)
            {
                if(j == 0)
                    j = 1;
                if(spot3d[i%spot3d.size()].z >= 0 and j != 3)
                    usefulSpot.push_back(i%spot3d.size()), j = 2;
                else if(j == 2)
                    j = 3;
            }
        }
        if(usefulSpot.size() == spot3d.size())
        {
            for(int i=0; i<usefulSpot.size(); i++)
            {
                tempPlane = planeCross(spot3d[i], vEye);
                spot.push_back({tempPlane.x*scale + p->window->getSize().x/2, tempPlane.y*scale + p->window->getSize().y/2});
                dot.push_back(dotSize(wallie.pSize, spot3d[i]));
                indicator = 0;
            }
        }
        else
        {
            tempPlane = planeCross(spot3d[usefulSpot[0]], spot3d[(usefulSpot[0] - 1)%spot3d.size()]);
            spot.push_back({tempPlane.x*scale + p->window->getSize().x/2, tempPlane.y*scale + p->window->getSize().y/2});
            dot.push_back(wallie.pSize);
            for(int i=0; i<usefulSpot.size(); i++)
            {
                tempPlane = planeCross(spot3d[ usefulSpot[i] ], vEye);
                spot.push_back({tempPlane.x*scale + p->window->getSize().x/2, tempPlane.y*scale + p->window->getSize().y/2});
                dot.push_back(dotSize(wallie.pSize, spot3d[usefulSpot[i]]));
            }
            tempPlane = planeCross(spot3d[ usefulSpot[ usefulSpot.size() - 1 ] ], spot3d[ (usefulSpot[ usefulSpot.size() - 1 ] + 1)%spot3d.size() ]);
            spot.push_back({tempPlane.x*scale + p->window->getSize().x/2, tempPlane.y*scale + p->window->getSize().y/2});
            dot.push_back(wallie.pSize);
            indicator = 1;
        }
        for(int i=1; i<spot.size()-1; i++)
        {
            triangleArray.push_back(sf::Vertex(spot[0], wallie.trans));
            triangleArray.push_back(sf::Vertex(spot[i], wallie.trans));
            triangleArray.push_back(sf::Vertex(spot[i+1], wallie.trans));
        }
        for(int i=0; i<spot.size() - indicator; i++)
        {
        drawLine(spot[i],
                 spot[(i+1)%spot.size()],
                 dot[i],
                 dot[(i+1)%spot.size()],
                 wallie.color);
        drawDot(spot[i], dot[i], wallie.color);
        }
    }
}

void Camera::display()
{
    auto squarePointDist = [](sf::Vector3f *point, sf::Vector3f *pos) -> float
    {
        sf::Vector3f temp = *point - *pos;
        return temp.x*temp.x + temp.y*temp.y + temp.z*temp.z;
    };
    auto squareWallDist = [](wall& jackson, sf::Vector3f& pos) -> float
    {
        float summary = 0;
        for(int i=0; i<jackson.size(); i++)
        {
            sf::Vector3f temp = *jackson.coord[i] - pos;
            summary += temp.x*temp.x + temp.y*temp.y + temp.z*temp.z;
        }
        return summary/float(jackson.size());
    };

    p->window->clear();
    //std::cout << "X: " << position.x << " Y: " << position.y << " Z: " << position.z << " Yaw: " << angle.y*180/M_PI << " Pitch: " << angle.x*180/M_PI << "\n" << " Eye: " << eye;
    struct tempType
    {
        int i;
        int j;
        float dist;
    };
    std::vector <tempType> wallOrder;

    for(int i=0; i<p->world.size(); i++)
        for(int j=0; j<p->world[i].size(); j++)
            wallOrder.push_back({i, j, squareWallDist(p->world[i].wallie[j], position)});
    std::sort(wallOrder.begin(), wallOrder.end(), [](const tempType &left, const tempType &right) {return left.dist > right.dist;});
    triangleArray.clear();
    for(int i=0; i<wallOrder.size(); i++)
        drawWall(p->world[wallOrder[i].i].wallie[wallOrder[i].j]);
    p->window->draw(&triangleArray[0], triangleArray.size(), sf::Triangles);
}

#endif //SYNTH3D_HPP
