///Synth3d ver. 2.6 alpha
#ifndef SYNTH3D_HPP
#define SYNTH3D_HPP

#pragma once

#include "../include/Scene.hpp"
#include "../include/Utils.hpp"
#include <vector>
#include <cmath>
#include <fstream>
#include <queue>
#include <stack>
#include <stdlib.h>
using namespace Utils;

template<typename realType>
inline realType toRad(realType angle)
{
    return angle * (realType)0.017453292519943295769;
}

class Wall
{
public:
    Wall();
    std::vector <int> coord;
    std::vector <bool> drawable;
    std::vector <bool> dotDraw;
    std::vector <std::vector <int> > lineStrip;
    sf::Color color=sf::Color::Green;
    sf::Color trans=sf::Color::Transparent;
    float pSize = 100;
    int grid = 0;
    bool OptAllowed = true;
    void push_back(int c, bool draw, bool dot);
    unsigned int size() const;
    void clear();
};

Wall::Wall():
    pSize(100),
    grid(0),
    OptAllowed(true)
{
}

void Wall::clear()
{
    coord.clear();
    drawable.clear();
    dotDraw.clear();
    lineStrip.clear();
    color=sf::Color::Green;
    trans=sf::Color::Transparent;
    pSize = 100;
    grid = 0;
    OptAllowed = true;
}

void Wall::push_back(int c, bool draw = true, bool dot = true)
{
    coord.push_back(c);
    drawable.push_back(draw);
    dotDraw.push_back(dot);
}

unsigned int Wall::size() const
{
    return coord.size();
}

class Object
{
public:
    Object();
    std::string name;
    std::vector <Wall> wallie;
    unsigned int size();
    void push_back(Wall w);
    void clear();
    bool OptAllowed;
    sf::Vector3f velocity;
    sf::Vector3f position;
    void update(double dt);
protected:

};

Object::Object():
    OptAllowed(true),
    velocity({0, 0, 0})
{
}

void Object::clear()
{
    wallie.clear();
    OptAllowed = true;
    name.clear();
    velocity = {0, 0, 0};
}

void Object::update(double dt)
{

}

void Object::push_back(Wall w)
{
    wallie.push_back(w);
}

unsigned int Object::size()
{
    return wallie.size();
}

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
    void printGraph();
    void enableCycleReduction(bool value);
    void enableWallIntersect(bool value);
    void enableRandomSort(bool value);
    void enableIdentifyWalls(bool value);
    void compare(int left, int right);
protected:
    std::vector <sf::Vector2f> halfDotBegin;
    std::vector <sf::Vector2f> circleDefExample;
    std::vector <Wall*> wallOrder;
    std::vector <sf::Vertex> quadArray;
    std::vector <sf::Vertex> debugArray;
    std::vector <sf::Text> textArray;
    std::vector <sf::Vector3f> spot3d;
    std::vector <sf::Vector3f> spot2d;
    std::vector <float> dot;
    std::vector <sf::Vector2f> wallToPoly(Wall* wallie);
    void initCircleDef(int n);
    void calcTerrain();
    void updateTerrain();
    SYNTH3D* p;
    float screenSize, viewAngle, scale, minDotSize;
    float sinx, siny, sinz, cosx, cosy, cosz, sinCircle, cosCircle, circleDef;
    sf::Vector3f position, angle, vEye;
    float &eye = vEye.z;
    sf::Vector2f whatever;
    sf::Vector3f planeCross(sf::Vector3f vec1, sf::Vector3f vec2);
    sf::Vector2f flatView(sf::Vector3f vec);
    sf::Vector3f vecTransform(sf::Vector3f vec);
    sf::Vector3f vecTransformAngle(sf::Vector3f vec, sf::Vector3f ang);
    sf::Vector3f vecTransformSin(sf::Vector3f vec, float& sinX, float& sinY, float& sinZ, float& cosX, float& cosY, float& cosZ);
    void calcAngle();
    void drawLine(sf::Vector2f vec1, sf::Vector2f vec2, float size1, float size2, sf::Color color, sf::Vector2f& beginning);
    void debugDrawLine(sf::Vector2f vec1, sf::Vector2f vec2, float size1, float size2, sf::Color color, sf::Vector2f& beginning);
    void drawDot(sf::Vector2f spot, float size, sf::Color color);
    void drawHalfDot(sf::Vector2f& spot, sf::Vector2f beginning, float& size, sf::Color color);
    void drawDotwBegin(sf::Vector2f& spot, sf::Vector2f beginning, float& size, sf::Color color);
    void drawSphere(sf::Vector3f spot, float radius, sf::Color color, float outlineThickness, sf::Color outlineColor);
    void drawWall(Wall const& wallie);
    void drawPlane(std::vector <sf::Vector2f>& spot, sf::Color color);
    void debugDrawPlane(std::vector <sf::Vector2f>& spot, sf::Color color);
    void drawGridWall(Wall const& wallie, int n);
    void wallSort();
    void cycleReduction(std::vector <std::vector <int> >& graph, std::vector <int>& graphLevel);
    void createGraph(std::vector <std::vector <int> >& graph, std::vector <int>& graphLevel, std::vector <Wall*> tempOrder);
    void topologicalSort(std::vector <std::vector <int> >& graph, std::vector <int>& graphLevel, std::vector <Wall*> tempOrder);
    void randSort(std::vector <Wall*> tempOrder);
    void displayGraph(std::vector <std::vector <int> >& graph, std::vector <int>& graphLevel);
    bool wallSortingAlgorythm(Wall* lhs, Wall* rhs);
    bool lineIntersect(sf::Vector2f u0, sf::Vector2f v0, sf::Vector2f u1, sf::Vector2f v1);
    bool polygonIntersect(std::vector <sf::Vector2f> poly1, std::vector <sf::Vector2f> poly2);
    bool polygonIntersectOld(std::vector <sf::Vector2f> poly1, std::vector <sf::Vector2f> poly2);
    bool polygonIntersectOlder(std::vector <sf::Vector2f> poly1, std::vector <sf::Vector2f> poly2);
    bool wallIntersect(Wall* wallie1, Wall* wallie2);
    bool wallIntersectOS(Wall* left, Wall* right);
    bool rightSide(sf::Vector2f line1, sf::Vector2f line2, sf::Vector2f point);
    int planeSide(sf::Vector3f& center, sf::Vector3f& top, sf::Vector3f& side, sf::Vector3f& point);
    float dotSize(sf::Vector3f vec);
    void identifyWalls(std::vector <Wall*> wallie);
    std::string cmdOutput;
    bool cycleReductionEnabled;
    bool wallIntersectEnabled;
    bool randomSortEnabled;
    bool identifyWallsEnabled;
};

class SYNTH3D: public Scene
{
public:
    virtual bool onConsoleUpdate(std::vector<std::string> args);
    friend class Camera;
    SYNTH3D(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
        :Scene(_name,mgr,w), c(this), cameraPos({0, 0, -50}), cameraAngle({0, 0, 0}),
        eyeDistance(-10), terrainSize(50), debug(0), offsetx(0), offsety(0), tick(1)
    {}

    virtual void onSceneLoadToMemory()
    {
        if (!font.loadFromFile("files/fonts/Carnevalee_Freakshow.ttf"))
        {
            std::cout << "cannot load font\n";
        }
        consoleCommands.push_back("print graph");
        consoleCommands.push_back("print info");
        consoleCommands.push_back("enable");
        consoleCommands.push_back("enable cycleReduction");
        consoleCommands.push_back("enable wallIntersect");
        consoleCommands.push_back("enable randomSort");
        consoleCommands.push_back("enable identifyWalls");
        consoleCommands.push_back("compare");
        //loadMap("cubeOnSurface");

        //loadMap("oblivion_old");
        //cameraPos = {210, 144, 1092};
        //cameraAngle = {-6, -137, 0};

        loadMap("catOnSurface");
        cameraPos = {26, 34, 32};

        //cameraPos = {26, 116, 30};
        //cameraAngle = {-34, 8, 0};

        OptLines();
        OptDots();
        c.update();
    }
    virtual void onSceneActivate() {}
    virtual void draw(double dt)
    {
        std::cout << "\n" << dt;
        float movementSpeed = 2.0f;
        float cameraRotationSpeed = 1.0f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
        {
            movementSpeed = 7.0f * (1 / 60) / dt;
            cameraRotationSpeed = 2.0f * (1 / 60) / dt;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            cameraPos.x-= movementSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            cameraPos.x+= movementSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            cameraPos.z+= movementSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            cameraPos.z-= movementSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
            cameraPos.y-= movementSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
            cameraPos.y+= movementSpeed;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            cameraAngle.y+= cameraRotationSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            cameraAngle.y-= cameraRotationSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            cameraAngle.x+= cameraRotationSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            cameraAngle.x-= cameraRotationSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
            cameraAngle.z-= cameraRotationSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
            cameraAngle.z+= cameraRotationSpeed;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
            eyeDistance+=0.1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
            eyeDistance-=0.1;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::J))
            for(int i=4; i<terrain.size(); i++)
                terrain[i].x-=3;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
            for(int i=4; i<terrain.size(); i++)
                terrain[i].x+=3;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))
            for(int i=4; i<terrain.size(); i++)
                terrain[i].z+=3;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::K))
            for(int i=4; i<terrain.size(); i++)
                terrain[i].z-=3;
        ///#####################################

        for(int i=3; i<7; i++)
        {
            if(i%2 == 0)
            {
                terrain[world[i].wallie[2].coord[0]].z += tick;
                terrain[world[i].wallie[2].coord[1]].z += tick;
                terrain[world[i].wallie[2].coord[2]].z += tick;
                terrain[world[i].wallie[2].coord[3]].z += tick;
            }
            else
            {
                terrain[world[i].wallie[2].coord[0]].z -= tick;
                terrain[world[i].wallie[2].coord[1]].z -= tick;
                terrain[world[i].wallie[2].coord[2]].z -= tick;
                terrain[world[i].wallie[2].coord[3]].z -= tick;
            }
        }

        if(offsetx == 10)
        {
            tick = -tick;
        }
        else if(offsetx == -10)
        {
            tick = -tick;
        }
        offsetx += tick;

        c.setPos(cameraPos);
        c.setAngle(cameraAngle);
        c.setEyeDistance(eyeDistance);
        c.display();
    }
protected:
    sf::Font font;
    std::vector <sf::Vector3f> terrain;
    std::vector <Object> world;
    Camera c;
    sf::Vector3f cameraPos;
    sf::Vector3f cameraAngle;
    float eyeDistance;
    int terrainSize;
    void loadMap(std::string path);
    void OptLines();
    void OptDots();
    void showInfo();
    bool debug;
    float offsetx, offsety;
    int tick;
};

void SYNTH3D::showInfo()
{
    printToConsole("Position: " + stringifyf(cameraPos.x) + "   " + stringifyf(cameraPos.y) + "   " + stringifyf(cameraPos.z));
    printToConsole("Angle: " + stringifyf(cameraAngle.x) + "   " + stringifyf(cameraAngle.y) + "   " + stringifyf(cameraAngle.z));
    printToConsole("Eye Distance: " + stringifyf(eyeDistance));
}

void SYNTH3D::loadMap(std::string path)
{
    auto first = [](std::string file, std::string word) -> bool
    {
        for(int i=0; i<word.size(); i++)
            if(file[i] != word[i])
                return false;
        return true;
    };
    auto value = [](std::string file, int a) -> std::pair <float, bool>
    {
        a++;
        float result = 0;
        int index = 0;
        int adress = 0;
        bool endOfFile = false;
        if(file[0] >= '0' and file[0] <= '9')
            index = 1;
        if(index < a)
            for(int i=0; i<file.size() - 1; i++)
            {
                if(file[i] == '\t' and file[i+1] >= '0' and file[i+1] <= '9')
                    index++;
                if(index == a)
                    {
                        adress = i+1;
                        break;
                    }
            }
        bool dotIndicator = false;
        float decimalCounter = 10;
        while(adress < file.size() and file[adress] != '\t')
        {
            if(!dotIndicator and file[adress] != '.')
            {
                result *= 10;
                result += file[adress] - '0';
            }
            else if(dotIndicator)
            {
                result += (file[adress] - '0') / decimalCounter;
                decimalCounter *= 10;
            }
            if(file[adress] == '.')
                dotIndicator = true;
            adress++;
        }
        if(adress == file.size())
            endOfFile = true;
        return {result, endOfFile};
    };
    std::ifstream level;
    level.open("files/maps/synth3d/"+path+".dat");
    std::string line;
    Object tmpObject;
    ///wall tmpWall; ///Uwaga zeby przy wpisywaniu kilku scian nie bylo nienadpisanych wartosci z poprzedniego
    int indicator = 0;
    Wall tmpWall;
    int VertexCount = 0;
    if(level.is_open())
        while(getline(level, line))
            if(!line.empty() and line[0] != '/' and line[1] != '/')
            {
                switch(indicator)
                {
                case 0:
                    tmpObject.name = line;
                    indicator = 1;
                    break;
                case 1:
                    if(first(line, "opt"))
                        tmpObject.OptAllowed = (bool)value(line, 0).first;
                    else if(first(line, "Vertex"))
                        indicator = 2;
                    else if(first(line, "Wall"))
                        indicator = 3;
                    else if(line == "objectEnd")
                    {
                        world.push_back(tmpObject);
                        tmpObject.clear();
                        VertexCount = 0;
                        indicator = 0;
                    }
                    break;
                case 2:
                    if(line != "end")
                    {
                        sf::Vector3f tmp;
                        tmp.x = value(line, 0).first; ///tomabycfloat
                        tmp.y = value(line, 1).first;
                        tmp.z = value(line, 2).first;
                        terrain.push_back(tmp);
                        VertexCount++;
                    }
                    else
                        indicator = 1;
                    break;
                case 3:
                    if(line != "end")
                    {
                        if(first(line, "size"))
                            tmpWall.pSize = value(line, 0).first;
                        else if(first(line, "grid"))
                            tmpWall.grid = value(line, 0).first;
                        else if(first(line, "opt"))
                            tmpWall.OptAllowed = (bool)value(line, 0).first;
                        else if(first(line, "color"))
                        {
                            tmpWall.color.r = value(line, 0).first;
                            tmpWall.color.g = value(line, 1).first;
                            tmpWall.color.b = value(line, 2).first;
                            tmpWall.color.a = value(line, 3).first;
                        }
                        else if(first(line, "trans"))
                        {
                            tmpWall.trans.r = value(line, 0).first;
                            tmpWall.trans.g = value(line, 1).first;
                            tmpWall.trans.b = value(line, 2).first;
                            tmpWall.trans.a = value(line, 3).first;
                        }
                        else if(first(line, "vert"))
                        {
                            int i = 0;
                            bool endOfFile = false;
                            while(!endOfFile)
                            {
                                tmpWall.push_back(value(line, i).first + terrain.size() - VertexCount);
                                endOfFile = value(line, i).second;
                                i++;
                            }
                        }
                        else if(first(line, "lMask"))
                            for(int i=0; i<tmpWall.size(); i++)
                                tmpWall.drawable[i] = bool(value(line, i).first);
                        else if(first(line, "dMask"))
                            for(int i=0; i<tmpWall.size(); i++)
                                tmpWall.dotDraw[i] = bool(value(line, i).first);
                        else if(first(line, "lStrip"))
                        {
                            std::vector <int> strip;
                            int i = 0;
                            bool endOfFile = false;
                            while(!endOfFile)
                            {
                                strip.push_back(value(line, i).first + terrain.size() - VertexCount);
                                endOfFile = value(line, i).second;
                                i++;
                            }
                            tmpWall.lineStrip.push_back(strip);
                        }
                        else if(line[0] == '#')
                        {
                            tmpObject.push_back(tmpWall);
                            tmpWall.clear();
                        }
                    }
                    else
                        indicator = 1;
                    break;
                }
            }
}

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

bool SYNTH3D::onConsoleUpdate(std::vector<std::string> args)
{
    if(!args.empty())
    {
        if(args[0] == "print")
        {
            if(args.size() == 2)
            {
                if(args[1] == "graph")
                {
                    c.printGraph();
                    return true;
                }
                else if(args[1] == "info")
                {
                    showInfo();
                    return true;
                }
            }
        }
        else if(args[0] == "enable")
        {
            if(args.size() == 3)
            {
                if(args[1] == "cycleReduction")
                {
                    c.enableCycleReduction(bool(atoi(args[2].c_str())));
                    return true;
                }
                else if(args[1] == "wallIntersect")
                {
                    c.enableWallIntersect(bool(atoi(args[2].c_str())));
                    return true;
                }
                else if(args[1] == "randomSort")
                {
                    c.enableRandomSort(bool(atoi(args[2].c_str())));
                    return true;
                }
                else if(args[1] == "identifyWalls")
                {
                    c.enableIdentifyWalls(bool(atoi(args[2].c_str())));
                    return true;
                }
            }
        }
        else if(args[0] == "compare")
        {
            if(args.size() == 3)
            {
                c.compare(atoi(args[1].c_str()), atoi(args[2].c_str()));
                return true;
            }
        }
    }
    return false;
}

Camera::Camera(SYNTH3D* parent):
    p(parent),
    position({0, 0, -400}),
    angle({0, 0, 0}),
    vEye({0, 0, -10}),
    screenSize(3),
    scale(100),
    minDotSize(2),
    cycleReductionEnabled(false),
    wallIntersectEnabled(true),
    randomSortEnabled(false),
    identifyWallsEnabled(false)
{
    initCircleDef(32);
}

void Camera::enableCycleReduction(bool value)
{
    cycleReductionEnabled = value;
}

void Camera::enableWallIntersect(bool value)
{
    wallIntersectEnabled = value;
}

void Camera::enableRandomSort(bool value)
{
    randomSortEnabled = value;
}

void Camera::enableIdentifyWalls(bool value)
{
    identifyWallsEnabled = value;
}

void Camera::compare(int left, int right)
{
    if(left != right)
    {
        int counter = 0;
        Wall* leftie;
        Wall* rightie;
        bool leftWallFound = false;
        bool rightWallFound = false;

        for(int i=0; i<p->world.size(); i++)
            for(int j=0; j<p->world[i].size(); j++)
            {
                if(left == counter)
                {
                    leftie = &(p->world[i].wallie[j]);
                    leftWallFound = true;
                    p->printToConsole("First object: " + p->world[i].name);
                }
                if(right == counter)
                {
                    rightie = &(p->world[i].wallie[j]);
                    rightWallFound = true;
                    p->printToConsole("Second object: " + p->world[i].name);
                }
                counter++;
            }
        if(leftWallFound and rightWallFound)
        {
            if(wallSortingAlgorythm(leftie, rightie))
            {
                if(!wallSortingAlgorythm(rightie, leftie))
                    p->printToConsole("Wall " + stringify(right) + " is in front of wall " + stringify(left), sf::Color::Cyan);
                else
                    p->printToConsole("[error] Both walls seem to be in front of each other");
            }
            else
            {
                if(wallSortingAlgorythm(rightie, leftie))
                {
                    p->printToConsole("Wall " + stringify(left) + " is in front of wall " + stringify(right), sf::Color::Cyan);
                }
                else
                    p->printToConsole("[error] Both walls seem to be in behind of each other");
            }
        }
        else
        {
            if(!leftWallFound and !rightWallFound)
                p->printToConsole("[error] Couldn't find any of these walls");
            else if(!leftWallFound)
                p->printToConsole("[error] Could't find the first wall");
            else
                p->printToConsole("[error] Could't find the second wall");
        }
    }
    else
    {
        p->printToConsole("[error] Cannot compare wall X with wall X");
    }
}

void Camera::updateTerrain()
{
    spot3d.resize(p->terrain.size());
    spot2d.resize(spot3d.size());
    dot.resize(spot3d.size());
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
    //target.y = vec.x*sinz + vec.y*cosz;
    target.y = -vec.x*sinz - vec.y*cosz;
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
    spot -= vEye;
    float lenght = sqrtf(spot.x*spot.x + spot.y*spot.y + spot.z*spot.z);
    float lenghtZ = sqrtf(spot.x*spot.x + spot.y*spot.y);
    float sinZ = spot.y / lenghtZ;
    float cosZ = spot.x / lenghtZ;
    sf::Vector3f temp = spot;
    temp.x =  spot.x*cosZ + spot.y*sinZ;
    temp.y = -spot.x*sinZ + spot.y*cosZ;
    sf::Vector3f line = temp;
    float lenghtY = sqrtf(temp.x*temp.x + temp.z*temp.z);
    float sinY = temp.z / lenghtY;
    float cosY = temp.x / lenghtY;
    line.x = temp.x*cosY + temp.z*sinY;
    line.z = temp.z*cosY - temp.x*sinY;
    temp = sf::Vector3f(0, radius, 0);
    float sin = radius / lenght;
    float cos = sqrtf(lenght*lenght - radius*radius) / lenght;
    buffor.x = temp.x*cos - temp.y*sin;
    buffor.y = temp.x*sin + temp.y*cos;
    buffor.z = 0;
    buffor.x += lenght;
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
        drawPlane(screenBuffor, color);
        /* Zrob punkty pomiedzy liniami moze, uzyj wlasnego draw line zeby znac te punkty
        if(outlineThickness != 0)
            for(int i=0; i<screenBuffor.size(); i++)
                drawLine(screenBuffor[i],
                         screenBuffor[(i+1)%screenBuffor.size()],
                         dotSize(outlineThickness, sphere[i]),
                         dotSize(outlineThickness, sphere[(i+1)%sphere.size()]),
                         outlineColor,
                         whatever);
        */
    }
}

float Camera::dotSize(sf::Vector3f vec)
{
    return -eye/(vec.z - eye);
}

void Camera::drawLine(sf::Vector2f vec1, sf::Vector2f vec2, float size1, float size2, sf::Color color, sf::Vector2f& beginning)
{
    if((vec1.x > -size1 and vec1.x < p->window->getSize().x + size1 and vec1.y > -size1 and vec1.y < p->window->getSize().y + size1) or
       (vec2.x > -size2 and vec2.x < p->window->getSize().x + size2 and vec2.y > -size2 and vec2.y < p->window->getSize().y + size2) or
       lineIntersect(vec1, vec2, {0, 0}, {0, p->window->getSize().y + std::max(size1, size2)}) or
       lineIntersect(vec1, vec2, {0, p->window->getSize().y + std::max(size1, size2)}, {p->window->getSize().x + std::max(size1, size2), p->window->getSize().y + std::max(size1, size2)}) or
       lineIntersect(vec1, vec2, {p->window->getSize().x + std::max(size1, size2), p->window->getSize().y + std::max(size1, size2)}, {p->window->getSize().x + std::max(size1, size2), 0}) or
       lineIntersect(vec1, vec2, {p->window->getSize().x + std::max(size1, size2), 0}, {0, 0}))
    {
        sf::Vector2f norm;
        float temp, lenght;
        norm = (vec1 - vec2);
        lenght = sqrtf(norm.x*norm.x + norm.y*norm.y);
        norm.x /= lenght, norm.y /= lenght;
        temp = norm.x, norm.x = -norm.y, norm.y = temp;
        beginning = norm*size1;
        quadArray.push_back(sf::Vertex(vec1 + norm*size1, color));
        quadArray.push_back(sf::Vertex(vec2 + norm*size2, color));
        quadArray.push_back(sf::Vertex(vec2 - norm*size2, color));
        quadArray.push_back(sf::Vertex(vec1 - norm*size1, color));
    }
}

void Camera::debugDrawLine(sf::Vector2f vec1, sf::Vector2f vec2, float size1, float size2, sf::Color color, sf::Vector2f& beginning)
{
    if((vec1.x > -size1 and vec1.x < p->window->getSize().x + size1 and vec1.y > -size1 and vec1.y < p->window->getSize().y + size1) or
       (vec2.x > -size2 and vec2.x < p->window->getSize().x + size2 and vec2.y > -size2 and vec2.y < p->window->getSize().y + size2) or
       lineIntersect(vec1, vec2, {0, 0}, {0, p->window->getSize().y + std::max(size1, size2)}) or
       lineIntersect(vec1, vec2, {0, p->window->getSize().y + std::max(size1, size2)}, {p->window->getSize().x + std::max(size1, size2), p->window->getSize().y + std::max(size1, size2)}) or
       lineIntersect(vec1, vec2, {p->window->getSize().x + std::max(size1, size2), p->window->getSize().y + std::max(size1, size2)}, {p->window->getSize().x + std::max(size1, size2), 0}) or
       lineIntersect(vec1, vec2, {p->window->getSize().x + std::max(size1, size2), 0}, {0, 0}))
    {
        sf::Vector2f norm;
        float temp, lenght;
        norm = (vec1 - vec2);
        lenght = sqrtf(norm.x*norm.x + norm.y*norm.y);
        norm.x /= lenght, norm.y /= lenght;
        temp = norm.x, norm.x = -norm.y, norm.y = temp;
        beginning = norm*size1;
        debugArray.push_back(sf::Vertex(vec1 + norm*size1, color));
        debugArray.push_back(sf::Vertex(vec2 + norm*size2, color));
        debugArray.push_back(sf::Vertex(vec2 - norm*size2, color));
        debugArray.push_back(sf::Vertex(vec1 - norm*size1, color));
    }
}

void Camera::drawHalfDot(sf::Vector2f& spot, sf::Vector2f beginning, float& size, sf::Color color)
{
    /*
    if(spot.x > -size and spot.x < p->window->getSize().x + size and spot.y > -size and spot.y < p->window->getSize().y + size)
    {
        std::vector <sf::Vector2f> halfCircle;
        sf::Vector2f temp = beginning;
        for(int i=0; i<circleDef/2 +1; i++)
    {
        halfCircle.push_back(beginning);
        beginning.x = temp.x*cosCircle + temp.y*sinCircle;
        beginning.y = -temp.x
        *sinCircle + temp.y*cosCircle;
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
    */
}

void Camera::drawDotwBegin(sf::Vector2f& spot, sf::Vector2f beginning, float& size, sf::Color color)
{
    /*
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
    */
}

void Camera::drawDot(sf::Vector2f spot, float size, sf::Color color)
{
    if(spot.x > -size and spot.x < p->window->getSize().x + size and spot.y > -size and spot.y < p->window->getSize().y + size)
        for(int i=circleDefExample.size()/2, j=i-1; j>0; i++, j--)
            {
                quadArray.push_back(sf::Vertex(spot + circleDefExample[j-1]*size, color));
                quadArray.push_back(sf::Vertex(spot + circleDefExample[j]*size, color));
                quadArray.push_back(sf::Vertex(spot + circleDefExample[i]*size, color));
                quadArray.push_back(sf::Vertex(spot + circleDefExample[i+1]*size, color));
            }
}

void Camera::calcTerrain()
{
    for(int i=0; i<p->terrain.size(); i++)
    {
        spot3d[i] = vecTransform(p->terrain[i] - position);
        spot2d[i] = planeCross(spot3d[i], vEye);
        dot[i] = dotSize(spot3d[i]);
    }
}

void Camera::drawGridWall(Wall const& wallie, int n)
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
                     dotSize(point[0][i])*wallie.pSize,
                     dotSize(point[2][i])*wallie.pSize,
                     wallie.color,
                     whatever);
        else if(point[0][i].z < 0 and point[2][i].z < 0)
        {}
        else if(point[0][i].z < 0)
            drawLine(buffor = flatView(planeCross(point[0][i],point[2][i])),
                     flatView(planeCross(point[2][i], vEye)),
                     wallie.pSize,
                     dotSize(point[2][i])*wallie.pSize,
                     wallie.color,
                     whatever),
            drawDot(buffor, wallie.pSize, wallie.color);
        else
            drawLine(flatView(planeCross(point[0][i], vEye)),
                     buffor = flatView(planeCross(point[0][i], point[2][i])),
                     dotSize(point[0][i])*wallie.pSize,
                     wallie.pSize,
                     wallie.color,
                     whatever),
            drawDot(buffor, wallie.pSize, wallie.color);

        if(point[1][i].z >= 0 and point[3][i].z >= 0)
            drawLine(flatView(planeCross(point[1][i], vEye)),
                     flatView(planeCross(point[3][i], vEye)),
                     dotSize(point[1][i])*wallie.pSize,
                     dotSize(point[3][i])*wallie.pSize,
                     wallie.color,
                     whatever);
        else if(point[1][i].z < 0 and point[3][i].z < 0)
        {}
        else if(point[1][i].z < 0)
            drawLine(buffor = flatView(planeCross(point[1][i], point[3][i])),
                     flatView(planeCross(point[3][i], vEye)),
                     wallie.pSize,
                     dotSize(point[3][i])*wallie.pSize,
                     wallie.color,
                     whatever),
            drawDot(buffor, wallie.pSize, wallie.color);
        else
            drawLine(flatView(planeCross(point[1][i], vEye)),
                     buffor = flatView(planeCross(point[1][i], point[3][i])),
                     dotSize(point[1][i])*wallie.pSize,
                     wallie.pSize,
                     wallie.color,
                     whatever),
            drawDot(buffor, wallie.pSize, wallie.color);
    }
}

void Camera::drawPlane(std::vector <sf::Vector2f>& spot, sf::Color color)
{
        if(spot.size() == 4)
        {
            quadArray.push_back(sf::Vertex(spot[0], color));
            quadArray.push_back(sf::Vertex(spot[1], color));
            quadArray.push_back(sf::Vertex(spot[2], color));
            quadArray.push_back(sf::Vertex(spot[3], color));
        }
        else
        {
            if(spot.size()%2 == 1)
            {
            quadArray.push_back(sf::Vertex(spot[0], color));
            quadArray.push_back(sf::Vertex(spot[0], color));
            quadArray.push_back(sf::Vertex(spot[spot.size() - 1], color));
            quadArray.push_back(sf::Vertex(spot[spot.size() - 2], color));
            }
            for(int i=spot.size()/2, j=i-1; j>0; i++, j--)
            {
                quadArray.push_back(sf::Vertex(spot[j-1], color));
                quadArray.push_back(sf::Vertex(spot[j], color));
                quadArray.push_back(sf::Vertex(spot[i], color));
                quadArray.push_back(sf::Vertex(spot[i+1], color));
            }
        }
}

void Camera::debugDrawPlane(std::vector <sf::Vector2f>& spot, sf::Color color)
{
        if(spot.size() == 4)
        {
            debugArray.push_back(sf::Vertex(spot[0], color));
            debugArray.push_back(sf::Vertex(spot[1], color));
            debugArray.push_back(sf::Vertex(spot[2], color));
            debugArray.push_back(sf::Vertex(spot[3], color));
        }
        else
        {
            if(spot.size()%2 == 1)
            {
            debugArray.push_back(sf::Vertex(spot[0], color));
            debugArray.push_back(sf::Vertex(spot[0], color));
            debugArray.push_back(sf::Vertex(spot[spot.size() - 1], color));
            debugArray.push_back(sf::Vertex(spot[spot.size() - 2], color));
            }
            for(int i=spot.size()/2, j=i-1; j>0; i++, j--)
            {
                debugArray.push_back(sf::Vertex(spot[j-1], color));
                debugArray.push_back(sf::Vertex(spot[j], color));
                debugArray.push_back(sf::Vertex(spot[i], color));
                debugArray.push_back(sf::Vertex(spot[i+1], color));
            }
        }
}


std::vector <sf::Vector2f> Camera::wallToPoly(Wall* wallie)
{
    std::vector <sf::Vector2f> spot;
    std::vector <int> usefulSpot;
    for(int i=0; i<wallie->size(); i++)
    {
        if(spot3d[ wallie->coord[i] ].z >= 0)
            usefulSpot.push_back(i);
    }
    if(!usefulSpot.empty())
    {
        if(usefulSpot.size() == wallie->size())
        {
            for(int i=0; i<usefulSpot.size(); i++)
                spot.push_back(flatView(spot2d[wallie->coord[i]]));
        }
        else
        {
            int first;
            usefulSpot.clear();
            for(int i=0, j=0; i<2*wallie->size(); i++)
                if(spot3d[ wallie->coord[i%wallie->size()] ].z < 0 or j > 0)
                {
                    if(j == 0)
                        j = 1;
                    if(spot3d[ wallie->coord[i%wallie->size()] ].z >= 0 and j != 3)
                    {
                        usefulSpot.push_back(i%wallie->size());
                        if(j != 2)
                            first = (i-1)%wallie->size();
                        j = 2;
                    }
                    else if(j == 2)
                        j = 3;
                }
            spot.push_back(flatView(planeCross(spot3d[wallie->coord[  usefulSpot[0]    ] ],
                                               spot3d[wallie->coord[ (usefulSpot[0] - 1)%wallie->size() ]])));
            for(int i=0; i<usefulSpot.size(); i++)
                spot.push_back(flatView(spot2d[ wallie->coord[usefulSpot[i]] ]));
            spot.push_back(flatView(planeCross(spot3d[ wallie->coord [usefulSpot[ usefulSpot.size() - 1 ]    ] ],
                                               spot3d[ wallie->coord[(usefulSpot[ usefulSpot.size() - 1 ] + 1)%wallie->size()] ])));
        }
    }
    return spot;
}

void Camera::drawWall(Wall const& wallie)
{
    std::vector <sf::Vector2f> spot;
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
                spot.push_back(flatView(spot2d[wallie.coord[i]]));
            if(wallie.trans != sf::Color::Transparent)
                drawPlane(spot, wallie.trans);
            if(wallie.grid > 1 and wallie.size() == 4)
                drawGridWall(wallie, wallie.grid);
            for(int i=0; i<wallie.size(); i++)
                if(wallie.drawable[i])
                    drawLine(spot[i],
                             spot[(i+1)%spot.size()],
                             dot[wallie.coord[i]]*wallie.pSize,
                             dot[wallie.coord[(i+1)%spot.size()]]*wallie.pSize,
                             wallie.color,
                             halfDotBegin[wallie.coord[i]]);
            if(!wallie.lineStrip.empty())
                for(int i=0; i<wallie.lineStrip.size(); i++)
                    for(int j=0; j<wallie.lineStrip[i].size()-1; j++)
                    {
                        drawDot(flatView(spot2d[wallie.lineStrip[i][j]]), dot[ wallie.lineStrip[i][j] ] * wallie.pSize, wallie.color);
                        drawLine(flatView(spot2d[wallie.lineStrip[i][j]]), flatView(spot2d[wallie.lineStrip[i][j+1]]), dot[ wallie.lineStrip[i][j] ] * wallie.pSize, dot[ wallie.lineStrip[i][j+1] ] * wallie.pSize, wallie.color, whatever);
                    }
            for(int i=0; i<spot.size(); i++)
                if(dot[wallie.coord[i]]*wallie.pSize > minDotSize and wallie.dotDraw[i])
                  //drawHalfDot(spot[i], halfDotBegin[wallie.coord[(i)%wallie.size()]], dot[i], wallie.color);
                  //drawDotwBegin(spot[i], halfDotBegin[wallie.coord[(i+first*indicator)%wallie.size()]], dot[i], sf::Color::Magenta);
                  drawDot(spot[i], dot[wallie.coord[i]]*wallie.pSize, wallie.color);
        }
        else
        {
            std::vector <float> dot2d;
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
            dot2d.push_back(wallie.pSize);
            for(int i=0; i<usefulSpot.size(); i++)
            {
                spot.push_back(flatView(spot2d[ wallie.coord[usefulSpot[i]] ]));
                dot2d.push_back(dot[ wallie.coord[usefulSpot[i]] ]*wallie.pSize);
            }
            spot.push_back(flatView(planeCross(spot3d[ wallie.coord [usefulSpot[ usefulSpot.size() - 1 ]    ] ],
                                               spot3d[ wallie.coord[(usefulSpot[ usefulSpot.size() - 1 ] + 1)%wallie.size()] ])));
            dot2d.push_back(wallie.pSize);
            if(wallie.trans != sf::Color::Transparent)
                drawPlane(spot, wallie.trans);
            if(wallie.grid > 1 and wallie.size() == 4)
                drawGridWall(wallie, wallie.grid);
            for(int i=0; i<spot.size() - 1; i++)
                if(wallie.drawable[(i+first)%wallie.size()])
                    drawLine(spot[i],
                         spot[(i+1)%spot.size()],
                         dot2d[i],
                         dot2d[(i+1)%spot.size()],
                         wallie.color,
                         halfDotBegin[wallie.coord[(i+first)%wallie.size()]]);
            if(!wallie.lineStrip.empty())
                for(int i=0; i<wallie.lineStrip.size(); i++)
                    for(int j=0; j<wallie.lineStrip[i].size()-1; j++)
                    {
                        if(spot3d[wallie.lineStrip[i][j]].z > 0)
                        {
                            drawDot(flatView(spot2d[wallie.lineStrip[i][j]]), dot[ wallie.lineStrip[i][j] ] * wallie.pSize, wallie.color);
                            if(spot3d[wallie.lineStrip[i][j+1]].z > 0)
                                drawLine(flatView(spot2d[wallie.lineStrip[i][j]]), flatView(spot2d[wallie.lineStrip[i][j+1]]), dot[ wallie.lineStrip[i][j] ] * wallie.pSize, dot[ wallie.lineStrip[i][j+1] ] * wallie.pSize, wallie.color, whatever);
                            else
                            {
                                drawLine(flatView(spot2d[wallie.lineStrip[i][j]]),
                                         flatView(planeCross(spot3d[wallie.lineStrip[i][j]], spot3d[wallie.lineStrip[i][j+1]])),
                                         dot[ wallie.lineStrip[i][j] ] * wallie.pSize,
                                         wallie.pSize,
                                         wallie.color,
                                         whatever);
                                drawDot(flatView(planeCross(spot3d[wallie.lineStrip[i][j]], spot3d[wallie.lineStrip[i][j+1]])), wallie.pSize, wallie.color);
                            }
                        }
                        else if(spot3d[wallie.lineStrip[i][j+1]].z > 0)
                        {
                            drawLine(flatView(spot2d[wallie.lineStrip[i][j+1]]),
                                         flatView(planeCross(spot3d[wallie.lineStrip[i][j+1]], spot3d[wallie.lineStrip[i][j]])),
                                         dot[ wallie.lineStrip[i][j+1] ] * wallie.pSize,
                                         wallie.pSize,
                                         wallie.color,
                                         whatever);
                            drawDot(flatView(planeCross(spot3d[wallie.lineStrip[i][j+1]], spot3d[wallie.lineStrip[i][j]])), wallie.pSize, wallie.color);
                        }
                    }
            for(int i=0; i<spot.size(); i++)
                if(dot2d[i] > minDotSize and wallie.dotDraw[(i+first)%wallie.size()])
                  //drawHalfDot(spot[i], halfDotBegin[wallie.coord[(i)%wallie.size()]], dot2d[i], wallie.color);
                  //drawDotwBegin(spot[i], halfDotBegin[wallie.coord[(i+first)%wallie.size()]], dot2d[i], wallie.color);
                  drawDot(spot[i], dot2d[i], wallie.color);
        }
    }
}

bool Camera::lineIntersect(sf::Vector2f u0, sf::Vector2f v0, sf::Vector2f u1, sf::Vector2f v1)
{
    v0 -= u0;
    v1 -= u1;
    float det = v1.x*v0.y - v0.x*v1.y;
    if(det != 0)
    {
        float s = (1/det) *  (  (u0.x - u1.x)*v0.y - (u0.y - u1.y)*v0.x);
        float t = (1/det) * -( -(u0.x - u1.x)*v1.y + (u0.y - u1.y)*v1.x);
        if(s >= 0 and s <= 1 and t >= 0 and t <= 1)
            return true;
    }
    return false;
}

bool Camera::rightSide(sf::Vector2f line1, sf::Vector2f line2, sf::Vector2f point)
{
    sf::Vector2f v1 = line2 - line1;
    sf::Vector2f p = point - line1;
    return v1.x*p.y - v1.y*p.x < 0;
}

int Camera::planeSide(sf::Vector3f& center, sf::Vector3f& top, sf::Vector3f& side, sf::Vector3f& point)
{
    sf::Vector3f planeTop = top - center;
    sf::Vector3f planeSide = side - center;
    sf::Vector3f planeNormal;
    planeNormal.x = planeSide.y*planeTop.z - planeTop.y*planeSide.z;
    planeNormal.y = planeSide.z*planeTop.x - planeTop.z*planeSide.x;
    planeNormal.z = planeSide.x*planeTop.y - planeTop.x*planeSide.y;
    sf::Vector3f posVector = point - center;
    float dotProduct = planeNormal.x*posVector.x + planeNormal.y*posVector.y + planeNormal.z*posVector.z;
    if(dotProduct < -0.1f)
        return 1;
    else if(dotProduct > 0.1f)
        return -1;
    return 0;
}

bool Camera::wallIntersect(Wall* wallie1, Wall* wallie2)
{
    int leftBelowZero = 0;
    int rightBelowZero = 0;
    for(int i=0; i<wallie1->size(); i++)
        if(spot3d[wallie1->coord[i]].z < 0)
        {
            leftBelowZero++;
        }
    for(int i=0; i<wallie2->size(); i++)
        if(spot3d[wallie2->coord[i]].z < 0)
        {
            rightBelowZero++;
        }
    if(leftBelowZero == 0 and rightBelowZero == 0)
        return wallIntersectOS(wallie1, wallie2);

    if(leftBelowZero == wallie1->size() or rightBelowZero == wallie2->size())
        return false;

    std::vector <sf::Vector2f> poly1 = wallToPoly(wallie1);
    std::vector <sf::Vector2f> poly2 = wallToPoly(wallie2);

    return polygonIntersect(poly1, poly2);
}

bool Camera::polygonIntersectOlder(std::vector <sf::Vector2f> poly1, std::vector <sf::Vector2f> poly2)
{
    sf::Vector2f center1, center2;
    float maxSquareRad1, maxSquareRad2;
    for(int i=0; i<poly1.size(); i++)
        center1 += poly1[i];
    center1 /= float(poly1.size());
    for(int i=0; i<poly1.size(); i++)
        maxSquareRad1 = std::max(maxSquareRad1, poly1[i].x*poly1[i].x + poly1[i].y+poly1[i].y);
    for(int i=0; i<poly2.size(); i++)
        center2 += poly2[i];
    center2 /= float(poly2.size());
    for(int i=0; i<poly2.size(); i++)
        maxSquareRad2 = std::max(maxSquareRad2, poly2[i].x*poly2[i].x + poly2[i].y+poly2[i].y);
    float squareCenterDist = (center1.x-center2.x)*(center1.x-center2.x) +
                             (center1.y-center2.y)*(center1.y-center2.y);
    if(sqrtf(squareCenterDist) >= sqrtf(maxSquareRad1) + sqrtf(maxSquareRad2))
        return false;

    for(int i=0; i<poly2.size(); i++)
    {
        bool pointInside = true;
        for(int j=0; j<poly1.size(); j++)
            if(rightSide(poly1[j], poly1[(j+1)%poly1.size()], poly2[i]) !=
               rightSide(poly1[j], poly1[(j+1)%poly1.size()], center1))
            {
                pointInside = false;
                break;
            }
        if(pointInside)
            return true;
    }
    for(int i=0; i<poly1.size(); i++)
    {
        bool pointInside = true;
        for(int j=0; j<poly2.size(); j++)
            if(rightSide(poly2[j], poly2[(j+1)%poly2.size()], poly1[i]) !=
               rightSide(poly2[j], poly2[(j+1)%poly2.size()], center2))
            {
                pointInside = false;
                break;
            }
        if(pointInside)
            return true;
    }

    for(int i=0; i<poly1.size(); i++)
    {
        for(int j=0; j<poly2.size(); j++)
        {
            if(lineIntersect( poly1[i], poly1[(i+1)%poly1.size()], poly2[j], poly2[(j+1)%poly2.size()] ))
                return true;
        }
    }
    return false;
}

bool Camera::polygonIntersectOld(std::vector <sf::Vector2f> poly1, std::vector <sf::Vector2f> poly2)
{
    sf::Vector2f center1, center2;
    float maxSquareRad1, maxSquareRad2;
    for(int i=0; i<poly1.size(); i++)
        center1 += poly1[i];
    center1 /= float(poly1.size());
    for(int i=0; i<poly1.size(); i++)
        maxSquareRad1 = std::max(maxSquareRad1, poly1[i].x*poly1[i].x + poly1[i].y+poly1[i].y);
    for(int i=0; i<poly2.size(); i++)
        center2 += poly2[i];
    center2 /= float(poly2.size());
    for(int i=0; i<poly2.size(); i++)
        maxSquareRad2 = std::max(maxSquareRad2, poly2[i].x*poly2[i].x + poly2[i].y+poly2[i].y);
    float squareCenterDist = (center1.x-center2.x)*(center1.x-center2.x) +
                             (center1.y-center2.y)*(center1.y-center2.y);

    if(sqrtf(squareCenterDist) >= sqrtf(maxSquareRad1) + sqrtf(maxSquareRad2))
        return false;

    for(int i=0; i<poly2.size(); i++)
    {
        bool pointInside = true;
        for(int j=0; j<poly1.size(); j++)
            if(rightSide(poly1[j], poly1[(j+1)%poly1.size()], poly2[i]) !=
               rightSide(poly1[j], poly1[(j+1)%poly1.size()], center1))
            {
                pointInside = false;
                break;
            }
        if(pointInside)
            return true;
    }
    for(int i=0; i<poly1.size(); i++)
    {
        bool pointInside = true;
        for(int j=0; j<poly2.size(); j++)
            if(rightSide(poly2[j], poly2[(j+1)%poly2.size()], poly1[i]) !=
               rightSide(poly2[j], poly2[(j+1)%poly2.size()], center2))
            {
                pointInside = false;
                break;
            }
        if(pointInside)
            return true;
    }
    return false;
}

bool Camera::polygonIntersect(std::vector <sf::Vector2f> poly1, std::vector <sf::Vector2f> poly2)
{
    for(int i=0; i<poly1.size(); i++)
    {
        bool firstSide = rightSide(poly1[i], poly1[(i+1)%poly1.size()], poly2[0]);
        bool allOnOneSide = true;
        for(int j=1; j<poly2.size(); j++)
            if(firstSide != rightSide(poly1[i], poly1[(i+1)%poly1.size()], poly2[j]))
            {
                allOnOneSide = false;
                break;
            }
        if(allOnOneSide)
        {
            sf::Vector2f opositePoint;
            for(int j = (i+2)%poly1.size(); j<poly1.size() + (i+2)%poly1.size(); j++)
                if(poly1[j%poly1.size()] != poly1[i] and
                   poly1[j%poly1.size()] != poly1[(i+1)%poly1.size()])
                {
                   opositePoint = poly1[j%poly1.size()];
                   break;
                }
            if(firstSide != rightSide(poly1[i], poly1[(i+1)%poly1.size()], opositePoint))
                return false;
        }
    }

    for(int i=0; i<poly2.size(); i++)
    {
        bool firstSide = rightSide(poly2[i], poly2[(i+1)%poly2.size()], poly1[0]);
        bool allOnOneSide = true;
        for(int j=1; j<poly1.size(); j++)
            if(firstSide != rightSide(poly2[i], poly2[(i+1)%poly2.size()], poly1[j]))
            {
                allOnOneSide = false;
                break;
            }
        if(allOnOneSide)
        {
            sf::Vector2f opositePoint;
            for(int j = (i+2)%poly2.size(); j<poly2.size() + (i+2)%poly2.size(); j++)
                if(poly2[j%poly2.size()] != poly2[i] and
                   poly2[j%poly2.size()] != poly2[(i+1)%poly2.size()])
                {
                   opositePoint = poly2[j%poly2.size()];
                   break;
                }
            if(firstSide != rightSide(poly2[i], poly2[(i+1)%poly2.size()], opositePoint))
                return false;
        }
    }

    return true;
}

bool Camera::wallIntersectOS(Wall* left, Wall* right)
{
    for(int i=0; i<left->size(); i++)
    {
        bool firstSide = rightSide(flatView(spot2d[left->coord[i]]), flatView(spot2d[left->coord[(i+1)%left->size()]]), flatView(spot2d[right->coord[0]]));
        bool allOnOneSide = true;
        for(int j=1; j<right->size(); j++)
            if(firstSide != rightSide(flatView(spot2d[left->coord[i]]), flatView(spot2d[left->coord[(i+1)%left->size()]]), flatView(spot2d[right->coord[j]])))
            {
                allOnOneSide = false;
                break;
            }
        if(allOnOneSide)
        {
            sf::Vector2f opositePoint;
            for(int j = (i+2)%left->size(); j<left->size() + (i+2)%left->size(); j++)
                if(flatView(spot2d[left->coord[j%left->size()]]) != flatView(spot2d[left->coord[i]]) and
                   flatView(spot2d[left->coord[j%left->size()]]) != flatView(spot2d[left->coord[(i+1)%left->size()]]))
                {
                   opositePoint = flatView(spot2d[left->coord[j%left->size()]]);
                   break;
                }
            if(firstSide != rightSide(flatView(spot2d[left->coord[i]]), flatView(spot2d[left->coord[(i+1)%left->size()]]), opositePoint))
                return false;
        }
    }

    for(int i=0; i<right->size(); i++)
    {
        bool firstSide = rightSide(flatView(spot2d[right->coord[i]]), flatView(spot2d[right->coord[(i+1)%right->size()]]), flatView(spot2d[left->coord[0]]));
        bool allOnOneSide = true;
        for(int j=1; j<left->size(); j++)
            if(firstSide != rightSide(flatView(spot2d[right->coord[i]]), flatView(spot2d[right->coord[(i+1)%right->size()]]), flatView(spot2d[left->coord[j]])))
            {
                allOnOneSide = false;
                break;
            }
        if(allOnOneSide)
        {
            sf::Vector2f opositePoint;
            for(int j = (i+2)%right->size(); j<right->size() + (i+2)%right->size(); j++)
                if(flatView(spot2d[right->coord[j%right->size()]]) != flatView(spot2d[right->coord[i]]) and
                   flatView(spot2d[right->coord[j%right->size()]]) != flatView(spot2d[right->coord[(i+1)%right->size()]]))
                {
                   opositePoint = flatView(spot2d[right->coord[j%right->size()]]);
                   break;
                }
            if(firstSide != rightSide(flatView(spot2d[right->coord[i]]), flatView(spot2d[right->coord[(i+1)%right->size()]]), opositePoint))
                return false;
        }
    }

    return true;
}

bool Camera::wallSortingAlgorythm(Wall* lhs, Wall* rhs)
{
    ///True - to z prawej jest blizej
    std::vector <int> leftWall;
    std::vector <int> rightWall;

    int leftC = lhs->coord[0];
    int leftT;
    int leftS;
    for(int i=1; i<lhs->size(); i++)
        if(leftC != lhs->coord[i])
        {
            leftT = lhs->coord[i];
            if((i+1) < lhs->size())
                for(int j=i+1; j<lhs->size(); j++)
                    if(leftC != lhs->coord[j] and leftT != lhs->coord[j])
                    {
                        leftS = lhs->coord[j];
                        break;
                    }
            else
                return false;
            break;
        }
    int rightC = rhs->coord[0];
    int rightT;
    int rightS;
    for(int i=1; i<rhs->size(); i++)
        if(rightC != rhs->coord[i])
        {
            rightT = rhs->coord[i];
            if((i+1) < lhs->size())
                for(int j=i+1; j<rhs->size(); j++)
                    if(rightC != rhs->coord[j] and rightT != rhs->coord[j])
                    {
                        rightS = rhs->coord[j];
                        break;
                    }
            else
                return false;
            break;
        }

    for(int i=0; i<lhs->size(); i++)
    {
        int value = planeSide(spot3d[rightC], spot3d[rightT], spot3d[rightS], spot3d[lhs->coord[i]]);
        if(value != 0)
            leftWall.push_back(value);
    }
    if(leftWall.empty())
        return false;

    for(int i=0; i<rhs->size(); i++)
    {
        int value = planeSide(spot3d[leftC], spot3d[leftT], spot3d[leftS], spot3d[rhs->coord[i]]);
        if(value != 0)
            rightWall.push_back(value);
    }
    if(rightWall.empty())
        return false;
    int leftWallEyePos = planeSide(spot3d[leftC], spot3d[leftT], spot3d[leftS], vEye);
    int rightWallEyePos = planeSide(spot3d[rightC], spot3d[rightT], spot3d[rightS], vEye);
    if(rightWallEyePos == 0 and leftWallEyePos == 0)
        return false;

    bool leftAllOnOneSide = true;
    bool rightAllOnOneSide = true;
    for(int i=1; i<leftWall.size(); i++)
        if(leftWall[0] != leftWall[i])
        {
            leftAllOnOneSide = false;
            break;
        }
    for(int i=1; i<rightWall.size(); i++)
        if(rightWall[0] != rightWall[i])
        {
            rightAllOnOneSide = false;
            break;
        }


    bool leftWallFirst = false;
    bool rightWallFirst = false;
    if(leftAllOnOneSide)
        if(leftWall[0] == rightWallEyePos)
            leftWallFirst = true;
        else
            rightWallFirst = true;

    if(rightAllOnOneSide)
        if(rightWall[0] == leftWallEyePos)
            rightWallFirst = true;
        else
            leftWallFirst = true;

    if(leftWallFirst != rightWallFirst)
        return rightWallFirst;
    return false;

}

void Camera::cycleReduction(std::vector <std::vector <int> >& graph, std::vector <int>& graphLevel)
{
    int visited[graph.size()][graph.size()];
    int father[graph.size()];
    memset(visited, -1, sizeof(int) * graph.size() * graph.size());

    for(int i=0; i<graph.size(); i++)
        if(!graph[i].empty() and visited[i][ graph[i][0] ] == -1)
        {
            std::stack <int> s;
            s.push(i);
            while(!s.empty())
            {
                int n = s.top();
                s.pop();
                for(int j=graph[n].size() - 1; j>=0; j--)
                {
                    if(visited[n][ graph[n][j] ] == -1)
                    {
                        s.push(graph[n][j]);
                        visited[n][graph[n][j]] = i;

                        father[graph[n][j]] = n;
                    }
                    else if(visited[n][ graph[n][j] ] == i and visited[father[n]][n] != -2)
                    {
                        graphLevel[n]--;
                        visited[father[n]][n] = -2;
                        for(int i=graph[ father[n] ].size()-1; i>=0; i--)
                            if(graph[ father[n] ][i] == n)
                            {
                                graph[ father[n] ].erase(graph[ father[n] ].begin() + i);
                                break;
                            }
                    }
                }
            }
        }
}

void Camera::createGraph(std::vector <std::vector <int> >& graph, std::vector <int>& graphLevel, std::vector <Wall*> tempOrder)
{
    for(int i=0; i<tempOrder.size(); i++)
    {
        std::vector <int> temp;
        for(int j=0; j<tempOrder.size(); j++)
            if(i!=j and
               (!wallIntersectEnabled or wallIntersect(tempOrder[i], tempOrder[j])) and
               wallSortingAlgorythm(tempOrder[i], tempOrder[j])
               )
            {
                temp.push_back(j);
                graphLevel[j]++;
            }
        graph.push_back(temp);
    }
}

void Camera::printGraph()
{
    p->printToConsole(cmdOutput);
}

void Camera::identifyWalls(std::vector <Wall*> wallie)
{
    for(int j=0; j<wallie.size(); j++)
    {
        std::vector <sf::Vector2f> polygon = wallToPoly(wallie[j]);
        if(!polygon.empty())
        {
            sf::Vector2f centerPosition2d;
            for(int i=0; i<polygon.size(); i++)
                centerPosition2d += polygon[i];
            centerPosition2d /= float(polygon.size());

            sf::Vector3f centerPosition3d;
            for(int i=0; i<wallie[j]->size(); i++)
                centerPosition3d += spot3d[wallie[j]->coord[i]];
            centerPosition3d /= float(wallie[j]->size());

            sf::Text t;
            int characterSize = 30; //roundf(dotSize(300, centerPosition3d));
            if(characterSize > 0)
            {
                if(characterSize > 50)
                    characterSize = 50;
                t.setCharacterSize(characterSize);
                t.setFont(p->font);
                t.setOutlineThickness(2);
                t.setString(stringify(j));
                t.setPosition(centerPosition2d);
                textArray.push_back(t);
            }
        }
    }
}

void Camera::topologicalSort(std::vector <std::vector <int> >& graph, std::vector <int>& graphLevel, std::vector <Wall*> tempOrder)
{
    std::queue <int> que;
    for(int i=0; i<graphLevel.size(); i++)
        if(graphLevel[i] == 0)
            que.push(i);

    cmdOutput += "\nWall Order:\n";
    if(que.empty())
    {
        std::cout << "\nERROR: Topological sort queue empty";
        p->printToConsole("[error] Topological sort queue empty");
    }
    int counter = 0;
    while(!que.empty())
    {
            wallOrder.push_back(tempOrder[que.front()]);
            cmdOutput += stringify(que.front()) + " ";
            if(counter%40 == 39)
                cmdOutput+= "\n";
            counter++;
            for(int i=0; i<graph[que.front()].size(); i++)
            {
                graphLevel[graph[que.front()][i]]--;
                if(graphLevel[graph[que.front()][i]] == 0)
                    que.push(graph[que.front()][i]);
            }
        que.pop();
    }
    if(counter < tempOrder.size())
    {
        std::cout << "\nERROR: Topological sort failure";
        p->printToConsole("[error] Topological sort failure");
    }
}

void Camera::randSort(std::vector <Wall*> tempOrder)
{
    while(!tempOrder.empty())
    {
        int randomValue = rand()%tempOrder.size();
        wallOrder.push_back(tempOrder[randomValue]);
        tempOrder.erase(tempOrder.begin() + randomValue);
    }
}

void Camera::displayGraph(std::vector <std::vector <int> >& graph, std::vector <int>& graphLevel)
{
    /*
    if(cmdOutput.size() > 0 or p->cmdOutput.size() > 0)
        cmdOutput += "\n";
    cmdOutput += "Graph Level |";
    for(int i=0; i<graphLevel.size(); i++)
        cmdOutput += stringify(i) + "|";
    cmdOutput += "\n            |";
    for(int i=0; i<graphLevel.size(); i++)
        cmdOutput += "-|";
    cmdOutput += "\n            |";
    for(int i=0; i<graphLevel.size(); i++)
        cmdOutput += stringify(graphLevel[i]) + "|";
    */

    for(int i=0; i<graph.size(); i++)
    {
        cmdOutput += "\nWall " + stringify(i) + " results:";
        for(int j=0; j<graph[i].size(); j++)
            cmdOutput += " " + stringify(graph[i][j]);
    }


    /*
    std::cout << "\nGraph Level |0|1|2|3|4|5|6|";
    std::cout << "\n            |-|-|-|-|-|-|-|";
    std::cout << "\n            |";
    for(int i=0; i<graphLevel.size(); i++)
        std::cout << graphLevel[i] << "|";

    for(int i=0; i<graph.size(); i++)
    {
        std::cout << "\nWall " << i << " results:";
        for(int j=0; j<graph[i].size(); j++)
            std::cout << " " << graph[i][j];
    }
    */
}

void Camera::wallSort()
{
    ///poza ekranem nie dodawaj do tempOrder
    std::vector <std::vector <int> > graph;
    std::vector <Wall*> tempOrder;
    wallOrder.clear();
    for(int i=0; i<p->world.size(); i++)
        for(int j=0; j<p->world[i].size(); j++)
        {
            tempOrder.push_back(&(p->world[i].wallie[j]));
        }
    std::vector <int> graphLevel;
    graphLevel.resize(tempOrder.size());

    if(!randomSortEnabled)
    {
        createGraph(graph, graphLevel, tempOrder);
        cmdOutput+= "\nBEFORE:";
        displayGraph(graph, graphLevel);
        if(cycleReductionEnabled)
        {
            cycleReduction(graph, graphLevel);
            cmdOutput+= "\nAFTER:";
            displayGraph(graph, graphLevel);
        }
        topologicalSort(graph, graphLevel, tempOrder);
    }
    else
        randSort(tempOrder);

    if(identifyWallsEnabled)
        identifyWalls(tempOrder);
}

void Camera::display()
{
    cmdOutput.clear();
    calcAngle();
    calcTerrain();
    wallSort();

    sf::Vector3f place = sf::Vector3f(100, 50, 300);
    place = vecTransform(place);
    drawSphere(place, 10, sf::Color::Magenta);

    for(int i=0; i<wallOrder.size(); i++)
        drawWall(*wallOrder[i]);

    p->window->clear();
    p->window->draw(&quadArray[0], quadArray.size(), sf::Quads);
    p->window->draw(&debugArray[0], debugArray.size(), sf::Quads);
    for(int i=0; i<textArray.size(); i++)
        p->window->draw(textArray[i]);
    quadArray.clear();
    debugArray.clear();
    textArray.clear();
}

#endif //SYNTH3D_HPP
