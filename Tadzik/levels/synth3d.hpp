///Synth3d ver. 2.0 alpha
#ifndef SYNTH3D_HPP
#define SYNTH3D_HPP

#pragma once

#include "../include/Scene.hpp"
#include "../include/Utils.hpp"
#include <vector>
#include <cmath>
using namespace Utils;

template<typename realType>
inline realType toRad(realType angle)
{
    return angle * (realType)0.017453292519943295769;
}

struct wall
{
    std::vector <int> coord;
    std::vector <bool> drawable;
    std::vector <bool> dotDraw;
    sf::Color color=sf::Color::Green;
    sf::Color trans=sf::Color::Transparent;
    float pSize = 100;
    int grid = 0;
    bool OptAllowed = true;
    void push_back(int c, bool draw = true, bool dot = true)
    {
        coord.push_back(c);
        drawable.push_back(draw);
        dotDraw.push_back(dot);
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
    bool OptAllowed = true;
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
    void update();
protected:
    std::vector <sf::Vector2f> halfDotBegin;
    std::vector <sf::Vector2f> circleDefExample;
    std::vector <sf::Vertex> lineArray;
    std::vector <sf::Vertex> triangleArray;
    std::vector <sf::Vertex> quadArray;
    std::vector <sf::Vertex> stripArray;
    std::vector <sf::Vector3f> spot3d;
    std::vector <sf::Vector3f> spot2d;
    std::vector <float> spot3dSquareDist;
    void initCircleDef(int n);
    void calcTerrain();
    void updateTerrain();
    SYNTH3D* p;
    float screenSize, viewAngle, eye, scale, minDotSize;
    float sinx, siny, sinz, cosx, cosy, cosz, sinCircle, cosCircle, circleDef;
    sf::Vector3f position, angle;
    sf::Vector2f whatever;
    sf::Vector3f planeCross(sf::Vector3f vec1, sf::Vector3f vec2);
    sf::Vector2f flatView(sf::Vector3f vec);
    sf::Vector3f vecTransform(sf::Vector3f vec);
    sf::Vector3f vecTransformAngle(sf::Vector3f vec, sf::Vector3f ang);
    sf::Vector3f vecTransformSin(sf::Vector3f vec, float& sinX, float& sinY, float& sinZ, float& cosX, float& cosY, float& cosZ);
    void calcAngle();
    void drawLine(sf::Vector2f vec1, sf::Vector2f vec2, float size1, float size2, sf::Color color, sf::Vector2f& beginning);
    void drawDot(sf::Vector2f spot, float size, sf::Color color);
    void drawHalfDot(sf::Vector2f& spot, sf::Vector2f beginning, float& size, sf::Color color);
    void drawDotwBegin(sf::Vector2f& spot, sf::Vector2f beginning, float& size, sf::Color color);
    void drawSphere(sf::Vector3f spot, float radius, sf::Color color, float outlineThickness, sf::Color outlineColor);
    void drawWall(wall const& wallie);
    void drawPlane(std::vector <sf::Vector2f>& spot, sf::Color color);
    void drawGridWall(wall const& wallie, int n);
    float dotSize(float dot, sf::Vector3f vec);
    float squareWallDist(wall& wallie);
};

class SYNTH3D: public Scene
{
public:
    friend class Camera;
    SYNTH3D(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
        :Scene(_name,mgr,w), c(this), cameraPos({0, 0, -50}), cameraAngle({0, 0, 0}),
        eyeDistance(-10), terrainSize(5000)
    {}

    virtual void onSceneLoadToMemory()
    {
        object cube;
        std::vector <wall> wallie;

        int scale = 100;
        /*
        for(int i=0; i<terrainSize; i++)
            for(int j=0; j<terrainSize; j++)
                terrain.push_back({i*scale, 0, j*scale});
        for(int i=0; i<(terrainSize-1)*(terrainSize-1); i++)
        {
            if(i%terrainSize != terrainSize-1)
            {
            wall walion;
            walion.push_back(i);
            walion.push_back(i+1);
            walion.push_back(i+terrainSize+1);
            walion.push_back(i+terrainSize);
            wallie.push_back(walion);
            }
        }
        */
        terrain.push_back({0,0,0});
        terrain.push_back({terrainSize*scale,0,0});
        terrain.push_back({terrainSize*scale,0,terrainSize*scale});
        terrain.push_back({0,0,terrainSize*scale});

        wall walion;
        walion.push_back(0);
        walion.push_back(1);
        walion.push_back(2);
        walion.push_back(3);
        walion.grid = terrainSize;
        wallie.push_back(walion);

        for(int i=0; i<wallie.size(); i++)
            cube.push_back(wallie[i]);
        world.push_back(cube);
        OptLines();
        OptDots();
        c.update();
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
    std::vector <sf::Vector3f> terrain;
    std::vector <object> world;
    Camera c;
    sf::Vector3f cameraPos;
    sf::Vector3f cameraAngle;
    float eyeDistance;
    int terrainSize;
    void OptLines();
    void OptDots();
};

void SYNTH3D::OptLines()
{
    int index;
    struct tempType
    {
        int first;
        int second;
        float pSize;
        int i;
        int j;
        int k;
    };
    std::vector <tempType> lineBuffor;
    auto insideVector = [](std::pair <int, int> element, std::vector <tempType>& vec, int& k) -> bool
    {
        for(int i=0; i<vec.size(); i++)
            if((element.first == vec[i].first and element.second == vec[i].second) or (element.first == vec[i].second and element.second == vec[i].first))
            {
                k = i;
                return true;
            }
        return false;
    };
    for(int i=0; i < world.size(); i++)
        for(int j=0; j < world[i].size(); j++)
            for(int k=0; k < world[i].wallie[j].size(); k++)
                if(insideVector({world[i].wallie[j].coord[k],
                                 world[i].wallie[j].coord[(k+1)%world[i].wallie[j].size()]},
                                 lineBuffor, index)
                                 and world[i].OptAllowed and world[i].wallie[j].OptAllowed)
                    if(world[i].wallie[j].pSize <= lineBuffor[index].pSize)
                        world[i].wallie[j].drawable[k] = false;
                    else
                        world[lineBuffor[index].i].wallie
                             [lineBuffor[index].j].drawable
                             [lineBuffor[index].k] = false;
                else
                {
                    lineBuffor.push_back({world[i].wallie[j].coord[k],
                                                  world[i].wallie[j].coord[(k+1)%world[i].wallie[j].size()],
                                                  world[i].wallie[j].pSize, i, j, k});
                    if(world[i].OptAllowed and world[i].wallie[j].OptAllowed)
                        world[i].wallie[j].drawable[k] = true;
                }
}

void SYNTH3D::OptDots()
{
    int index;
    struct tempType
    {
        int first;
        float pSize;
        int i;
        int j;
        int k;
    };
    std::vector <tempType> dotBuffor;
    auto insideVector = [](int element, std::vector <tempType>& vec, int& k) -> bool
    {
        for(int i=0; i<vec.size(); i++)
            if(element == vec[i].first)
            {
                k = i;
                return true;
            }
        return false;
    };
    for(int i=0; i < world.size(); i++)
        for(int j=0; j < world[i].size(); j++)
            for(int k=0; k < world[i].wallie[j].size(); k++)
                if(insideVector(world[i].wallie[j].coord[k],
                                 dotBuffor, index)
                                 and world[i].OptAllowed and world[i].wallie[j].OptAllowed)
                    if(world[i].wallie[j].pSize <= dotBuffor[index].pSize)
                        world[i].wallie[j].dotDraw[k] = false;
                    else
                        world[dotBuffor[index].i].wallie
                             [dotBuffor[index].j].dotDraw
                             [dotBuffor[index].k] = false;
                else
                {
                    dotBuffor.push_back({world[i].wallie[j].coord[k], world[i].wallie[j].pSize, i, j, k});
                    if(world[i].OptAllowed and world[i].wallie[j].OptAllowed)
                        world[i].wallie[j].dotDraw[k] = true;
                }
}

Camera::Camera(SYNTH3D* parent):
    p(parent),
    position({0, 0, -400}),
    eye(-80),
    angle({0,0,0}),
    screenSize(3),
    scale(100),
    minDotSize(2)
{
    initCircleDef(32);
}

void Camera::updateTerrain()
{
    spot3d.resize(p->terrain.size());
    spot3dSquareDist.resize(spot3d.size());
    spot2d.resize(spot3d.size());
    halfDotBegin.resize(spot3d.size());
}

void Camera::update()
{
    updateTerrain();
}

void Camera::calcAngle()
{
    sinx = sinf(angle.x), cosx = cosf(angle.x);
    siny = sinf(angle.y), cosy = cosf(angle.y);
    sinz = sinf(angle.z), cosz = cosf(angle.z);
}

void Camera::initCircleDef(int n)
{
    if(n%2 != 0)
        n++;
    sf::Vector2f target = {0, 1}, temp = target;
    sinCircle = sinf(2*M_PI/n);
    cosCircle = cosf(2*M_PI/n);
    circleDef = n;
    for(int i=0; i<n; i++)
    {
        circleDefExample.push_back(target);
        target.x = temp.x*cosCircle - temp.y*sinCircle;
        target.y = temp.x*sinCircle + temp.y*cosCircle;
        temp = target;
    }
}

sf::Vector3f Camera::planeCross(sf::Vector3f vec1, sf::Vector3f vec2)
{
    return vec2 + (vec1-vec2)*((-vec2.z)/(vec1.z - vec2.z));
}

sf::Vector2f Camera::flatView(sf::Vector3f vec)
{
    return {vec.x*scale + p->window->getSize().x/2, vec.y*scale + p->window->getSize().y/2};
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

sf::Vector3f Camera::vecTransformAngle(sf::Vector3f vec, sf::Vector3f ang)
{
    sf::Vector3f target = vec;
    //Yaw: (y axis)
    target.x = vec.x*cosf(ang.y) + vec.z*sinf(ang.y);
    target.z = vec.z*cosf(ang.y) - vec.x*sinf(ang.y);
    //Pitch; (x axis)
    vec = target;
    target.y = vec.y*cosf(ang.x) - vec.z*sinf(ang.x);
    target.z = vec.y*sinf(ang.x) + vec.z*cosf(ang.x);
    //Roll: (z axis)
    vec = target;
    target.x = vec.x*cosf(ang.z) - vec.y*sinf(ang.z);
    target.y = vec.x*sinf(ang.z) + vec.y*cosf(ang.z);
    return target;
}

sf::Vector3f Camera::vecTransform(sf::Vector3f vec)
{
    sf::Vector3f target = vec;
    //Yaw: (y axis)
    target.x = vec.x*cosy + vec.z*siny;
    target.z = vec.z*cosy - vec.x*siny;
    //Pitch; (x axis)
    vec = target;
    target.y = vec.y*cosx - vec.z*sinx;
    target.z = vec.y*sinx + vec.z*cosx;
    //Roll: (z axis)
    vec = target;
    target.x = vec.x*cosz - vec.y*sinz;
    target.y = vec.x*sinz + vec.y*cosz;
    return target;
}

sf::Vector3f Camera::vecTransformSin(sf::Vector3f vec, float& sinX, float& sinY, float& sinZ,
                                                    float& cosX, float& cosY, float& cosZ)
{
    sf::Vector3f target = vec;
    //Yaw: (y axis)
    target.x = vec.x*cosY + vec.z*sinY;
    target.z = vec.z*cosY - vec.x*sinY;
    //Pitch; (x axis)
    vec = target;
    target.y = vec.y*cosX - vec.z*sinX;
    target.z = vec.y*sinX + vec.z*cosX;
    //Roll: (z axis)
    vec = target;
    target.x = vec.x*cosZ - vec.y*sinZ;
    target.y = vec.x*sinZ + vec.y*cosZ;
    return target;
}

void Camera::setEyeDistance(float distance)
{
    eye = distance;
}

void Camera::drawSphere(sf::Vector3f spot, float radius, sf::Color color, float outlineThickness = 0, sf::Color outlineColor = sf::Color::Transparent)
{
    int n = 64;
    sf::Vector3f buffor;
    sf::Vector3f vEye = {0, 0, eye};
    spot -= vEye;
    float lenght = sqrt(spot.x*spot.x + spot.y*spot.y + spot.z*spot.z);
    float lenghtZ = sqrt(spot.x*spot.x + spot.y*spot.y);
    float sinZ = spot.y / lenghtZ;
    float cosZ = spot.x / lenghtZ;
    sf::Vector3f temp = spot;
    temp.x =  spot.x*cosZ + spot.y*sinZ;
    temp.y = -spot.x*sinZ + spot.y*cosZ;
    sf::Vector3f line = temp;
    float lenghtY = sqrt(temp.x*temp.x + temp.z*temp.z);
    float sinY = temp.z / lenghtY;
    float cosY = temp.x / lenghtY;
    line.x = temp.x*cosY + temp.z*sinY;
    line.z = temp.z*cosY - temp.x*sinY;
    std::cout << "\nX: " << line.x << " Y: " << line.y << " Z: " << line.z;
    std::cout << "\nL: " << lenght;
    temp = sf::Vector3f(0, radius, 0);
    float sin = radius / lenght;
    float cos = sqrt(lenght*lenght - radius*radius) / lenght;
    buffor.x = temp.x*cos - temp.y*sin;
    buffor.y = temp.x*sin + temp.y*cos;
    buffor.z = 0;
    buffor.x += lenght;
    // simplifies the simulation for long distances, makes it wrong at close distance
    //buffor = sf::Vector3f(lenght, radius, 0);
    std::vector <sf::Vector3f> sphere;
    sin = sinf(2*M_PI/n);
    cos = cosf(2*M_PI/n);
    for(int i=0; i<n; i++)
    {
        sphere.push_back(buffor);
        temp = buffor;
        buffor.y = temp.y*cos - temp.z*sin;
        buffor.z = temp.y*sin + temp.z*cos;
    }
    sf::Vector3f vec;
    for(int i=0; i<sphere.size(); i++)
    {
        vec = sphere[i];
        vec.x = sphere[i].x*cosY - sphere[i].z*sinY;
        vec.z = sphere[i].z*cosY + sphere[i].x*sinY;
        temp = vec;
        vec.x = temp.x*cosZ - temp.y*sinZ;
        vec.y = temp.x*sinZ + temp.y*cosZ;
        sphere[i] = vec + vEye;
    }
    std::vector <sf::Vector2f> screenBuffor;
    for(int i=0; i<sphere.size(); i++)
        if(sphere[i].z >= 0)
            screenBuffor.push_back(flatView(planeCross(sphere[i], vEye)));
    if(!screenBuffor.empty())
    {
        stripArray.push_back(sf::Vertex(screenBuffor[screenBuffor.size()/2]));
        stripArray.push_back(sf::Vertex(screenBuffor[screenBuffor.size()/2], color));
        for(int i = 1; i<screenBuffor.size()/2; i++)
        {
            stripArray.push_back(sf::Vertex(screenBuffor[screenBuffor.size()/2 + i], color));
            stripArray.push_back(sf::Vertex(screenBuffor[screenBuffor.size()/2 - i], color));
        }
        stripArray.push_back(sf::Vertex(screenBuffor[0], color));
        stripArray.push_back(sf::Vertex(screenBuffor[0]));
        sf::Vector2f whatever;
        if(outlineThickness != 0)
            for(int i=0; i<screenBuffor.size(); i++)
                drawLine(screenBuffor[i],
                         screenBuffor[(i+1)%screenBuffor.size()],
                         dotSize(outlineThickness, sphere[i]),
                         dotSize(outlineThickness, sphere[(i+1)%sphere.size()]),
                         outlineColor,
                         whatever);
    }
}

float Camera::dotSize(float dot, sf::Vector3f vec)
{
    return -eye*dot/(vec.z - eye);
}

void Camera::drawLine(sf::Vector2f vec1, sf::Vector2f vec2, float size1, float size2, sf::Color color, sf::Vector2f& beginning)
{
        sf::Vector2f norm;
        float temp, lenght;
        norm = (vec1 - vec2);
        lenght = sqrt(norm.x*norm.x + norm.y*norm.y);
        norm.x /= lenght, norm.y /= lenght;
        temp = norm.x, norm.x = -norm.y, norm.y = temp;
        beginning = norm*size1;
        quadArray.push_back(sf::Vertex(vec1 + norm*size1, color));
        quadArray.push_back(sf::Vertex(vec2 + norm*size2, color));
        quadArray.push_back(sf::Vertex(vec2 - norm*size2, color));
        quadArray.push_back(sf::Vertex(vec1 - norm*size1, color));
}

void Camera::drawHalfDot(sf::Vector2f& spot, sf::Vector2f beginning, float& size, sf::Color color)
{
    if(spot.x > -size and spot.x < p->window->getSize().x + size and spot.y > -size and spot.y < p->window->getSize().y + size)
    {
        std::vector <sf::Vector2f> halfCircle;
        sf::Vector2f temp = beginning;
        for(int i=0; i<circleDef/2 +1; i++)
    {
        halfCircle.push_back(beginning);
        beginning.x = temp.x*cosCircle + temp.y*sinCircle;
        beginning.y = -temp.x*sinCircle + temp.y*cosCircle;
        temp = beginning;
    }
        stripArray.push_back(sf::Vertex(spot + halfCircle[halfCircle.size()/2]));
        stripArray.push_back(sf::Vertex(spot + halfCircle[halfCircle.size()/2], color));
        for(int i = 1; i<halfCircle.size()/2; i++)
        {
            stripArray.push_back(sf::Vertex(spot + halfCircle[halfCircle.size()/2 + i], color));
            stripArray.push_back(sf::Vertex(spot + halfCircle[halfCircle.size()/2 - i], color));
        }
        stripArray.push_back(sf::Vertex(spot + halfCircle[halfCircle.size() - 1], color));
        stripArray.push_back(sf::Vertex(spot + halfCircle[0], color));
        stripArray.push_back(sf::Vertex(spot + halfCircle[0]));
    }
}

void Camera::drawDotwBegin(sf::Vector2f& spot, sf::Vector2f beginning, float& size, sf::Color color)
{
    if(spot.x > -size and spot.x < p->window->getSize().x + size and spot.y > -size and spot.y < p->window->getSize().y + size)
    {
        std::vector <sf::Vector2f> fullCircle;
        sf::Vector2f temp = beginning;
        for(int i=0; i<circleDef; i++)
    {
        fullCircle.push_back(beginning);
        beginning.x = temp.x*cosCircle + temp.y*sinCircle;
        beginning.y = -temp.x*sinCircle + temp.y*cosCircle;
        temp = beginning;
    }
        stripArray.push_back(sf::Vertex(spot + fullCircle[fullCircle.size()/2]));
        stripArray.push_back(sf::Vertex(spot + fullCircle[fullCircle.size()/2], color));
        for(int i = 1; i<fullCircle.size()/2; i++)
        {
            stripArray.push_back(sf::Vertex(spot + fullCircle[fullCircle.size()/2 + i], color));
            stripArray.push_back(sf::Vertex(spot + fullCircle[fullCircle.size()/2 - i], color));
        }
        stripArray.push_back(sf::Vertex(spot + fullCircle[0], color));
        stripArray.push_back(sf::Vertex(spot + fullCircle[0]));
    }
}

void Camera::drawDot(sf::Vector2f spot, float size, sf::Color color)
{
    if(spot.x > -size and spot.x < p->window->getSize().x + size and spot.y > -size and spot.y < p->window->getSize().y + size)
    {
        stripArray.push_back(sf::Vertex(spot + circleDefExample[circleDefExample.size()/2]*size));
        stripArray.push_back(sf::Vertex(spot + circleDefExample[circleDefExample.size()/2]*size, color));
        for(int i = 1; i<circleDefExample.size()/2; i++)
        {
            stripArray.push_back(sf::Vertex(spot + circleDefExample[circleDefExample.size()/2 + i]*size, color));
            stripArray.push_back(sf::Vertex(spot + circleDefExample[circleDefExample.size()/2 - i]*size, color));
        }
        stripArray.push_back(sf::Vertex(spot + circleDefExample[0]*size, color));
        stripArray.push_back(sf::Vertex(spot + circleDefExample[0]*size));
    }
}

void Camera::calcTerrain()
{
    sf::Vector3f vEye = {0, 0, eye};
    for(int i=0; i<p->terrain.size(); i++)
    {
        spot3d[i] = vecTransform(p->terrain[i] - position);
        spot2d[i] = planeCross(spot3d[i], vEye);
        spot3dSquareDist[i] = spot3d[i].x*spot3d[i].x + spot3d[i].y*spot3d[i].y + spot3d[i].z*spot3d[i].z;
    }
}

void Camera::drawGridWall(wall const& wallie, int n)
{
    ///      1>
    ///  #########
    /// ^#       #^      1 2
    /// 0#       #2      0 3
    ///  #       #
    ///  #########
    ///      3>

    sf::Vector3f line[4];
    sf::Vector3f point[4][n];
    sf::Vector3f vEye = sf::Vector3f(0, 0, eye);
    sf::Vector2f buffor;

    line[0] = spot3d[ wallie.coord[1] ] - spot3d[ wallie.coord[0] ];
    line[1] = spot3d[ wallie.coord[2] ] - spot3d[ wallie.coord[1] ];
    line[2] = spot3d[ wallie.coord[2] ] - spot3d[ wallie.coord[3] ];
    line[3] = spot3d[ wallie.coord[3] ] - spot3d[ wallie.coord[0] ];

    line[0] /= float(n); line[1] /= float(n); line[2] /= float(n); line[3] /= float(n);

    point[0][0] = spot3d[ wallie.coord[0] ];
    point[1][0] = spot3d[ wallie.coord[1] ];
    point[2][0] = spot3d[ wallie.coord[3] ];
    point[3][0] = spot3d[ wallie.coord[0] ];

    for(int i = 1; i < n; i++)
        for(int k = 0; k < 4; k++)
            point[k][i] = point[k][i-1] + line[k];

    for(int i = 1; i < n; i++)
    {
        if(point[0][i].z >= 0 and point[2][i].z >= 0)
            drawLine(flatView(planeCross(point[0][i], vEye)),
                     flatView(planeCross(point[2][i], vEye)),
                     dotSize(wallie.pSize, point[0][i]),
                     dotSize(wallie.pSize, point[2][i]),
                     wallie.color,
                     whatever);
        else if(point[0][i].z < 0 and point[2][i].z < 0)
        {}
        else if(point[0][i].z < 0)
            drawLine(buffor = flatView(planeCross(point[0][i],point[2][i])),
                     flatView(planeCross(point[2][i], vEye)),
                     wallie.pSize,
                     dotSize(wallie.pSize, point[2][i]),
                     wallie.color,
                     whatever),
            drawDot(buffor, wallie.pSize, wallie.color);
        else
            drawLine(flatView(planeCross(point[0][i], vEye)),
                     buffor = flatView(planeCross(point[0][i], point[2][i])),
                     dotSize(wallie.pSize, point[0][i]),
                     wallie.pSize,
                     wallie.color,
                     whatever),
            drawDot(buffor, wallie.pSize, wallie.color);

        if(point[1][i].z >= 0 and point[3][i].z >= 0)
            drawLine(flatView(planeCross(point[1][i], vEye)),
                     flatView(planeCross(point[3][i], vEye)),
                     dotSize(wallie.pSize, point[1][i]),
                     dotSize(wallie.pSize, point[3][i]),
                     wallie.color,
                     whatever);
        else if(point[1][i].z < 0 and point[3][i].z < 0)
        {}
        else if(point[1][i].z < 0)
            drawLine(buffor = flatView(planeCross(point[1][i], point[3][i])),
                     flatView(planeCross(point[3][i], vEye)),
                     wallie.pSize,
                     dotSize(wallie.pSize, point[3][i]),
                     wallie.color,
                     whatever),
            drawDot(buffor, wallie.pSize, wallie.color);
        else
            drawLine(flatView(planeCross(point[1][i], vEye)),
                     buffor = flatView(planeCross(point[1][i], point[3][i])),
                     dotSize(wallie.pSize, point[1][i]),
                     wallie.pSize,
                     wallie.color,
                     whatever),
            drawDot(buffor, wallie.pSize, wallie.color);
    }
}

void Camera::drawPlane(std::vector <sf::Vector2f>& spot, sf::Color color)
{
        if(spot.size() == 4)
            quadArray.push_back(sf::Vertex(spot[0], color)),
            quadArray.push_back(sf::Vertex(spot[1], color)),
            quadArray.push_back(sf::Vertex(spot[2], color)),
            quadArray.push_back(sf::Vertex(spot[3], color));
        else
            for(int i=1; i<spot.size()-1; i++)
                triangleArray.push_back(sf::Vertex(spot[0], color)),
                triangleArray.push_back(sf::Vertex(spot[i], color)),
                triangleArray.push_back(sf::Vertex(spot[i+1], color));
}

void Camera::drawWall(wall const& wallie)
{
    std::vector <sf::Vector2f> spot;
    std::vector <float> dot;
    std::vector <int> usefulSpot;
    for(int i=0; i<wallie.size(); i++)
    {
        if(spot3d[ wallie.coord[i] ].z >= 0)
            usefulSpot.push_back(i);
    }
    if(!usefulSpot.empty())
    {
        if(usefulSpot.size() == wallie.size())
        {
            for(int i=0; i<usefulSpot.size(); i++)
                spot.push_back(flatView(spot2d[wallie.coord[i]])),
                dot.push_back(dotSize(wallie.pSize, spot3d[wallie.coord[i]]));
            if(wallie.trans != sf::Color::Transparent)
                drawPlane(spot, wallie.trans);
            if(wallie.grid > 1 and wallie.size() == 4)
                drawGridWall(wallie, wallie.grid);
            for(int i=0; i<wallie.size(); i++)
                if(wallie.drawable[i])
                    drawLine(spot[i],
                             spot[(i+1)%spot.size()],
                             dot[i],
                             dot[(i+1)%spot.size()],
                             wallie.color,
                             halfDotBegin[wallie.coord[i]]);
            for(int i=0; i<spot.size(); i++)
                if(dot[i] > minDotSize and wallie.dotDraw[i])
                    drawHalfDot(spot[i], halfDotBegin[wallie.coord[(i)%wallie.size()]], dot[i], wallie.color);
                  //drawDotwBegin(spot[i], halfDotBegin[wallie.coord[(i+first*indicator)%wallie.size()]], dot[i], sf::Color::Magenta);
                  //drawDot(spot[i], dot[i], wallie.color);
        }
        else
        {
            int first;
            usefulSpot.clear();
            for(int i=0, j=0; i<2*wallie.size(); i++)
                if(spot3d[ wallie.coord[i%wallie.size()] ].z < 0 or j > 0)
                {
                    if(j == 0)
                        j = 1;
                    if(spot3d[ wallie.coord[i%wallie.size()] ].z >= 0 and j != 3)
                    {
                        usefulSpot.push_back(i%wallie.size());
                        if(j != 2)
                            first = (i-1)%wallie.size();
                        j = 2;
                    }
                    else if(j == 2)
                        j = 3;
                }
            spot.push_back(flatView(planeCross(spot3d[wallie.coord[  usefulSpot[0]    ] ],
                                               spot3d[wallie.coord[ (usefulSpot[0] - 1)%wallie.size() ]])));
            dot.push_back(wallie.pSize);
            for(int i=0; i<usefulSpot.size(); i++)
            {
                spot.push_back(flatView(spot2d[ wallie.coord[usefulSpot[i]] ]));
                dot.push_back(dotSize(wallie.pSize, spot3d[ wallie.coord[usefulSpot[i]] ]));
            }
            spot.push_back(flatView(planeCross(spot3d[ wallie.coord [usefulSpot[ usefulSpot.size() - 1 ]    ] ],
                                               spot3d[ wallie.coord[(usefulSpot[ usefulSpot.size() - 1 ] + 1)%wallie.size()] ])));
            dot.push_back(wallie.pSize);
            if(wallie.trans != sf::Color::Transparent)
                drawPlane(spot, wallie.trans);
            if(wallie.grid > 1 and wallie.size() == 4)
                drawGridWall(wallie, wallie.grid);
            for(int i=0; i<spot.size() - 1; i++)
                if(wallie.drawable[(i+first)%wallie.size()])
                    drawLine(spot[i],
                         spot[(i+1)%spot.size()],
                         dot[i],
                         dot[(i+1)%spot.size()],
                         wallie.color,
                         halfDotBegin[wallie.coord[(i+first)%wallie.size()]]);
            for(int i=0; i<spot.size(); i++)
                if(dot[i] > minDotSize and wallie.dotDraw[(i+first)%wallie.size()])
                  //drawHalfDot(spot[i], halfDotBegin[wallie.coord[(i)%wallie.size()]], dot[i], wallie.color);
                    drawDotwBegin(spot[i], halfDotBegin[wallie.coord[(i+first)%wallie.size()]], dot[i], wallie.color);
                  //drawDot(spot[i], dot[i], wallie.color);
        }
    }
}

float Camera::squareWallDist(wall& wallie)
{
    float summary = 0;
    for(int i=0; i<wallie.size(); i++)
        {
            summary += spot3dSquareDist[wallie.coord[i]];
        }
    return summary/float(wallie.size());
}

void Camera::display()
{
    //std::cout << "X: " << position.x << " Y: " << position.y << " Z: " << position.z << " Yaw: " << angle.y*180/M_PI << " Pitch: " << angle.x*180/M_PI << "\n" << " Eye: " << eye;
    struct tempType
    {
        int i;
        int j;
        float dist;
    };
    std::vector <tempType> wallOrder;
    calcAngle();
    calcTerrain();
    for(int i=0; i<p->world.size(); i++)
        for(int j=0; j<p->world[i].size(); j++)
            wallOrder.push_back({i, j, squareWallDist(p->world[i].wallie[j])});
    std::sort(wallOrder.begin(), wallOrder.end(), [](const tempType &left, const tempType &right) {return left.dist > right.dist;});
    triangleArray.clear(), quadArray.clear(), stripArray.clear();
    for(int i=0; i<wallOrder.size(); i++)
        drawWall(p->world[wallOrder[i].i].wallie[wallOrder[i].j]);
    sf::Vector3f place = sf::Vector3f(100, -150, 300);
    place = vecTransform(place - position);
    drawSphere(place, 50, sf::Color::Magenta);

    p->window->clear();
    p->window->draw(&triangleArray[0], triangleArray.size(), sf::Triangles);
    p->window->draw(&quadArray[0], quadArray.size(), sf::Quads);
    p->window->draw(&stripArray[0], stripArray.size(), sf::TriangleStrip);
    std::cout << "\nTriangle Array size: " << triangleArray.size() << "\nQuad Array size: " << quadArray.size() << "\nStrip Array size: " << stripArray.size();
}

#endif //SYNTH3D_HPP
