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
    void display();                     //Wyswietlanie
    void setPos(sf::Vector3f posGiven); //Ustaw pozycje kamery
    void setAngle(sf::Vector3f aGiven); //Ustaw kat skierowania kamery
    void setEyeDistance(float distance);//Ustaw odleglosc oka od plaszczyzny
    bool eyeTurn;                       //Wlacz badz wylacz emulacje obrotu plaszczyzny wzgledem oka
protected:
    sf::Vertex lina[2];
    SYNTH3D* p;
    sf::Vector3f position;      //Pozycja kamery
    sf::Vector3f center;        //Centrum plaszczyzny
    sf::Vector3f defSide, side; //Bok plaszczyzny
    sf::Vector3f defTop, top;   //Gora plaszczyzny
    sf::Vector3f defEye, eye;   //Oko
    sf::Vector3f vPlaneTop;     //Wektor plaszczyzny gorny
    sf::Vector3f vPlaneSide;    //Wektor plaszczyzny dolny
    sf::Vector3f angle;         //Kat skierowania kamery
    void calcPlane();           //Wylicz wektory plaszczyzny
    sf::Vector3f planeCross(sf::Vector3f* point); //Policz przeciecie punktu z ekranem
    sf::Vector2f process(sf::Vector3f* point);    //Podaj wspolrzedne na ekranie
    void calcPos();                               //Przywroc plaszczyzne do default wartosci
    void calcAngle();                             //Obroc plaszczyzne o kat skierowania
    sf::Vector3f pointTranslation(sf::Vector3f centre, sf::Vector3f target, sf::Vector3f aGiven);       //Obroc punkt wzgledem punktu o kat dany
    sf::Vector3f centerTranslation(sf::Vector3f target, sf::Vector3f aGiven);                           //Obroc punkt wzgledem srodka ukladu wspolrzednych o kat dany
    sf::Vector3f matrixPointTranslation(sf::Vector3f centre, sf::Vector3f target, sf::Vector3f aGiven); //Obroc punkt wzgledem punktu o kat dany za pomoca macierzy
};

class SYNTH3D: public Scene
{
public:
    friend class Camera;
    SYNTH3D(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
        :Scene(_name,mgr,w), c(this), cameraPos({0,0,0}), cameraAngle({0,0,0}), eyeDistance(-3), t(false)
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
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::T) and t==1)
                    c.eyeTurn-=1;
            if (!sf::Keyboard::isKeyPressed(sf::Keyboard::T))
                t=1; else t=0;

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
    bool t;
};
Camera::Camera(SYNTH3D* parent):
    p(parent),
    //         X   Y   Z
       center({0,  0,  0}),
       defEye({0,  0, -1}),
      defSide({1,  0,  0}),
       defTop({0,  1,  0}),
    //--------------------
    eyeTurn(true),
    angle({0, 0, 0})
{}

sf::Vector3f Camera::planeCross(sf::Vector3f* point)
{
    sf::Vector3f temp = *point - position;
    if(eyeTurn == true)
        temp += (eye - defEye); //Emulacja obrotu wzgledem oka
    return eye + temp*det3f(vPlaneTop, vPlaneSide, eye)/det3f(vPlaneTop, vPlaneSide, -temp);
}

void  Camera::calcPlane()
{
    vPlaneTop = top - center; // zawsze od centrum ukladu
    vPlaneSide = side - center; // z zawsze = 0
}

sf::Vector2f Camera::process(sf::Vector3f* point)
{
    float scale = 100;
    sf::Vector3f tempPlane = pointTranslation(center, planeCross(point), -angle);
    return {tempPlane.x*scale + p->window->getSize().x/2, tempPlane.y*scale + p->window->getSize().y/2};
}

void Camera::calcPos()
{
    eye = defEye, side = defSide, top = defTop;
}

void Camera::setPos(sf::Vector3f posGiven)
{
    position = posGiven;
}

void Camera::setAngle(sf::Vector3f aGiven)
{
    // x, y, z to te osie ktore sie przy danym obrocie nie ruszaja, czyli aGiven.x = 30 obraca punkty eye i top wzgledem punktu center i zarowno osi x
    angle.x = aGiven.x*(M_PI/180);
    angle.y = aGiven.y*(M_PI/180);
    angle.z = aGiven.z*(M_PI/180);
}

void Camera::calcAngle()
{
    eye = centerTranslation(eye, angle);
    top = centerTranslation(top, angle);
    side = centerTranslation(side, angle);
}

sf::Vector3f Camera::pointTranslation(sf::Vector3f centre, sf::Vector3f target, sf::Vector3f aGiven = {0,0,0})
{
    sf::Vector3f newTarget = target;
    //x translation
    newTarget.z = (target.z - centre.z)*cosf(aGiven.x) - (target.y - centre.y)*sinf(aGiven.x) + centre.z;
    newTarget.y = (target.z - centre.z)*sinf(aGiven.x) + (target.y - centre.y)*cosf(aGiven.x) + centre.y;
    //y translation
    target = newTarget;
    newTarget.x = (target.x - centre.x)*cosf(aGiven.y) - (target.z - centre.z)*sinf(aGiven.y) + centre.x;
    newTarget.z = (target.x - centre.x)*sinf(aGiven.y) + (target.z - centre.z)*cosf(aGiven.y) + centre.z;
    //z trasnlation
    target = newTarget;
    newTarget.x = (target.x - centre.x)*cosf(aGiven.z) - (target.y - centre.y)*sinf(aGiven.z) + centre.x;
    newTarget.y = (target.x - centre.x)*sinf(aGiven.z) + (target.y - centre.y)*cosf(aGiven.z) + centre.y;
    return newTarget;
}

sf::Vector3f Camera::centerTranslation(sf::Vector3f target, sf::Vector3f aGiven = {0,0,0})
{
    sf::Vector3f newTarget = target;
    //x translation
    newTarget.z = target.z*cosf(aGiven.x) - target.y*sinf(aGiven.x);
    newTarget.y = target.z*sinf(aGiven.x) + target.y*cosf(aGiven.x);
    //y translation
    target = newTarget;
    newTarget.x = target.x*cosf(aGiven.y) - target.z*sinf(aGiven.y);
    newTarget.z = target.x*sinf(aGiven.y) + target.z*cosf(aGiven.y);
    //z trasnlation
    target = newTarget;
    newTarget.x = target.x*cosf(aGiven.z) - target.y*sinf(aGiven.z);
    newTarget.y = target.x*sinf(aGiven.z) + target.y*cosf(aGiven.z);
    return newTarget;
}

sf::Vector3f Camera::matrixPointTranslation(sf::Vector3f centre, sf::Vector3f target, sf::Vector3f aGiven)
{
    //TO DO "https://pl.wikipedia.org/wiki/Macierz_obrotu"
    float matrix3d[3][3];
    // [0][0]   [1][0]    [2][0]
    // [0][1]   [1][1]    [2][1]
    // [0][2]   [1][2]    [2][2]
    //------------------------------------
    //X axle translation
    matrix3d[0][0] = 1, matrix3d[1][0] = 0, matrix3d[2][0] = 0;
    matrix3d[0][1] = 0, matrix3d[1][1] = cosf(aGiven.x), matrix3d[2][1] = -sinf(aGiven.x);
    matrix3d[0][2] = 0, matrix3d[1][2] = sinf(aGiven.x), matrix3d[2][2] = cos(aGiven.x);
    // ^ Matrix declaration

    return target;
}

void Camera::setEyeDistance(float distance)
{
    defEye = {0, 0, distance};
}

void Camera::display()
{
    p->window->clear();
    calcPos();
    calcAngle();
    calcPlane();
    std::cout << "X: " << position.x << " Y: " << position.y << " Z: " << position.z << " eyeTurn: " << eyeTurn << "\n";
    for(int i=0; i<p->world.size();i++)
    {
        for(int j=0; j< p->world[i].size(); j++)
            {
                int mod = p->world[i].wallie[j].size();
                for(int k=0; k< p->world[i].wallie[j].size(); k++)
                {
                    lina[0] = sf::Vertex(process(p->world[i].wallie[j].coord[k]), p->world[i].wallie[j].color);
                    lina[1] = sf::Vertex(process(p->world[i].wallie[j].coord[(k+1)%mod]), p->world[i].wallie[j].color);
                    p->window->draw(lina, 2, sf::Lines);
                }
            }
    }
}

#endif //SYNTH3D_HPP
