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

sf::Vector3f operator/(sf::Vector3f lhs, const sf::Vector3f& rhs)
{
    return {lhs.x/rhs.x, lhs.y/rhs.y, lhs.z/rhs.z};
}

sf::Vector3f operator*(sf::Vector3f lhs, const sf::Vector3f& rhs)
{
    return {lhs.x*rhs.x, lhs.y*rhs.y, lhs.z*rhs.z};
}

class collisionPacket
{
public:
    sf::Vector3f eRadius;
    sf::Vector3f R3Velocity;
    sf::Vector3f R3Position;
    sf::Vector3f velocity;
    sf::Vector3f normalizedVelocity;
    sf::Vector3f basePoint;
    bool foundCollision;
    double nearestDistance;
    sf::Vector3f intersectionPoint;
};

class Wall
{
public:
    Wall();
    std::string name;
    std::vector <int> coord;
    std::vector <bool> drawable;
    std::vector <bool> dotDraw;
    std::vector <std::vector <int> > lineStrip;
    sf::Color color=sf::Color::Green;
    sf::Color trans=sf::Color::Transparent;
    float pSize = 100;
    int grid = 0;
    bool OptAllowed = false;
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
    name.clear();
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
    int beginning;
    int finish;
    void calcStartAndFinish();
};

Object::Object():
    OptAllowed(false),
    beginning(2000000),
    finish(0)
{
}

void Object::clear()
{
    wallie.clear();
    OptAllowed = true;
    name.clear();
}

void Object::push_back(Wall w)
{
    wallie.push_back(w);
    for(int i=0; i<w.size(); i++)
    {
        if(w.coord[i] < beginning)
            beginning = w.coord[i];
        if(w.coord[i] > finish)
            finish = w.coord[i];
    }
    for(int i=0; i<w.lineStrip.size(); i++)
    {
        for(int j=0; j<w.lineStrip[i].size(); j++)
        {
            if(w.coord[i] < beginning)
                beginning = w.coord[i];
            if(w.coord[i] > finish)
                finish = w.coord[i];
        }
    }
}

unsigned int Object::size()
{
    return wallie.size();
}

void Object::calcStartAndFinish()
{
    beginning = 2000000000;
    finish = 0;
    for(int j=0; j<size(); j++)
    {
        for(int k=0; k < wallie[j].size(); k++)
        {
            if(wallie[j].coord[k] > finish)
                finish = wallie[j].coord[k];
            if(wallie[j].coord[k] < beginning)
                beginning = wallie[j].coord[k];
        }
    }
    for(int j=0; j<size(); j++)
    {
        for(int k=0; k<wallie[j].lineStrip.size(); k++)
        {
            for(int m=0; m<wallie[j].lineStrip[k].size(); m++)
            {
                if(wallie[j].lineStrip[k][m] > finish)
                    finish = wallie[j].lineStrip[k][m];
                if(wallie[j].lineStrip[k][m] < beginning)
                    beginning = wallie[j].lineStrip[k][m];
            }
        }
    }
}

class Plane
{
public:
    Plane(const sf::Vector3f& originGiven, const sf::Vector3f& normalGiven);
    Plane(const sf::Vector3f& p1, const sf::Vector3f& p2, const sf::Vector3f& p3);
    float equation[4];
    sf::Vector3f origin;
    sf::Vector3f normal;
    bool isFrontFacingTo(const sf::Vector3f& direction) const;
    double signedDistanceTo(const sf::Vector3f& point) const;
};

Plane::Plane(const sf::Vector3f& originGiven, const sf::Vector3f& normalGiven)
{
    normal = normalGiven;
    origin = originGiven;
    equation[0] = normal.x;
    equation[1] = normal.y;
    equation[2] = normal.z;
    equation[3] = -(normal.x*origin.x+normal.y*origin.y+normal.z*origin.z);
}
/// Construct from triangle:
Plane::Plane(const sf::Vector3f& p1,const sf::Vector3f& p2, const sf::Vector3f& p3)
{
    normal = crossProduct((p2-p1), (p3-p1));
    normalize3f(normal);
    origin = p1;
    equation[0] = normal.x;
    equation[1] = normal.y;
    equation[2] = normal.z;
    equation[3] = -(normal.x*origin.x+normal.y*origin.y+normal.z*origin.z);
}
bool Plane::isFrontFacingTo(const sf::Vector3f& direction) const
{
    double dot = dotProduct(normal, direction);
    return (dot <= 0);
}
double Plane::signedDistanceTo(const sf::Vector3f& point) const
{
    return dotProduct(point, normal) + equation[3];
}

class SYNTH3D;

class ObjectUtility
{
public:
    ObjectUtility(SYNTH3D* parent, sf::Vector2u setWindowSize);
    void show();
protected:
    SYNTH3D* p;
    sf::Vector2u windowSize;
    void showRightWindow();
    void showCreationWindow();
    void showLeftWindow();
    void showTopWindow();
    void showTopWindowAlternative();
    void showBottomWindow();
    void showErrorMessage();
    void showWallCreationWindow();
    void moveObject(int deltaX, int deltaY, int deltaZ);
    void calcObjectBoundaries(int& minx, int& maxx, int& miny, int& maxy, int& minz, int& maxz);
    void resizeObject(float sizeX, float sizeY, float sizeZ, sf::Vector3f center);
    void calcStartAndFinish(int& start, int& finish);
    void calcStartAndFinishSafe(int& start, int& finish);
    int calcObjectSize();
    void saveObject();
    sf::Vector3f rotateVector(sf::Vector3f vec, sf::Vector3f angle, sf::Vector3f center);
    void rotateObject(sf::Vector3f angle, sf::Vector3f center);
    void alignToGrid(int gridSize);
    void alignObject();
    void alignAll();
    void representVectorAsThreeOthers(sf::Vector3f destination, sf::Vector3f vec1, sf::Vector3f vec2, sf::Vector3f vec3,
                                      float& a, float& b, float& c);
    void makeBackup();
    void restoreBackup();
    void deleteTerrainPoint(int index);

    unsigned int xButtonInterval;
    unsigned int yButtonInterval;

    unsigned int rightWindowWidth;
    ImVec4 rightActiveButtonColor;
    ImVec4 rightActiveButtonColorSemi;
    ImVec4 rightNewButtonColor;
    ImVec4 rightWindowBackgroundColor;
    sf::Vector2u rightWindowSize;

    unsigned int leftWindowWidth;
    ImVec4 leftActiveButtonColor;
    ImVec4 leftActiveButtonColorSemi;
    ImVec4 leftNewButtonColor;
    ImVec4 leftWindowBackgroundColor;
    sf::Vector2u leftWindowSize;

    unsigned int topWindowHeight;
    ImVec4 topActiveButtonColor;
    ImVec4 topActiveButtonColorSemi;
    ImVec4 topNewButtonColor;
    ImVec4 topWindowBackgroundColor;
    sf::Vector2u topWindowSize;

    unsigned int bottomWindowHeight;
    ImVec4 bottomActiveButtonColor;
    ImVec4 bottomActiveButtonColorSemi;
    ImVec4 bottomNewButtonColor;
    ImVec4 bottomWindowBackgroundColor;
    sf::Vector2u bottomWindowSize;

    sf::Vector2u creationWindowSize;
    sf::Vector2u errorWindowSize;

    int editModeActiveObject;
    bool createObjectModeActive;
    bool createObjectModeJustActivated;
    bool errorMessageActivate;
    bool errorMessageJustActivated;
    std::string errorMessage;

    char* editModeActiveObjectName;
    char filename[100];

    std::vector <sf::Vector3f> object;
    std::vector <Object> worldBackup;
    std::vector <sf::Vector3f> terrainBackup;
    std::vector <sf::Vector3f> defTerrainBackup;
    bool backupMade;

    int selectedCenter;
    float currentAngle;
    int rotationAxis;
    sf::Vector3f personalCenter;
    int gridSize;
    int currentWall;
    int objectGrid;
    float objectColor[4];
    float objectTrans[4];
    float objectPointSize;
    bool createWallModeActive;
    bool createWallModeJustActivated;
    Wall tempWall;
    Object tempObject;
    int creationModeActiveObject;
    std::vector <bool> selectedObject;
    std::vector <bool> activeObject;
};

class Entity;

class Camera
{
public:
    Camera(SYNTH3D* parent);
    void display();                     //Wyswietlanie
    void setPos(sf::Vector3f posGiven); //Ustaw pozycje kamery
    void setAngle(sf::Vector3f aGiven); //Ustaw kat skierowania kamery
    void setEyeDistance(float distance);//Ustaw odleglosc oka od plaszczyzny
    void setDirection(sf::Vector3f direction, sf::Vector3f origin);
    void update();
    void printGraph();
    void printCycle();
    void enableCycleReduction(bool value);
    void enableWallIntersect(bool value);
    void enableRandomSort(bool value);
    void enableIdentifyWalls(bool value);
    void enableDistanceCheck(bool value);
    void compare(int left, int right);
protected:
    std::vector <sf::Vector2f> halfDotBegin;
    std::vector <sf::Vector2f> circleDefExample;
    std::vector <Wall*> wallOrder;
    std::vector <sf::Vertex> quadArray;
    std::vector <sf::Vertex> debugArray;
    std::vector <sf::Text> textArray;
    std::vector <sf::Vector3f> spot3d;
    std::vector <sf::Vector2f> spot2d;
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
    std::pair<bool, bool> wallSortingAlgorythm(Wall* lhs, Wall* rhs);
    std::pair<bool, bool> wallSortingAlgorythmDebug(Wall* lhs, Wall* rhs);
    bool lineIntersect(sf::Vector2f u0, sf::Vector2f v0, sf::Vector2f u1, sf::Vector2f v1);
    bool polygonIntersect(std::vector <sf::Vector2f> poly1, std::vector <sf::Vector2f> poly2);
    bool polygonIntersectOld(std::vector <sf::Vector2f> poly1, std::vector <sf::Vector2f> poly2);
    bool polygonIntersectOlder(std::vector <sf::Vector2f> poly1, std::vector <sf::Vector2f> poly2);
    bool wallIntersect(Wall* wallie1, Wall* wallie2);
    bool wallIntersectOS(Wall* left, Wall* right);
    bool rightSide(sf::Vector2f line1, sf::Vector2f line2, sf::Vector2f point);
    int planeSide(sf::Vector3f& center, sf::Vector3f& top, sf::Vector3f& side, sf::Vector3f& point);
    bool onPlane(sf::Vector3f& center, sf::Vector3f& top, sf::Vector3f& side, sf::Vector3f& point);
    bool activeWall(Wall* wallie);
    float dotSize(sf::Vector3f vec);
    void identifyWalls(std::vector <Wall*> wallie);
    std::string cmdOutput;
    bool cycleReductionEnabled;
    bool wallIntersectEnabled;
    bool randomSortEnabled;
    bool identifyWallsEnabled;
    bool topologicalError;
    bool distanceCheckEnabled;
    double sinceLastTopologicalError;
    void findCycle(std::vector <std::vector <int> >& graph, std::vector <int>& graphLevel);
    std::vector <std::vector <int> > wallSortGraph;
    std::vector <int> wallSortGraphLevel;
    float wallIntersectionOffset;
    bool straightWallCheck;
};

class SYNTH3D: public Scene
{
public:
    virtual bool onConsoleUpdate(std::vector<std::string> args);
    friend class Camera;
    SYNTH3D(std::string _name, SceneManager* mgr, sf::RenderWindow* w);
    virtual void onSceneLoadToMemory();
    void onSceneActivate() {}
    void draw(sf::Time deltaTime);
    void checkCollision(collisionPacket* colPackage, int beginning, int ending);
    std::vector <sf::Vector3f> terrain;
    std::vector <sf::Vector3f> defTerrain;
    std::vector <Object> world;
    std::vector <Entity> character;
    void saveMap(std::string path);
protected:
    Camera c;
    sf::Vector3f cameraPos;
    sf::Vector3f cameraAngle;
    float eyeDistance;
    float mouseSpeed;
    void loadMap(std::string path);
    void loadEntities(std::string path);
    void OptLines();
    void OptDots();
    void showInfo();
    bool debug;
    bool grabbedMouse;
    bool gBuffor;
    float offsetx, offsety;
    int tick;
    double dt;
    sf::Vector3f catVelocity;
    sf::Vector3f catAngle;
    sf::Vector3f characterAcceleration;
    sf::Vector3f gravity;

    void checkTriangle(collisionPacket* colPackage,
        const sf::Vector3f& p1, const sf::Vector3f& p2, const sf::Vector3f& p3);
    bool checkPointInTriangle(const sf::Vector3f& point,
        const sf::Vector3f& pa, const sf::Vector3f& pb, const sf::Vector3f& pc);
    bool getLowestRoot(float a, float b, float c, float maxR, float* root);

    sf::Vector2i getDeltaMouse();
    sf::Vector2i bufforMousePos;

    ObjectUtility editMap;
    bool firstPerson;
};

class Entity
{
public:
    Entity(SYNTH3D* gameBase);
    void update(double deltaTime);
    void importObject(int newElement);
    void setDirection(sf::Vector3f vec, sf::Vector3f beginning);
    void setPos(sf::Vector3f vec);
    void setRotationCenter(sf::Vector3f);
    void setEllipsoidRadius(sf::Vector3f);
    void calcRotationCenter();
    std::vector <std::pair <int, sf::Vector3f> > leftLeg;
    std::vector <std::pair <int, sf::Vector3f> > rightLeg;
    void setAcceleration(sf::Vector3f acceleration);
    void setGravity(sf::Vector3f acceleration);
    void setSpeed(sf::Vector3f vec);
    sf::Vector3f ellipsoidCenter;                             ///TUTAJ ZMIANA
    collisionPacket collisionPackage;
    sf::Vector3f position;
    std::string name;
    void moveLeft(float speed);
    void moveRight(float speed);
    void moveForward(float speed);
    void moveBackward(float speed);
    void jump(float speed);
protected:
    SYNTH3D* synth;
    sf::Vector3f positionBuffor;
    void movement();
    void calcAngle();
    void calcVelocity(sf::Vector3f& velocity);
    sf::Vector3f vecTransform(sf::Vector3f vec);
    std::vector <int> element;
    float sinx, siny, sinz, cosx, cosy, cosz;
    sf::Vector3f origin, direction;
    sf::Vector3f dirVec;
    sf::Vector3f rotationCenter;
    double dt;
    float offset, offsetLimit;
    bool directionSet;
    bool standingOnSurface;
    bool velocitySet;
    int tick;
    sf::Vector3f velocityVector;
    sf::Vector3f gravityVector;
    sf::Vector3f speedVector;
    sf::Vector3f gravitySpeedVector;
    float maxSpeedSquared;
    float maxSpeed;
    float veryCloseDistanceVelocity;
    float veryCloseDistanceGravity;
    int collisionRecursionDepth;
    void collideAndSlide(const sf::Vector3f& vel, const sf::Vector3f& gravity);
    sf::Vector3f collideWithWorld(const sf::Vector3f& pos, const sf::Vector3f& vel, float veryCloseDistance);
};

ObjectUtility::ObjectUtility(SYNTH3D* parent, sf::Vector2u setWindowSize) :
    xButtonInterval(20), yButtonInterval(20),

    rightWindowWidth(180),
    rightActiveButtonColor(sfColorToImColor(sf::Color(255, 255, 0, 255))),
    rightActiveButtonColorSemi(sfColorToImColor(sf::Color(255, 255, 0, 122))),
    rightNewButtonColor(sfColorToImColor(sf::Color(0, 122, 122, 255))),
    rightWindowBackgroundColor(sfColorToImColor(sf::Color(255, 0, 255, 47))),
    rightWindowSize(sf::Vector2u(rightWindowWidth, setWindowSize.y - 2*yButtonInterval)),

    leftWindowWidth(340),
    leftActiveButtonColor(sfColorToImColor(sf::Color(255, 255, 0, 255))),
    leftActiveButtonColorSemi(sfColorToImColor(sf::Color(255, 255, 0, 122))),
    leftNewButtonColor(sfColorToImColor(sf::Color(0, 122, 122, 255))),
    leftWindowBackgroundColor(sfColorToImColor(sf::Color(255, 0, 255, 47))),
    leftWindowSize(sf::Vector2u(leftWindowWidth, setWindowSize.y - 2*yButtonInterval)),

    topWindowHeight(110),
    topActiveButtonColor(sfColorToImColor(sf::Color(255, 255, 0, 255))),
    topActiveButtonColorSemi(sfColorToImColor(sf::Color(255, 255, 0, 122))),
    topNewButtonColor(sfColorToImColor(sf::Color(0, 122, 122, 255))),
    topWindowBackgroundColor(sfColorToImColor(sf::Color(255, 0, 255, 47))),
    topWindowSize(sf::Vector2u(setWindowSize.x - leftWindowWidth - rightWindowWidth - 3*xButtonInterval, topWindowHeight)),

    bottomWindowHeight(80),
    bottomActiveButtonColor(sfColorToImColor(sf::Color(255, 255, 0, 255))),
    bottomActiveButtonColorSemi(sfColorToImColor(sf::Color(255, 255, 0, 122))),
    bottomNewButtonColor(sfColorToImColor(sf::Color(0, 122, 122, 255))),
    bottomWindowBackgroundColor(sfColorToImColor(sf::Color(255, 0, 255, 47))),
    bottomWindowSize(sf::Vector2u(setWindowSize.x - leftWindowWidth - rightWindowWidth - 3*xButtonInterval, bottomWindowHeight)),

    creationWindowSize(sf::Vector2u(400, 200)),
    editModeActiveObject(-1), createObjectModeActive(false), createObjectModeJustActivated(false),

    editModeActiveObjectName("default"),

    errorWindowSize(sf::Vector2u(400, 200)),
    errorMessageActivate(false),
    errorMessageJustActivated(false),
    errorMessage("No Errors"),

    selectedCenter(1),
    currentAngle(0),
    rotationAxis(0),
    personalCenter({0, 0, 0}),
    gridSize(5),
    backupMade(false),
    currentWall(-1),
    objectGrid(0),
    objectColor({0, 1.0f, 0, 1.0f}),
    objectTrans({0, 0.0f, 0, 1.0f}),
    objectPointSize(100.0f),
    createWallModeActive(false),
    createWallModeJustActivated(false),
    creationModeActiveObject(-1)
{
    windowSize = setWindowSize;
    p = parent;
    filename[0] = '\0';
}

void ObjectUtility::show()
{
    createObjectModeJustActivated = false;
    showRightWindow();
    showBottomWindow();
    bool alternativeTopWindow = false;
    if(createObjectModeActive)
    {
        if(createObjectModeJustActivated)
        {
            createObjectModeJustActivated = false;
            ImGui::SetNextWindowPos(sf::Vector2u((p->window->getSize().x - creationWindowSize.x)/2, (p->window->getSize().y - creationWindowSize.y)/2));
            ImGui::SetNextWindowSize(creationWindowSize);
            tempObject.clear();
        }
        showCreationWindow();
    }
    if(activeObject.size() > 1)
    {
        alternativeTopWindow = true;
        showTopWindowAlternative();
    }
    if(editModeActiveObject >=0 and editModeActiveObject < p->world.size())
    {
        showLeftWindow();
        if(calcObjectSize() >= 0 and !alternativeTopWindow)
            showTopWindow();
    }
    if(createWallModeActive)
    {
        showWallCreationWindow();
    }
    if(errorMessageActivate)
    {
        if(errorMessageJustActivated)
        {
            errorMessageJustActivated = false;
            ImGui::SetNextWindowPos(sf::Vector2u((p->window->getSize().x - errorWindowSize.x)/2, (p->window->getSize().y - errorWindowSize.y)/2));
            ImGui::SetNextWindowSize(errorWindowSize);
        }
        showErrorMessage();
    }
}

void ObjectUtility::showRightWindow()
{
    ImGui::SetNextWindowPos(sf::Vector2u(p->window->getSize().x - rightWindowSize.x - xButtonInterval,yButtonInterval));
    ImGui::SetNextWindowSize(rightWindowSize);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, rightWindowBackgroundColor);
    ImGui::Begin("Object Selection");
    if(ImGui::Button("Align All", sf::Vector2u(rightWindowWidth - 20,20)))
    {
        alignAll();
    }
    ImGui::PushStyleColor(ImGuiCol_Button, rightNewButtonColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, rightActiveButtonColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, rightActiveButtonColorSemi);
    if(ImGui::Button("Create New", sf::Vector2u(rightWindowWidth - 20,20)))
    //if(ImGui::Button("Create", sf::Vector2u(rightWindowWidth/2 - 14,20)))
    {
        createObjectModeActive = true;
        createObjectModeJustActivated = true;
        editModeActiveObject = -1;
    }
    ImGui::PopStyleColor();
    ImGui::PushStyleColor(ImGuiCol_Button, sfColorToImColor(sf::Color(122,0,0)));
    //if(ImGui::Button("Delete", sf::Vector2u(rightWindowWidth/2 - 14,20)))
    if(ImGui::Button("Delete Selected", sf::Vector2u(rightWindowWidth - 20,20)))
    {
        if(editModeActiveObject >=0 and editModeActiveObject < p->world.size())
        {
            p->world.erase(p->world.begin() + editModeActiveObject);
        }
        editModeActiveObject = -1;
        currentAngle = 0;
        currentWall = -1;
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::Separator();
    for(int i=0; i<p->world.size(); i++)
    {
        bool popStyle = false;
        if(i == editModeActiveObject)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, rightActiveButtonColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, rightActiveButtonColor);
            popStyle = true;
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, rightActiveButtonColor);
        }
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, rightActiveButtonColorSemi);
        if(ImGui::Button(p->world[i].name.c_str(), sf::Vector2u(rightWindowWidth - 20,20)))
        {
            editModeActiveObject = i;
            currentAngle = 0;
            currentWall = -1;
        }
        if(popStyle)
        {
            ImGui::PopStyleColor();
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
    ImGui::Separator();
    if(ImGui::TreeNode("Move objects"))
    {
        selectedObject.resize(p->world.size());
        activeObject.clear();
        for(int i=0; i<p->world.size(); i++)
        {
            bool tempBool = selectedObject[i];
            ImGui::Selectable(p->world[i].name.c_str(), &tempBool);
            selectedObject[i] = tempBool;
            if(selectedObject[i])
            {
                activeObject.push_back(i);
            }
        }
        ImGui::TreePop();
    }
    ImGui::End();
    ImGui::PopStyleColor();
}

void ObjectUtility::showCreationWindow()
{
    ImGui::Begin("Creation Menu");
    char name[50];
    strcpy(name, tempObject.name.c_str());
    ImGui::InputText("Object name", name, 50);
    tempObject.name = name;
    if(ImGui::Button("Create Blank Object", sf::Vector2u(creationWindowSize.x - 15,20)))
    {
        if(tempObject.name.size() > 0)
        {
            p->world.push_back(tempObject);
            tempObject.clear();
        }
        else
        {
            errorMessage = "Object has got to have a name!\n(It's no woman, treat it with respect hehe)";
            errorMessageActivate = true;
            errorMessageJustActivated = true;
        }
    }
    if(ImGui::Button("Create Cube", sf::Vector2u(creationWindowSize.x - 15,20)))
    {
        if(tempObject.name.size() > 0)
        {
            Wall wall[6];
            wall[0].push_back(p->terrain.size() + 0);
            wall[0].push_back(p->terrain.size() + 1);
            wall[0].push_back(p->terrain.size() + 2);
            wall[0].push_back(p->terrain.size() + 3);
            wall[0].name = "Front Wall";

            wall[1].push_back(p->terrain.size() + 4);
            wall[1].push_back(p->terrain.size() + 5);
            wall[1].push_back(p->terrain.size() + 1);
            wall[1].push_back(p->terrain.size() + 0);
            wall[1].name = "Left Wall";

            wall[2].push_back(p->terrain.size() + 1);
            wall[2].push_back(p->terrain.size() + 5);
            wall[2].push_back(p->terrain.size() + 6);
            wall[2].push_back(p->terrain.size() + 2);
            wall[2].name = "Up Wall";

            wall[3].push_back(p->terrain.size() + 3);
            wall[3].push_back(p->terrain.size() + 2);
            wall[3].push_back(p->terrain.size() + 6);
            wall[3].push_back(p->terrain.size() + 7);
            wall[3].name = "Right Wall";

            wall[4].push_back(p->terrain.size() + 4);
            wall[4].push_back(p->terrain.size() + 0);
            wall[4].push_back(p->terrain.size() + 3);
            wall[4].push_back(p->terrain.size() + 7);
            wall[4].name = "Down Wall";

            wall[5].push_back(p->terrain.size() + 7);
            wall[5].push_back(p->terrain.size() + 6);
            wall[5].push_back(p->terrain.size() + 5);
            wall[5].push_back(p->terrain.size() + 4);
            wall[5].name = "Back Wall";
            tempObject.beginning = p->terrain.size();
            tempObject.finish = p->terrain.size() + 7;
            p->terrain.push_back(sf::Vector3f(0, 0, 0));
            p->terrain.push_back(sf::Vector3f(0, 100, 0));
            p->terrain.push_back(sf::Vector3f(100, 100, 0));
            p->terrain.push_back(sf::Vector3f(100, 0, 0));
            p->terrain.push_back(sf::Vector3f(0, 0, 100));
            p->terrain.push_back(sf::Vector3f(0, 100, 100));
            p->terrain.push_back(sf::Vector3f(100, 100, 100));
            p->terrain.push_back(sf::Vector3f(100, 0, 100));
            for(int i=0; i<6; i++)
                tempObject.push_back(wall[i]);
            p->world.push_back(tempObject);
            tempObject.clear();
        }
        else
        {
            errorMessage = "Object has got to have a name!\n(It's no woman, treat it with respect hehe)";
            errorMessageActivate = true;
            errorMessageJustActivated = true;
        }
    }
    std::string nameList;
    for(int i=0; i<p->world.size(); i++)
    {
        nameList += p->world[i].name + '\0';
    }
    nameList += '\0';
    ImGui::Combo(":", &creationModeActiveObject, nameList.c_str());
    ImGui::SameLine();
    if(ImGui::Button("Copy", ImVec2(creationWindowSize.x - 292, 20)))
    {
        if(creationModeActiveObject >=0 and creationModeActiveObject < p->world.size())
        {
            for(int i=0; i<p->world[creationModeActiveObject].size(); i++)
            {
                tempObject.push_back(p->world[creationModeActiveObject].wallie[i]);
            }
            for(int i=0; i<tempObject.size(); i++)
            {
                for(int j=0; j<tempObject.wallie[i].size(); j++)
                {
                    tempObject.wallie[i].coord[j] += p->terrain.size() - p->world[creationModeActiveObject].beginning;
                }
                for(int j=0; j<tempObject.wallie[i].lineStrip.size(); j++)
                    for(int k=0; k<tempObject.wallie[i].lineStrip[j].size(); k++)
                    {
                        tempObject.wallie[i].lineStrip[j][k] += p->terrain.size() - p->world[creationModeActiveObject].beginning;
                    }
            }
            tempObject.beginning = p->terrain.size();
            tempObject.finish = p->world[creationModeActiveObject].finish - p->world[creationModeActiveObject].beginning + p->terrain.size();
            for(int i=p->world[creationModeActiveObject].beginning; i<=p->world[creationModeActiveObject].finish; i++)
            {
                p->terrain.push_back(p->terrain[i]);
                p->defTerrain.push_back(p->defTerrain[i]);
            }
            p->world.push_back(tempObject);
            tempObject.clear();
        }
    }
    if(ImGui::Button("Close", sf::Vector2u(creationWindowSize.x - 15,20)))
    {
        createObjectModeActive = false;
        tempObject.clear();
    }
    ImGui::Separator();
    ImGui::End();
}

void ObjectUtility::showLeftWindow()
{
    ImGui::SetNextWindowPos(sf::Vector2u(xButtonInterval, yButtonInterval));
    ImGui::SetNextWindowSize(leftWindowSize);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, leftWindowBackgroundColor);
    ImGui::Begin("Object Menu");
    char name[100];
    strcpy(name, p->world[editModeActiveObject].name.c_str());
    ImGui::InputText("Edit name", name, 100);
    p->world[editModeActiveObject].name = name;
    std::string objectSize = "Size: " + stringify(p->world[editModeActiveObject].size());
    ImGui::Text(objectSize.c_str());
    if(p->world[editModeActiveObject].OptAllowed)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, sfColorToImColor(sf::Color(0, 122, 0)));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, leftActiveButtonColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, leftActiveButtonColorSemi);
        if(ImGui::Button("Optimization On",sf::Vector2u(leftWindowWidth - 10,20)))
        {
            p->world[editModeActiveObject].OptAllowed = !p->world[editModeActiveObject].OptAllowed;
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Button, sfColorToImColor(sf::Color(122, 0, 0)));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, leftActiveButtonColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, leftActiveButtonColorSemi);
        if(ImGui::Button("Optimization Off",sf::Vector2u(leftWindowWidth - 10,20)))
        {
            p->world[editModeActiveObject].OptAllowed = !p->world[editModeActiveObject].OptAllowed;
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
    int a;
    ImGui::Combo("Pick center", &selectedCenter, "lowest XYZ\0object center\0my own\0\0");
    if(selectedCenter == 2)
    {
        int center[3];
        center[0] = personalCenter.x;
        center[1] = personalCenter.y;
        center[2] = personalCenter.z;
        ImGui::DragInt3("Edit center", center, 0.05);
        personalCenter = sf::Vector3f(center[0], center[1], center[2]);
    }
    ImGui::Text("Rotation:");
    if(ImGui::RadioButton("X axis", rotationAxis == 0))
    {
        currentAngle = 0;
        rotationAxis = 0;
    }
    ImGui::SameLine();
    if(ImGui::RadioButton("Y axis", rotationAxis == 1))
    {
        currentAngle = 0;
        rotationAxis = 1;
    }
    ImGui::SameLine();
    if(ImGui::RadioButton("Z axis", rotationAxis == 2))
    {
        currentAngle = 0;
        rotationAxis = 2;
    }
    if(ImGui::Button("Align to a grid of", ImVec2(200,20)))
    {
        alignToGrid(gridSize);
    }
    ImGui::SameLine();
    ImGui::InputInt("", &gridSize);
    if(ImGui::Button("Align Object", ImVec2(200,20)))
    {
        alignObject();
    }
    if(ImGui::Button("Set walls grid to", ImVec2(200,20)))
    {
        for(int i=0; i<p->world[editModeActiveObject].wallie.size(); i++)
        {
            p->world[editModeActiveObject].wallie[i].grid = objectGrid;
        }
    }
    ImGui::SameLine();
    ImGui::InputInt("", &objectGrid);
    if(ImGui::Button("Set Color", ImVec2(70,20)))
    {
        for(int i=0; i<p->world[editModeActiveObject].wallie.size(); i++)
        {
            p->world[editModeActiveObject].wallie[i].color.r = objectColor[0] * 255.0f;
            p->world[editModeActiveObject].wallie[i].color.g = objectColor[1] * 255.0f;
            p->world[editModeActiveObject].wallie[i].color.b = objectColor[2] * 255.0f;
            p->world[editModeActiveObject].wallie[i].color.a = objectColor[3] * 255.0f;
        }
    }
    ImGui::SameLine();
    ImGui::ColorEdit4("C", objectColor);
    if(ImGui::Button("Set Trans", ImVec2(70,20)))
    {
        for(int i=0; i<p->world[editModeActiveObject].wallie.size(); i++)
        {
            p->world[editModeActiveObject].wallie[i].trans.r = objectTrans[0] * 255.0f;
            p->world[editModeActiveObject].wallie[i].trans.g = objectTrans[1] * 255.0f;
            p->world[editModeActiveObject].wallie[i].trans.b = objectTrans[2] * 255.0f;
            p->world[editModeActiveObject].wallie[i].trans.a = objectTrans[3] * 255.0f;
        }
    }
    ImGui::SameLine();
    ImGui::ColorEdit4("T", objectTrans);
    if(ImGui::Button("Invert walls", ImVec2(leftWindowWidth - 10,20)))
    {
        for(int i=0; i<p->world[editModeActiveObject].wallie.size(); i++)
        {
            std::vector <int> invertedWall;
            std::vector <bool> invertedDrawable;
            std::vector <bool> invertedDotDraw;
            for(int j=p->world[editModeActiveObject].wallie[i].size()-1; j>=0; j--)
            {
                invertedWall.push_back(p->world[editModeActiveObject].wallie[i].coord[j]);
            }
            for(int j=p->world[editModeActiveObject].wallie[i].size()-2; j>=0; j--)
            {
                invertedDrawable.push_back(p->world[editModeActiveObject].wallie[i].drawable[j]);
                invertedDotDraw.push_back(p->world[editModeActiveObject].wallie[i].dotDraw[j]);
            }
            invertedDrawable.push_back(p->world[editModeActiveObject].wallie[i].drawable[p->world[editModeActiveObject].wallie[i].size()-1]);
            invertedDotDraw.push_back(p->world[editModeActiveObject].wallie[i].dotDraw[p->world[editModeActiveObject].wallie[i].size()-1]);
            p->world[editModeActiveObject].wallie[i].coord = invertedWall;
            p->world[editModeActiveObject].wallie[i].drawable = invertedDrawable;
            p->world[editModeActiveObject].wallie[i].dotDraw = invertedDotDraw;
        }
    }
    if(ImGui::Button("Set point size", ImVec2(110,20)))
    {
        for(int i=0; i<p->world[editModeActiveObject].wallie.size(); i++)
        {
            p->world[editModeActiveObject].wallie[i].pSize = objectPointSize;
        }
    }
    ImGui::SameLine();
    ImGui::DragFloat("c", &objectPointSize, 1.0f, 1.0f, 1000.0f);
    ImGui::Text("");
    ImGui::TextColored(ImVec4(255, 255, 0, 255), "Walls:");
    ImGui::Separator();
    std::string nameList;
    for(int i=0; i<p->world[editModeActiveObject].size(); i++)
    {
        std::string s;
        if(p->world[editModeActiveObject].wallie[i].name.size() > 0)
            s = p->world[editModeActiveObject].wallie[i].name;
        else
            s = "Wall " + stringify(i);
        nameList += s + '\0';
    }
    nameList += "none\0";
    nameList += '\0';
    ImGui::Combo("Pick wall", &currentWall, nameList.c_str());
    if(currentWall == p->world[editModeActiveObject].size())
        currentWall = -1;
    if(currentWall >=0 and currentWall < p->world[editModeActiveObject].size())
    {
        std::string sizeOfWall;
        sizeOfWall = "Size: " + stringify(calcObjectSize());
        ImGui::Text(sizeOfWall.c_str());
        Wall& wallie = p->world[editModeActiveObject].wallie[currentWall];
        char wallName[32];
        strcpy(wallName, wallie.name.c_str());
        ImGui::InputText("Change name", wallName, 32);
        wallie.name = wallName;
        float col4[4];
        col4[0] = wallie.color.r / 255.0f; col4[1] = wallie.color.g / 255.0f;
        col4[2] = wallie.color.b / 255.0f; col4[3] = wallie.color.a / 255.0f;
        ImGui::ColorEdit4("color", col4);
        wallie.color.r = col4[0] * 255.0f; wallie.color.g = col4[1] * 255.0f;
        wallie.color.b = col4[2] * 255.0f; wallie.color.a = col4[3] * 255.0f;

        col4[0] = wallie.trans.r / 255.0f; col4[1] = wallie.trans.g / 255.0f;
        col4[2] = wallie.trans.b / 255.0f; col4[3] = wallie.trans.a / 255.0f;
        ImGui::ColorEdit4("trans", col4);
        wallie.trans.r = col4[0] * 255.0f; wallie.trans.g = col4[1] * 255.0f;
        wallie.trans.b = col4[2] * 255.0f; wallie.trans.a = col4[3] * 255.0f;

        ImGui::DragFloat("Point size", &wallie.pSize, 1.0f, 1.0f, 1000.0f);

        if(ImGui::TreeNode("Line Strip"))
        {
            int start;
            int finish;
            calcStartAndFinishSafe(start, finish);
            int objectCount;
            objectCount = calcObjectSize();

            ImGui::PushStyleColor(ImGuiCol_Button, leftNewButtonColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, leftActiveButtonColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, leftActiveButtonColorSemi);
            if(ImGui::Button("Create", sf::Vector2u(leftWindowWidth/2 - 50,20)))
            {
                std::vector <int> vec;
                vec.push_back(start);
                wallie.lineStrip.push_back(vec);
            }
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, sfColorToImColor(sf::Color(122,0,0)));
            if(ImGui::Button("Delete", sf::Vector2u(leftWindowWidth/2 - 50,20)))
            {
                if(!wallie.lineStrip.empty())
                    wallie.lineStrip.erase(wallie.lineStrip.begin() + wallie.lineStrip.size() - 1);
            }
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();

            for(int i=0; i<wallie.lineStrip.size(); i++)
            {
                std::string s = "strip " + stringify(i);
                if(ImGui::TreeNode(s.c_str()))
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, leftNewButtonColor);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, leftActiveButtonColor);
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, leftActiveButtonColorSemi);
                    if(ImGui::Button("Create", sf::Vector2u(leftWindowWidth/2 - 50,20)))
                    {
                        if(!wallie.lineStrip[i].empty())
                        {
                            if(wallie.lineStrip[i][wallie.lineStrip[i].size() - 1] < finish)
                            {
                                wallie.lineStrip[i].push_back(wallie.lineStrip[i][wallie.lineStrip[i].size() - 1] + 1);
                            }
                            else
                            {
                                wallie.lineStrip[i].push_back(wallie.lineStrip[i][wallie.lineStrip[i].size() - 1]);
                            }
                        }
                        else
                        {
                            wallie.lineStrip[i].push_back(start);
                        }
                    }
                    ImGui::PopStyleColor();
                    ImGui::SameLine();
                    ImGui::PushStyleColor(ImGuiCol_Button, sfColorToImColor(sf::Color(122,0,0)));
                    if(ImGui::Button("Delete", sf::Vector2u(leftWindowWidth/2 - 50,20)))
                    {
                        if(wallie.lineStrip[i].size() > 1)
                            wallie.lineStrip[i].erase(wallie.lineStrip[i].begin() + wallie.lineStrip[i].size() - 1);
                    }
                    ImGui::PopStyleColor();
                    ImGui::PopStyleColor();
                    ImGui::PopStyleColor();

                    for(int j=0; j<wallie.lineStrip[i].size(); j++)
                    {
                        int falseCoord = wallie.lineStrip[i][j] - start;
                        ImGui::SliderInt(stringify(j).c_str(), &falseCoord, 0, objectCount - 1);
                        wallie.lineStrip[i][j] = falseCoord + start;
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }

        if(ImGui::TreeNode("Coord"))
        {
            int start;
            int finish;
            calcStartAndFinishSafe(start, finish);
            int objectCount;
            objectCount = calcObjectSize();

            ImGui::PushStyleColor(ImGuiCol_Button, leftNewButtonColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, leftActiveButtonColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, leftActiveButtonColorSemi);
            if(ImGui::Button("Create", sf::Vector2u(leftWindowWidth/2 - 50,20)))
            {
                if(!wallie.coord.empty())
                {
                    if(wallie.coord[wallie.size()-1] == finish)
                    {
                        wallie.push_back(finish);
                    }
                    else
                    {
                        wallie.push_back(wallie.coord[wallie.size()-1] + 1);
                    }
                }
                else
                {
                    wallie.push_back(start);
                }
            }
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, sfColorToImColor(sf::Color(122,0,0)));
            if(ImGui::Button("Delete", sf::Vector2u(leftWindowWidth/2 - 50,20)))
            {
                if(!wallie.coord.empty())
                    wallie.coord.erase(wallie.coord.begin() + wallie.size() - 1);
            }
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();

            for(int i=0; i<wallie.size(); i++)
            {
                int falseCoord = wallie.coord[i] - start;
                ImGui::SliderInt(stringify(i).c_str(), &falseCoord, 0, objectCount - 1);
                wallie.coord[i] = falseCoord + start;
            }
            ImGui::TreePop();
        }

        if(ImGui::TreeNode("Draw mask"))
        {
            int density = 4;
            ImGui::Columns(density, NULL, false);
            for(int i=0; i<wallie.size(); i++)
            {
                std::string tempButtonName;
                tempButtonName = "L" + stringify(i) + ": " + stringify(int(wallie.drawable[i]));
                if(ImGui::SmallButton(tempButtonName.c_str()))
                {
                    wallie.drawable[i] = !wallie.drawable[i];
                }
                tempButtonName = "D" + stringify(i) + ": " + stringify(int(wallie.dotDraw[i]));
                if(ImGui::SmallButton(tempButtonName.c_str()))
                {
                    wallie.dotDraw[i] = !wallie.dotDraw[i];
                }
                ImGui::NextColumn();
                if(i%density == 7)
                {
                    ImGui::Columns(1);
                    ImGui::Separator();
                    ImGui::Columns(density, NULL, false);
                }
            }
            ImGui::Columns(1);
            ImGui::TreePop();
        }

        if(ImGui::TreeNode("Points"))
        {
            ImGui::Columns(4, NULL, false);
            int lineBeginning = 0;
            int i = editModeActiveObject;
            for(int j=0; j<p->world[i].size(); j++)
            {
                for(int k=0; k < p->world[i].wallie[j].size(); k++)
                {
                    if(p->world[i].wallie[j].coord[k] > lineBeginning)
                        lineBeginning = p->world[i].wallie[j].coord[k];
                }
            }
            int start, finish;
            calcStartAndFinishSafe(start, finish);
            ImGui::Selectable("X", false);
            ImGui::NextColumn();
            ImGui::Selectable("Y", false);
            ImGui::NextColumn();
            ImGui::Selectable("Z", false);
            ImGui::NextColumn();
            ImGui::Selectable("ID", false);
            ImGui::NextColumn();
            for(int i=start; i<=finish; i++)
            {
                bool activePoint = false;
                for(int j=0; j<wallie.coord.size(); j++)
                {
                    if(wallie.coord[j] == i)
                    {
                        activePoint = true;
                        break;
                    }
                }
                ImGui::Selectable(stringifyf(p->terrain[i].x).c_str(), activePoint);
                ImGui::NextColumn();
                ImGui::Selectable(stringifyf(p->terrain[i].y).c_str(), activePoint);
                ImGui::NextColumn();
                ImGui::Selectable(stringifyf(p->terrain[i].z).c_str(), activePoint);
                ImGui::NextColumn();
                ImGui::Selectable(stringify(i - start).c_str(), activePoint);
                ImGui::NextColumn();
            }
            ImGui::Columns(1);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Edit Points"))
        {
            int lineBeginning = 0;
            int i = editModeActiveObject;
            for(int j=0; j<p->world[i].size(); j++)
            {
                for(int k=0; k < p->world[i].wallie[j].size(); k++)
                {
                    if(p->world[i].wallie[j].coord[k] > lineBeginning)
                        lineBeginning = p->world[i].wallie[j].coord[k];
                }
            }
            int start, finish;
            calcStartAndFinishSafe(start, finish);
            for(int i=start; i<=finish; i++)
            {
                std::string s = "nr " + stringify(i - start);
                float singlePoint[3];
                singlePoint[0] = p->terrain[i].x;
                singlePoint[1] = p->terrain[i].y;
                singlePoint[2] = p->terrain[i].z;
                ImGui::DragFloat3(s.c_str(), singlePoint, 0.02f);
                p->terrain[i].x = singlePoint[0];
                p->terrain[i].y = singlePoint[1];
                p->terrain[i].z = singlePoint[2];
            }
            if(start <= finish and ImGui::Button("Copy Last", ImVec2(100, 20)))
            {
                start = p->world[editModeActiveObject].beginning;
                finish = p->world[editModeActiveObject].finish;
                sf::Vector3f desiredPoint = p->terrain[finish];
                if(start > finish)
                {
                    if(editModeActiveObject > 0)
                    {
                        start = p->world[editModeActiveObject - 1].finish + 1;
                    }
                    else
                    {
                        start = 0;
                    }
                    finish = start - 1;
                    p->world[editModeActiveObject].beginning = start;
                    p->world[editModeActiveObject].finish = finish;
                }
                p->terrain.insert(p->terrain.begin() + finish + 1, desiredPoint);
                p->defTerrain.insert(p->defTerrain.begin() + finish + 1, desiredPoint);
                p->world[editModeActiveObject].finish++;
                for(int i=editModeActiveObject + 1; i<p->world.size(); i++)
                {
                    p->world[i].beginning++;
                    p->world[i].finish++;
                    for(int j=0; j<p->world[i].size(); j++)
                    {
                        for(int m=0; m<p->world[i].wallie[j].size(); m++)
                        {
                            p->world[i].wallie[j].coord[m]++;
                        }
                        for(int m=0; m<p->world[i].wallie[j].lineStrip.size(); m++)
                        {
                            for(int n=0; n<p->world[i].wallie[j].lineStrip[m].size(); n++)
                            {
                                p->world[i].wallie[j].lineStrip[m][n]++;
                            }
                        }
                    }
                }
            }
            ImGui::TreePop();
        }
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, leftActiveButtonColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, leftActiveButtonColorSemi);
        ImGui::PushStyleColor(ImGuiCol_Button, sfColorToImColor(sf::Color(122,0,0)));
        if(ImGui::Button("Delete Selected Wall", sf::Vector2u(leftWindowWidth - 10,20)))
        {
            if(currentWall >=0 and currentWall < p->world[editModeActiveObject].wallie.size())
            {
                p->world[editModeActiveObject].wallie.erase(p->world[editModeActiveObject].wallie.begin() + currentWall);
                currentWall = -1;
            }
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Button, leftNewButtonColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, leftActiveButtonColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, leftActiveButtonColorSemi);
        if(ImGui::Button("Create New Wall", sf::Vector2u(leftWindowWidth - 10,20)))
        {
            createWallModeJustActivated = true;
            createWallModeActive = true;
        }
        if(ImGui::Button("Create New Point", sf::Vector2u(leftWindowWidth - 10,20)))
        {
            int start = p->world[editModeActiveObject].beginning;
            int finish = p->world[editModeActiveObject].finish;
            if(start > finish)
            {
                if(editModeActiveObject > 0)
                {
                    start = p->world[editModeActiveObject - 1].finish + 1;
                }
                else
                {
                    start = 0;
                }
                finish = start - 1;
                p->world[editModeActiveObject].beginning = start;
                p->world[editModeActiveObject].finish = finish;
            }
            p->terrain.insert(p->terrain.begin() + finish + 1, sf::Vector3f(0, 0, 0));
            p->defTerrain.insert(p->defTerrain.begin() + finish + 1, sf::Vector3f(0, 0, 0));
            p->world[editModeActiveObject].finish++;
            for(int i=editModeActiveObject + 1; i<p->world.size(); i++)
            {
                p->world[i].beginning++;
                p->world[i].finish++;
                for(int j=0; j<p->world[i].size(); j++)
                {
                    for(int m=0; m<p->world[i].wallie[j].size(); m++)
                    {
                        p->world[i].wallie[j].coord[m]++;
                    }
                    for(int m=0; m<p->world[i].wallie[j].lineStrip.size(); m++)
                    {
                        for(int n=0; n<p->world[i].wallie[j].lineStrip[m].size(); n++)
                        {
                            p->world[i].wallie[j].lineStrip[m][n]++;
                        }
                    }
                }
            }
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, leftActiveButtonColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, leftActiveButtonColorSemi);
        ImGui::PushStyleColor(ImGuiCol_Button, sfColorToImColor(sf::Color(122,0,0)));
        if(ImGui::Button("Delete Last Point", sf::Vector2u(leftWindowWidth - 10,20)))
        {
            if((p->world[editModeActiveObject].finish - p->world[editModeActiveObject].beginning) >= 1)
                deleteTerrainPoint(p->world[editModeActiveObject].finish);
            else
            {
                errorMessage = "Please do not delete the last one!\n(You can always add one / modify)";
                errorMessageActivate = true;
                errorMessageJustActivated = true;
            }
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        if (ImGui::TreeNode("Edit Points"))
        {
            int lineBeginning = 0;
            int i = editModeActiveObject;
            for(int j=0; j<p->world[i].size(); j++)
            {
                for(int k=0; k < p->world[i].wallie[j].size(); k++)
                {
                    if(p->world[i].wallie[j].coord[k] > lineBeginning)
                        lineBeginning = p->world[i].wallie[j].coord[k];
                }
            }
            int start, finish;
            calcStartAndFinishSafe(start, finish);
            for(int i=start; i<=finish; i++)
            {
                std::string s = "nr " + stringify(i - start);
                float singlePoint[3];
                singlePoint[0] = p->terrain[i].x;
                singlePoint[1] = p->terrain[i].y;
                singlePoint[2] = p->terrain[i].z;
                ImGui::InputFloat3(s.c_str(), singlePoint, 0);
                p->terrain[i].x = singlePoint[0];
                p->terrain[i].y = singlePoint[1];
                p->terrain[i].z = singlePoint[2];
            }
            if(start <= finish and ImGui::Button("Copy Last", ImVec2(100, 20)))
            {
                start = p->world[editModeActiveObject].beginning;
                finish = p->world[editModeActiveObject].finish;
                sf::Vector3f desiredPoint = p->terrain[finish];
                if(start > finish)
                {
                    if(editModeActiveObject > 0)
                    {
                        start = p->world[editModeActiveObject - 1].finish + 1;
                    }
                    else
                    {
                        start = 0;
                    }
                    finish = start - 1;
                    p->world[editModeActiveObject].beginning = start;
                    p->world[editModeActiveObject].finish = finish;
                }
                p->terrain.insert(p->terrain.begin() + finish + 1, desiredPoint);
                p->defTerrain.insert(p->defTerrain.begin() + finish + 1, desiredPoint);
                p->world[editModeActiveObject].finish++;
                for(int i=editModeActiveObject + 1; i<p->world.size(); i++)
                {
                    p->world[i].beginning++;
                    p->world[i].finish++;
                    for(int j=0; j<p->world[i].size(); j++)
                    {
                        for(int m=0; m<p->world[i].wallie[j].size(); m++)
                        {
                            p->world[i].wallie[j].coord[m]++;
                        }
                        for(int m=0; m<p->world[i].wallie[j].lineStrip.size(); m++)
                        {
                            for(int n=0; n<p->world[i].wallie[j].lineStrip[m].size(); n++)
                            {
                                p->world[i].wallie[j].lineStrip[m][n]++;
                            }
                        }
                    }
                }
            }
            ImGui::TreePop();
        }
        if(ImGui::TreeNode("Delete Points (experimental)"))
        {
            ImGui::Columns(4, NULL, false);
            int lineBeginning = 0;
            int i = editModeActiveObject;
            for(int j=0; j<p->world[i].size(); j++)
            {
                for(int k=0; k < p->world[i].wallie[j].size(); k++)
                {
                    if(p->world[i].wallie[j].coord[k] > lineBeginning)
                        lineBeginning = p->world[i].wallie[j].coord[k];
                }
            }
            int start, finish;
            calcStartAndFinishSafe(start, finish);
            ImGui::Selectable("X", false);
            ImGui::NextColumn();
            ImGui::Selectable("Y", false);
            ImGui::NextColumn();
            ImGui::Selectable("Z", false);
            ImGui::NextColumn();
            ImGui::Selectable("ID", false);
            ImGui::NextColumn();
            for(int i=start; i<=finish; i++)
            {
                bool activePoint = false;
                ImGui::Selectable(stringifyf(p->terrain[i].x).c_str(), activePoint);
                ImGui::NextColumn();
                ImGui::Selectable(stringifyf(p->terrain[i].y).c_str(), activePoint);
                ImGui::NextColumn();
                ImGui::Selectable(stringifyf(p->terrain[i].z).c_str(), activePoint);
                ImGui::NextColumn();

                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, leftActiveButtonColor);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, leftActiveButtonColorSemi);
                ImGui::PushStyleColor(ImGuiCol_Button, sfColorToImColor(sf::Color(122,0,0)));
                if(ImGui::Button(stringify(i - start).c_str(), ImVec2(60, 20)))
                {
                    if((p->world[editModeActiveObject].finish - p->world[editModeActiveObject].beginning) >= 1)
                        deleteTerrainPoint(i);
                    else
                    {
                        errorMessage = "Please do not delete the last one!\n(You can always add one / modify)";
                        errorMessageActivate = true;
                        errorMessageJustActivated = true;
                    }
                }
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::NextColumn();
            }
            ImGui::Columns(1);
            ImGui::TreePop();
        }
    }
    ImGui::End();
    ImGui::PopStyleColor();
}

void ObjectUtility::showTopWindowAlternative()
{
    ImGui::SetNextWindowPos(sf::Vector2u(1.5*xButtonInterval + leftWindowWidth,yButtonInterval));
    ImGui::SetNextWindowSize(topWindowSize);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, topWindowBackgroundColor);
    ImGui::Begin("Edit Object");

    int position[3];
    int minx, maxx, miny, maxy, minz, maxz;
    int bufforObject = editModeActiveObject;
    editModeActiveObject = 0;
    calcObjectBoundaries(minx, maxx, miny, maxy, minz, maxz);
    for(int i=1; i<activeObject.size(); i++)
    {
        int tempMinx, tempMaxx, tempMiny, tempMaxy, tempMinz, tempMaxz;
        editModeActiveObject = activeObject[i];
        calcObjectBoundaries(tempMinx, tempMaxx, tempMiny, tempMaxy, tempMinz, tempMaxz);
        minx = std::min(tempMinx, minx);
        maxx = std::max(tempMaxx, maxx);
        miny = std::min(tempMiny, miny);
        maxy = std::max(tempMaxy, maxy);
        minz = std::min(tempMinz, minz);
        maxz = std::max(tempMaxz, maxz);
    }

    editModeActiveObject = bufforObject;
    sf::Vector3f center;
    if(selectedCenter == 1)
    {
        center.x = minx + (maxx - minx)*0.5f;
        center.y = miny + (maxy - miny)*0.5f;
        center.z = minz + (maxz - minz)*0.5f;
    }
    else if(selectedCenter == 2)
    {
        center = personalCenter;
    }
    else
    {
        center.x = minx;
        center.y = miny;
        center.z = minz;
    }

    position[0] = center.x;
    position[1] = center.y;
    position[2] = center.z;
    if(ImGui::DragInt3("Edit Position XYZ", position, 0.04))
    {
        int buffor = editModeActiveObject;
        for(int i=0; i<activeObject.size(); i++)
        {
            editModeActiveObject = activeObject[i];
            moveObject(position[0] - center.x, position[1] - center.y, position[2] - center.z);
        }
        editModeActiveObject = buffor;
    }

    ImGui::Separator();
    int sizeOfObject[3];
    sizeOfObject[0] = maxx - minx;
    sizeOfObject[1] = maxy - miny;
    sizeOfObject[2] = maxz - minz;
    float divideX = float(maxx - minx);
    float divideY = float(maxy - miny);
    float divideZ = float(maxz - minz);
    bool zeroX = false;
    bool zeroY = false;
    bool zeroZ = false;
    if(sizeOfObject[0] == 0)
    {
        zeroX = true;
    }
    if(sizeOfObject[1] == 0)
    {
        zeroY = true;
    }
    if(sizeOfObject[2] == 0)
    {
        zeroZ = true;
    }
    if(ImGui::DragInt3("Edit Size XYZ", sizeOfObject, 0.02))
    {
        if(zeroX == true)
        {
            divideX = 1;
            sizeOfObject[0] = 1;
        }
        if(zeroY == true)
        {
            divideY = 1;
            sizeOfObject[1] = 1;
        }
        if(zeroZ == true)
        {
            divideZ = 1;
            sizeOfObject[2] = 1;
        }
        int buffor = editModeActiveObject;
        for(int i=0; i<activeObject.size(); i++)
        {
            editModeActiveObject = activeObject[i];
            resizeObject(float(sizeOfObject[0])/divideX,
                         float(sizeOfObject[1])/divideY,
                         float(sizeOfObject[2])/divideZ,
                         center);
        }
        editModeActiveObject = buffor;
    }
    ImGui::Separator();
    float tempAngle = currentAngle;
    ImGui::SliderAngle("Rotate", &currentAngle, -200.0f, 200.0f);
    if(tempAngle != currentAngle)
    {
        sf::Vector3f angle = sf::Vector3f(0, 0, 0);
        if(rotationAxis == 0)
            angle.x = currentAngle - tempAngle;
        else if(rotationAxis == 1)
            angle.y = currentAngle - tempAngle;
        else
            angle.z = currentAngle - tempAngle;

        int buffor = editModeActiveObject;
        for(int i=0; i<activeObject.size(); i++)
        {
            editModeActiveObject = activeObject[i];
            rotateObject(angle,center);
        }
        editModeActiveObject = buffor;
    }
    ImGui::End();
    ImGui::PopStyleColor();
}

void ObjectUtility::showTopWindow()
{
    ImGui::SetNextWindowPos(sf::Vector2u(1.5*xButtonInterval + leftWindowWidth,yButtonInterval));
    ImGui::SetNextWindowSize(topWindowSize);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, topWindowBackgroundColor);
    ImGui::Begin("Edit Object");

    int position[3];
    int minx, maxx, miny, maxy, minz, maxz;
    calcObjectBoundaries(minx, maxx, miny, maxy, minz, maxz);
    sf::Vector3f center;
    if(selectedCenter == 1)
    {
        center.x = minx + (maxx - minx)*0.5f;
        center.y = miny + (maxy - miny)*0.5f;
        center.z = minz + (maxz - minz)*0.5f;
    }
    else if(selectedCenter == 2)
    {
        center = personalCenter;
    }
    else
    {
        center.x = minx;
        center.y = miny;
        center.z = minz;
    }

    position[0] = center.x;
    position[1] = center.y;
    position[2] = center.z;
    int positionBuffor[3];
    positionBuffor[0] = position[0];
    positionBuffor[1] = position[1];
    positionBuffor[2] = position[2];

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
            position[2] += 1;
        else
            position[1] += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
            position[2] += -1;
        else
            position[1] += -1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        position[0] += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        position[0] += -1;

    ImGui::DragInt3("Edit Position XYZ", position, 0.04);
    if(position[0] != positionBuffor[0] or position[1] != positionBuffor[1] or position[2] != positionBuffor[2])
    {
        moveObject(position[0] - center.x, position[1] - center.y, position[2] - center.z);
    }

    ImGui::Separator();
    int sizeOfObject[3];
    sizeOfObject[0] = maxx - minx;
    sizeOfObject[1] = maxy - miny;
    sizeOfObject[2] = maxz - minz;
    float divideX = float(maxx - minx);
    float divideY = float(maxy - miny);
    float divideZ = float(maxz - minz);
    bool zeroX = false;
    bool zeroY = false;
    bool zeroZ = false;
    if(sizeOfObject[0] == 0)
    {
        zeroX = true;
    }
    if(sizeOfObject[1] == 0)
    {
        zeroY = true;
    }
    if(sizeOfObject[2] == 0)
    {
        zeroZ = true;
    }
    if(ImGui::DragInt3("Edit Size XYZ", sizeOfObject, 0.02))
    {
        if(sizeOfObject[0] == 0)
        {
            sizeOfObject[0] = 1;
        }
        if(sizeOfObject[1] == 0)
        {
            sizeOfObject[1] = 1;
        }
        if(sizeOfObject[2] == 0)
        {
            sizeOfObject[2] = 1;
        }
        if(zeroX == true)
        {
            divideX = 1;
            sizeOfObject[0] = 1;
        }
        if(zeroY == true)
        {
            divideY = 1;
            sizeOfObject[1] = 1;
        }
        if(zeroZ == true)
        {
            divideZ = 1;
            sizeOfObject[2] = 1;
        }
        resizeObject(float(sizeOfObject[0])/divideX,
                 float(sizeOfObject[1])/divideY,
                 float(sizeOfObject[2])/divideZ,
                 center);
    }
    ImGui::Separator();
    float tempAngle = currentAngle;
    ImGui::SliderAngle("Rotate", &currentAngle, -200.0f, 200.0f);
    if(tempAngle != currentAngle)
    {
        sf::Vector3f angle = sf::Vector3f(0, 0, 0);
        if(rotationAxis == 0)
            angle.x = currentAngle - tempAngle;
        else if(rotationAxis == 1)
            angle.y = currentAngle - tempAngle;
        else
            angle.z = currentAngle - tempAngle;
        rotateObject(angle,center);
    }
    ImGui::End();
    ImGui::PopStyleColor();
}

void ObjectUtility::showBottomWindow()
{
    ImGui::SetNextWindowPos(sf::Vector2u(1.5*xButtonInterval + leftWindowWidth, p->window->getSize().y - bottomWindowHeight - yButtonInterval));
    ImGui::SetNextWindowSize(bottomWindowSize);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, bottomWindowBackgroundColor);
    ImGui::Begin("Save Menu");
    ImGui::InputText("", filename, 100);
    ImGui::SameLine();
    if(ImGui::Button("Save", ImVec2(60, 20)))
    {
        std::string s = filename;
        std::string path = "files/maps/synth3d/" + s + ".dat";
        if(fileExists(path))
        {
            errorMessage = "This file already exists!";
            errorMessageActivate = true;
            errorMessageJustActivated = true;
        }
        else
        {
            p->saveMap(s);
            errorMessage = "Everything went fine!\nYour map is saved";
            errorMessageActivate = true;
            errorMessageJustActivated = true;
        }
    }
    if(ImGui::Button("Make Backup", ImVec2(bottomWindowSize.x/2 - 14, 20)))
    {
        makeBackup();
        p->saveMap("backup");
    }
    ImGui::SameLine();
    if(ImGui::Button("Restore Backup", ImVec2(bottomWindowSize.x/2 - 14, 20)))
    {
        restoreBackup();
    }
    ImGui::End();
    ImGui::PopStyleColor();
}

void ObjectUtility::showErrorMessage()
{
    ImGui::Begin("ERROR");
    ImGui::Text(errorMessage.c_str());
    if(ImGui::Button("Understood", sf::Vector2u(100,20)))
    {
        errorMessage = "No Errors";
        errorMessageActivate = false;
    }
    ImGui::End();
}

void ObjectUtility::showWallCreationWindow()
{
    int start;
    int finish;
    calcStartAndFinishSafe(start, finish);
    int objectCount;
    objectCount = calcObjectSize();
    Wall& wallie = tempWall;

    if(createWallModeJustActivated)
    {
        createWallModeJustActivated = false;
        tempWall.clear();
        for(int i=0; i<4; i++)
        {
            if(!wallie.coord.empty())
            {
                if(wallie.coord[wallie.size()-1] == finish)
                {
                    wallie.push_back(finish);
                }
                else
                {
                    wallie.push_back(wallie.coord[wallie.size()-1] + 1);
                }
            }
            else
            {
                wallie.push_back(start);
            }
        }
    }
    ImGui::PushStyleColor(ImGuiCol_Button, leftNewButtonColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, leftActiveButtonColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, leftActiveButtonColorSemi);
    if(ImGui::Button("Add point", sf::Vector2u(100,20)))
    {
        if(!wallie.coord.empty())
        {
            if(wallie.coord[wallie.size()-1] == finish)
            {
                wallie.push_back(finish);
            }
            else
            {
                wallie.push_back(wallie.coord[wallie.size()-1] + 1);
            }
        }
        else
        {
            wallie.push_back(start);
        }
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, sfColorToImColor(sf::Color(122,0,0)));
    if(ImGui::Button("Delete last", sf::Vector2u(100,20)))
    {
        if(!wallie.coord.empty())
            wallie.coord.erase(wallie.coord.begin() + wallie.size() - 1);
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();

    for(int i=0; i<wallie.size(); i++)
    {
        int falseCoord = wallie.coord[i] - start;
        ImGui::SliderInt(stringify(i).c_str(), &falseCoord, 0, objectCount - 1);
        wallie.coord[i] = falseCoord + start;
    }
    if(ImGui::Button("Close", ImVec2(60, 20)))
    {
        createWallModeActive = false;
    }
    ImGui::SameLine();
    if(ImGui::Button("Apply", ImVec2(60, 20)))
    {
        if(tempWall.size() > 2)
        {
            p->world[editModeActiveObject].push_back(tempWall);
        }
        else
        {
            errorMessage = "I dont know why but this sure will crash the app\nso you better add some random points just to\nmodify them later";
            errorMessageActivate = true;
            errorMessageJustActivated = true;
        }
    }
}

void ObjectUtility::moveObject(int deltaX, int deltaY, int deltaZ)
{
    /*
    int start = 2000000000;
    int finish = 0;
    int i = editModeActiveObject;
    for(int j=0; j<p->world[i].size(); j++)
    {
        for(int k=0; k < p->world[i].wallie[j].size(); k++)
        {
            if(p->world[i].wallie[j].coord[k] > finish)
                finish = p->world[i].wallie[j].coord[k];
            if(p->world[i].wallie[j].coord[k] < start)
                start = p->world[i].wallie[j].coord[k];
        }
    }
    for(int j=0; j<p->world[i].size(); j++)
    {
        for(int k=0; k<p->world[i].wallie[j].lineStrip.size(); k++)
        {
            for(int m=0; m<p->world[i].wallie[j].lineStrip[k].size(); m++)
            {
                if(p->world[i].wallie[j].lineStrip[k][m] > finish)
                    finish = p->world[i].wallie[j].lineStrip[k][m];
                if(p->world[i].wallie[j].lineStrip[k][m] < start)
                    start = p->world[i].wallie[j].lineStrip[k][m];
            }
        }
    }
    */
    for(int m=p->world[editModeActiveObject].beginning; m <= p->world[editModeActiveObject].finish; m++)
    {
        p->terrain[m].x += deltaX;
        p->terrain[m].y += deltaY;
        p->terrain[m].z += deltaZ;
    }
}

void ObjectUtility::calcObjectBoundaries(int& minx, int& maxx, int& miny, int& maxy, int& minz, int& maxz)
{
        sf::Vector3f first = p->terrain[p->world[editModeActiveObject].beginning];
        float minxF = first.x;
        float maxxF = first.x;
        float minyF = first.y;
        float maxyF = first.y;
        float minzF = first.z;
        float maxzF = first.z;
        for(int j=p->world[editModeActiveObject].beginning + 1; j<=p->world[editModeActiveObject].finish; j++)
        {
            sf::Vector3f temp = p->terrain[j];
            maxxF = std::max(maxxF, temp.x);
            minxF = std::min(minxF, temp.x);
            maxyF = std::max(maxyF, temp.y);
            minyF = std::min(minyF, temp.y);
            maxzF = std::max(maxzF, temp.z);
            minzF = std::min(minzF, temp.z);
        }
        minx = int(roundf(minxF));
        maxx = int(roundf(maxxF));
        miny = int(roundf(minyF));
        maxy = int(roundf(maxyF));
        minz = int(roundf(minzF));
        maxz = int(roundf(maxzF));
}

void ObjectUtility::resizeObject(float sizeX, float sizeY, float sizeZ, sf::Vector3f center)
{
    /*
    int start = 2000000000;
    int finish = 0;
    int i = editModeActiveObject;
    for(int j=0; j<p->world[editModeActiveObject].size(); j++)
            for(int k=0; k<p->world[editModeActiveObject].wallie[j].size(); k++)
            {
                if(p->world[i].wallie[j].coord[k] > finish)
                    finish = p->world[i].wallie[j].coord[k];
                if(p->world[i].wallie[j].coord[k] < start)
                    start = p->world[i].wallie[j].coord[k];
            }
    for(int j=0; j<p->world[i].size(); j++)
    {
        for(int k=0; k<p->world[i].wallie[j].lineStrip.size(); k++)
        {
            for(int m=0; m<p->world[i].wallie[j].lineStrip[k].size(); m++)
            {
                if(p->world[i].wallie[j].lineStrip[k][m] > finish)
                    finish = p->world[i].wallie[j].lineStrip[k][m];
                if(p->world[i].wallie[j].lineStrip[k][m] < start)
                    start = p->world[i].wallie[j].lineStrip[k][m];
            }
        }
    }
    */
    for(int m=p->world[editModeActiveObject].beginning; m <= p->world[editModeActiveObject].finish; m++)
    {
        p->terrain[m].x = center.x + (p->terrain[m].x - center.x)*sizeX;
        p->terrain[m].y = center.y + (p->terrain[m].y - center.y)*sizeY;
        p->terrain[m].z = center.z + (p->terrain[m].z - center.z)*sizeZ;
    }
}

void ObjectUtility::calcStartAndFinish(int& start, int& finish)
{
    start = p->world[editModeActiveObject].beginning;
    finish = p->world[editModeActiveObject].finish;
    /*
    start = 2000000000;
    finish = 0;
    int i = editModeActiveObject;
    for(int j=0; j<p->world[i].size(); j++)
    {
        for(int k=0; k < p->world[i].wallie[j].size(); k++)
        {
            if(p->world[i].wallie[j].coord[k] > finish)
                finish = p->world[i].wallie[j].coord[k];
            if(p->world[i].wallie[j].coord[k] < start)
                start = p->world[i].wallie[j].coord[k];
        }
    }
    for(int j=0; j<p->world[i].size(); j++)
    {
        for(int k=0; k<p->world[i].wallie[j].lineStrip.size(); k++)
        {
            for(int m=0; m<p->world[i].wallie[j].lineStrip[k].size(); m++)
            {
                if(p->world[i].wallie[j].lineStrip[k][m] > finish)
                    finish = p->world[i].wallie[j].lineStrip[k][m];
                if(p->world[i].wallie[j].lineStrip[k][m] < start)
                    start = p->world[i].wallie[j].lineStrip[k][m];
            }
        }
    }*/
}

void ObjectUtility::calcStartAndFinishSafe(int& start, int& finish)
{
    start = p->world[editModeActiveObject].beginning;
    finish = p->world[editModeActiveObject].finish;
/*
    int tempStart, tempFinish;
    int counter = 0;
    while(1==1)
    {
        editModeActiveObject--;
        counter++;
        if(editModeActiveObject < 0)
        {
            tempFinish = -1;
            break;
        }
        calcStartAndFinish(tempStart, tempFinish);
        if(tempFinish >= tempStart)
            break;
    }
    editModeActiveObject += counter;
    start = tempFinish + 1;
    counter = 0;
    while(1==1)
    {
        editModeActiveObject++;
        counter++;
        if(editModeActiveObject >= p->world.size())
        {
            tempStart = p->terrain.size();
            break;
        }
        calcStartAndFinish(tempStart, tempFinish);
        if(tempFinish >= tempStart)
            break;
    }
    finish = tempStart - 1;
    editModeActiveObject -= counter;
    */
}

int ObjectUtility::calcObjectSize()
{
    int start, finish;
    calcStartAndFinishSafe(start, finish);
    return finish - start + 1;
}

void ObjectUtility::saveObject()
{
    int start;
    int finish;
    calcStartAndFinish(start, finish);
    object.clear();
    for(int m=start; m<=finish; m++)
    {
        object.push_back(p->terrain[m]);
    }
}

sf::Vector3f ObjectUtility::rotateVector(sf::Vector3f vec, sf::Vector3f angle, sf::Vector3f center)
{
    vec -= center;
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
    return target + center;
}

void ObjectUtility::rotateObject(sf::Vector3f angle, sf::Vector3f center)
{
    int start, finish;
    calcStartAndFinishSafe(start, finish);
    for(int i=start; i<=finish; i++)
    {
        p->terrain[i] = rotateVector(p->terrain[i], angle, center);
    }
}

void ObjectUtility::alignToGrid(int gridSize)
{
    int start = 2000000000;
    int finish = 0;
    int i = editModeActiveObject;
    for(int j=0; j<p->world[i].size(); j++)
    {
        for(int k=0; k < p->world[i].wallie[j].size(); k++)
        {
            if(p->world[i].wallie[j].coord[k] > finish)
                finish = p->world[i].wallie[j].coord[k];
            if(p->world[i].wallie[j].coord[k] < start)
                start = p->world[i].wallie[j].coord[k];
        }
    }
    std::vector <std::pair <float, float> > lineStrip;
    for(int j=0; j<p->world[i].size(); j++)
    {
        for(int k=0; k<p->world[i].wallie[j].lineStrip.size(); k++)
        {
            for(int m=0; m<p->world[i].wallie[j].lineStrip[k].size(); m++)
            {
                float a, b, c;
                sf::Vector3f destinationVector = p->terrain[p->world[i].wallie[j].lineStrip[k][m]] -
                                                 p->terrain[p->world[i].wallie[j].coord[0]];
                sf::Vector3f vec1 = p->terrain[p->world[i].wallie[j].coord[1]] -
                                    p->terrain[p->world[i].wallie[j].coord[0]];
                sf::Vector3f vec2 = p->terrain[p->world[i].wallie[j].coord[p->world[i].wallie[j].size()-1]] -
                                    p->terrain[p->world[i].wallie[j].coord[0]];
                sf::Vector3f vec3 = crossProduct(vec1, vec2);
                representVectorAsThreeOthers(destinationVector, vec1, vec2, vec3, a, b, c);
                lineStrip.push_back({a, b});
            }
        }
    }
    for(int n=start; n<=finish; n++)
    {
        if(fmodf(p->terrain[n].x, float(gridSize)) > float(gridSize)/2.0f)
            p->terrain[n].x = p->terrain[n].x - fmodf(p->terrain[n].x, float(gridSize)) + float(gridSize);
        else
            p->terrain[n].x = p->terrain[n].x - fmodf(p->terrain[n].x, float(gridSize));

        if(fmodf(p->terrain[n].y, float(gridSize)) > float(gridSize)/2.0f)
            p->terrain[n].y = p->terrain[n].y - fmodf(p->terrain[n].y, float(gridSize)) + float(gridSize);
        else
            p->terrain[n].y = p->terrain[n].y - fmodf(p->terrain[n].y, float(gridSize));

        if(fmodf(p->terrain[n].z, float(gridSize)) > float(gridSize)/2.0f)

            p->terrain[n].z = p->terrain[n].z - fmodf(p->terrain[n].z, float(gridSize)) + float(gridSize);
        else
            p->terrain[n].z = p->terrain[n].z - fmodf(p->terrain[n].z, float(gridSize));
    }

    start = 2000000000;
    finish = 0;
    int counter = 0;
    for(int j=0; j<p->world[i].size(); j++)
    {
        for(int k=0; k<p->world[i].wallie[j].lineStrip.size(); k++)
        {
            for(int m=0; m<p->world[i].wallie[j].lineStrip[k].size(); m++)
            {
                sf::Vector3f vec1 = p->terrain[p->world[i].wallie[j].coord[1]] -
                                    p->terrain[p->world[i].wallie[j].coord[0]];
                sf::Vector3f vec2 = p->terrain[p->world[i].wallie[j].coord[p->world[i].wallie[j].size()-1]] -
                                    p->terrain[p->world[i].wallie[j].coord[0]];
                p->terrain[p->world[i].wallie[j].lineStrip[k][m]] = p->terrain[p->world[i].wallie[j].coord[0]] +
                lineStrip[counter].first*vec1 + lineStrip[counter].second*vec2;
                counter++;
            }
        }
    }
}

void ObjectUtility::representVectorAsThreeOthers(sf::Vector3f destination,
                                  sf::Vector3f vec1, sf::Vector3f vec2, sf::Vector3f vec3,
                                  float& a, float& b, float& c)
{
    float equationDet = det3f(vec1, vec2, vec3);
    float aDet = det3f(destination, vec2, vec3);
    float bDet = det3f(vec1, destination, vec3);
    float cDet = det3f(vec1, vec2, destination);
    a = aDet / equationDet;
    b = bDet / equationDet;
    c = cDet / equationDet;
}

void ObjectUtility::alignObject()
{
    int start, finish;
    calcStartAndFinishSafe(start, finish);
    for(int i=start; i<=finish; i++)
    {
        p->terrain[i].x = roundf(p->terrain[i].x);
        p->terrain[i].y = roundf(p->terrain[i].y);
        p->terrain[i].z = roundf(p->terrain[i].z);
    }
}

void ObjectUtility::alignAll()
{
    for(int i=0; i<p->terrain.size(); i++)
    {
        p->terrain[i].x = roundf(p->terrain[i].x);
        p->terrain[i].y = roundf(p->terrain[i].y);
        p->terrain[i].z = roundf(p->terrain[i].z);
    }
}

void ObjectUtility::makeBackup()
{
    worldBackup = p->world;
    terrainBackup = p->terrain;
    defTerrainBackup = p->defTerrain;
    backupMade = true;
}

void ObjectUtility::restoreBackup()
{
    if(backupMade)
    {
        p->world = worldBackup;
        p->terrain = terrainBackup;
        p->defTerrain = defTerrainBackup;
    }
    else
    {
        errorMessage = "No backup made!";
        errorMessageActivate = true;
        errorMessageJustActivated = true;
    }
}

void ObjectUtility::deleteTerrainPoint(int index)
{
    ///Fix NAN
    p->terrain.erase(p->terrain.begin() + index);
    p->defTerrain.erase(p->defTerrain.begin() + index);
    if(index == p->world[editModeActiveObject].beginning)
    {
        p->world[editModeActiveObject].beginning++;
    }
    p->world[editModeActiveObject].finish--;
    for(int i=editModeActiveObject+1; i<p->world.size(); i++)
    {
        p->world[i].beginning--;
        p->world[i].finish--;
    }
    for(int i=0; i<p->world.size(); i++)
    {
        for(int j=0; j<p->world[i].size(); j++)
        {
            for(int m=0; m<p->world[i].wallie[j].size(); m++)
            {
                if(p->world[i].wallie[j].coord[m] >= index and p->world[i].wallie[j].coord[m] > p->world[i].beginning)
                {
                    if(p->world[i].wallie[j].coord[m] > 0)
                        p->world[i].wallie[j].coord[m]--;
                }
            }
            for(int m=0; m<p->world[i].wallie[j].lineStrip.size(); m++)
            {
                for(int n=0; n<p->world[i].wallie[j].lineStrip[m].size(); n++)
                {
                    if(p->world[i].wallie[j].lineStrip[m][n] >= index and p->world[i].wallie[j].lineStrip[m][n] > p->world[i].beginning)
                    {
                        if(p->world[i].wallie[j].lineStrip[m][n] > 0)
                            p->world[i].wallie[j].lineStrip[m][n]--;
                    }
                }
            }
        }
    }
}


SYNTH3D::SYNTH3D(std::string _name, SceneManager* mgr, sf::RenderWindow* w) :
    Scene(_name,mgr,w), c(this), cameraPos({0, 0, -50}), cameraAngle({0, 0, 0}),
    eyeDistance(-6), debug(0), offsetx(0), offsety(0), tick(1),
    gravity({0, -3.0f, 0}), catVelocity({0.0f, 0.0f, 10.0f}), mouseSpeed(7.5f),
    grabbedMouse(true), gBuffor(false), editMap(this, window->getSize()),
    firstPerson(true)
{
}

void SYNTH3D::onSceneLoadToMemory()
{
    consoleCommands.push_back("print_graph");
    consoleCommands.push_back("print_cycle");
    consoleCommands.push_back("print_info");
    consoleCommands.push_back("enable_cycleReduction");
    consoleCommands.push_back("enable_wallIntersect");
    consoleCommands.push_back("enable_randomSort");
    consoleCommands.push_back("enable_identifyWalls");
    consoleCommands.push_back("enable_distanceCheck");
    consoleCommands.push_back("compare");

    //loadMap("human");
    //loadMap("human_4");
    //loadMap("backup");
    //loadMap("backup");
    loadMap("oblivion_room_3");
    loadMap("human_4");
    defTerrain = terrain;
    //loadEntities("oblivion_npc");
    loadEntities("oblivion_room_npc");
    //OptLines();
    //OptDots();
    c.update();
    bufforMousePos = sf::Mouse::getPosition(*window);
}

void SYNTH3D::draw(sf::Time deltaTime)
{
    //ImGui::SetMouseCursor(false);
    dt = deltaTime.asMilliseconds() / 16.6666f;
    sf::Vector2i deltaMouse = sf::Vector2i(0,0);
    if(grabbedMouse)
    {
        deltaMouse = getDeltaMouse();
        window->setMouseCursorVisible(false);
    }

    if(dt == 0.0d)
        dt = 1.0d;
    float movementSpeed = 2.0f * dt;
    float cameraRotationSpeed = 1.0f * dt;
    float catMovingSpeed = 2.0f * dt;
    float catRotationSpeed = 1.0f * dt;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
    {
        movementSpeed = 7.0f * dt;
        cameraRotationSpeed = 2.0f * dt;
        catMovingSpeed = 7.0f * dt;
        catRotationSpeed = 2.0f * dt;
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

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
        cameraAngle.z-= cameraRotationSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
        cameraAngle.z+= cameraRotationSpeed;

    if(gBuffor)
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::G))
            grabbedMouse = !grabbedMouse;
    gBuffor = sf::Keyboard::isKeyPressed(sf::Keyboard::G);


    cameraAngle.x -= float(deltaMouse.y) / mouseSpeed;
    cameraAngle.y -= float(deltaMouse.x) / mouseSpeed;

    if(firstPerson)
    {
        sf::Vector3f buffor = catVelocity;
        float angle = 0.0174524f * catRotationSpeed * float(deltaMouse.x) / mouseSpeed;
        catAngle.y -= angle;
        catVelocity.x = buffor.x*cosf(angle) + buffor.z*sinf(angle);
        catVelocity.z = buffor.z*cosf(angle) - buffor.x*sinf(angle);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        sf::Vector3f buffor = catVelocity;
        float angle = -0.0174524f * catRotationSpeed;
        catAngle.y -= angle;
        catVelocity.x = buffor.x*cosf(angle) + buffor.z*sinf(angle);
        catVelocity.z = buffor.z*cosf(angle) - buffor.x*sinf(angle);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        sf::Vector3f buffor = catVelocity;
        float angle = 0.0174524f * catRotationSpeed;
        catAngle.y -= angle;
        catVelocity.x = buffor.x*cosf(angle) + buffor.z*sinf(angle);
        catVelocity.z = buffor.z*cosf(angle) - buffor.x*sinf(angle);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))
        character[0].setAcceleration(catVelocity);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::K))
        character[0].setAcceleration(-catVelocity);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::J))
        character[0].setAcceleration({-catVelocity.z, 0, catVelocity.x});
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
        character[0].setAcceleration({catVelocity.z, 0, -catVelocity.x});
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        character[0].jump(60);

    if(character[0].position.y < -1000)
    {
        character[0].setPos({400, 350, 300});
        character[0].setSpeed({0,0,0});
    }
    character[0].setGravity(gravity);
    character[0].setDirection(catVelocity, {0,0,0});
    character[0].update(dt);
    //cameraPos = terrain[34];
    if(firstPerson)
    {
        if(cameraAngle.x < -45)
            cameraAngle.x = -45;
        if(cameraAngle.x > 45)
            cameraAngle.x = 45;
        cameraPos = character[0].ellipsoidCenter + sf::Vector3f(0, 170, 0);
        cameraAngle = sf::Vector3f(cameraAngle.x, catAngle.y * (360.0f / (2*M_PI)), 0);
    }


    c.setPos(cameraPos);
    c.setAngle(cameraAngle);
    c.setEyeDistance(eyeDistance);
    c.display();

    //editMap.show();
    c.update();
}

sf::Vector2i SYNTH3D::getDeltaMouse()
{
    //sf::Vector2i buffor = bufforMousePos;
    sf::Vector2i buffor = {int(window->getSize().x)/2, int(window->getSize().y)/2};
    bufforMousePos = sf::Mouse::getPosition(*window);
    sf::Mouse::setPosition({int(window->getSize().x)/2, int(window->getSize().y)/2});
    return bufforMousePos - buffor;
}

void SYNTH3D::checkCollision(collisionPacket* colPackage, int beginning, int ending)
{
    for(int i=0; i<beginning; i++)
    {
        for(int j=0; j<world[i].size(); j++)
        {
            for(int k=0; k<world[i].wallie[j].size()-2; k++)
            {
                sf::Vector3f p1 = terrain[world[i].wallie[j].coord[0]] / colPackage->eRadius;
                sf::Vector3f p2 = terrain[world[i].wallie[j].coord[k+1]] / colPackage->eRadius;
                sf::Vector3f p3 = terrain[world[i].wallie[j].coord[k+2]] / colPackage->eRadius;
                ///ASSUMES: Triangles are in ellipsoid space:
                checkTriangle(colPackage, p1, p2, p3);
            }
        }
    }
    for(int i=ending+1; i<world.size(); i++)
    {
        for(int j=0; j<world[i].size(); j++)
        {
            for(int k=0; k<world[i].wallie[j].size()-2; k++)
            {
                sf::Vector3f p1 = terrain[world[i].wallie[j].coord[0]] / colPackage->eRadius;
                sf::Vector3f p2 = terrain[world[i].wallie[j].coord[k+1]] / colPackage->eRadius;
                sf::Vector3f p3 = terrain[world[i].wallie[j].coord[k+2]] / colPackage->eRadius;
                ///ASSUMES: Triangles are in ellipsoid space:
                checkTriangle(colPackage, p1, p2, p3);
            }
        }
    }
}

void SYNTH3D::checkTriangle(collisionPacket* colPackage, const sf::Vector3f& p1, const sf::Vector3f& p2, const sf::Vector3f& p3)
{
    Plane trianglePlane(p1,p2,p3);
    if (trianglePlane.isFrontFacingTo( colPackage->normalizedVelocity ))
    {
        double t0, t1;
        bool embeddedInPlane = false;
        double signedDistToTrianglePlane = trianglePlane.signedDistanceTo(colPackage->basePoint);
        float normalDotVelocity = dotProduct(trianglePlane.normal, colPackage->velocity);
        if (normalDotVelocity == 0.0f)
        {
            if (fabs(signedDistToTrianglePlane) >= 1.0f)
            {
                return;
            }
            else
            {
                embeddedInPlane = true;
                t0 = 0.0;
                t1 = 1.0;
            }
        }
        else
        {
            t0=(-1.0-signedDistToTrianglePlane) / normalDotVelocity;
            t1=( 1.0-signedDistToTrianglePlane) / normalDotVelocity;
            if (t0 > t1)
            {
                double temp = t1;
                t1 = t0;
                t0 = temp;
            }
            if (t0 > 1.0f || t1 < 0.0f)
            {
                return;
            }
            if (t0 < 0.0) t0 = 0.0;
            if (t1 < 0.0) t1 = 0.0;
            if (t0 > 1.0) t0 = 1.0;
            if (t1 > 1.0) t1 = 1.0;
        }
        sf::Vector3f collisionPoint;
        bool foundCollison = false;
        float t = 1.0;
        if (!embeddedInPlane)
        {
            sf::Vector3f planeIntersectionPoint = (colPackage->basePoint-trianglePlane.normal)
                + float(t0) * colPackage->velocity;
            if (checkPointInTriangle(planeIntersectionPoint, p1,p2,p3))
            {
                foundCollison = true;
                t = t0;
                collisionPoint = planeIntersectionPoint;
            }
        }
        if (foundCollison == false)
        {
            sf::Vector3f velocity = colPackage->velocity;
            sf::Vector3f base = colPackage->basePoint;
            float velocitySquaredLength = vecSquaredLength(velocity);
            float a,b,c;
            float newT;
            a = velocitySquaredLength;
            b = 2.0*dotProduct(velocity, base-p1);
            c = vecSquaredLength(p1-base) - 1.0;
            if (getLowestRoot(a,b,c, t, &newT))
            {
                t = newT;
                foundCollison = true;
                collisionPoint = p1;
            }
            b = 2.0*dotProduct(velocity, base-p2);
            c = vecSquaredLength(p2-base) - 1.0;
            if (getLowestRoot(a,b,c, t, &newT))
            {
                t = newT;
                foundCollison = true;
                collisionPoint = p2;
            }
            b = 2.0*dotProduct(velocity, base-p3);
            c = vecSquaredLength(p3-base) - 1.0;
            if (getLowestRoot(a,b,c, t, &newT))
            {
                t = newT;
                foundCollison = true;
                collisionPoint = p3;
            }
            sf::Vector3f edge = p2-p1;
            sf::Vector3f baseToVertex = p1 - base;
            float edgeSquaredLength = vecSquaredLength(edge);
            float edgeDotVelocity = dotProduct(edge, velocity);
            float edgeDotBaseToVertex = dotProduct(edge, baseToVertex);
            a = edgeSquaredLength*-velocitySquaredLength +
                edgeDotVelocity*edgeDotVelocity;
            b = edgeSquaredLength*(2*dotProduct(velocity, baseToVertex))-
                2.0*edgeDotVelocity*edgeDotBaseToVertex;
            c = edgeSquaredLength*(1-vecSquaredLength(baseToVertex))+
                edgeDotBaseToVertex*edgeDotBaseToVertex;
            if (getLowestRoot(a,b,c, t, &newT))
            {
                float f=(edgeDotVelocity*newT-edgeDotBaseToVertex)/
                        edgeSquaredLength;
                if (f >= 0.0 && f <= 1.0)
                {
                    t = newT;
                    foundCollison = true;
                    collisionPoint = p1 + f*edge;
                }
            }
            edge = p3-p2;
            baseToVertex = p2 - base;
            edgeSquaredLength = vecSquaredLength(edge);
            edgeDotVelocity = dotProduct(edge, velocity);
            edgeDotBaseToVertex = dotProduct(edge, baseToVertex);
            a = edgeSquaredLength*-velocitySquaredLength +
                edgeDotVelocity*edgeDotVelocity;
            b = edgeSquaredLength*(2*dotProduct(velocity, baseToVertex))-
                2.0*edgeDotVelocity*edgeDotBaseToVertex;
            c = edgeSquaredLength*(1-vecSquaredLength(baseToVertex))+
                edgeDotBaseToVertex*edgeDotBaseToVertex;
            if (getLowestRoot(a,b,c, t, &newT))
            {
                float f=(edgeDotVelocity*newT-edgeDotBaseToVertex)/
                        edgeSquaredLength;
                if (f >= 0.0 && f <= 1.0)
                {
                    t = newT;
                    foundCollison = true;
                    collisionPoint = p2 + f*edge;
                }
            }
            edge = p1-p3;
            baseToVertex = p3 - base;
            edgeSquaredLength = vecSquaredLength(edge);
            edgeDotVelocity = dotProduct(edge, velocity);
            edgeDotBaseToVertex = dotProduct(edge, baseToVertex);
            a = edgeSquaredLength*-velocitySquaredLength +
                edgeDotVelocity*edgeDotVelocity;
            b = edgeSquaredLength*(2*dotProduct(velocity, baseToVertex))-
                2.0*edgeDotVelocity*edgeDotBaseToVertex;
            c = edgeSquaredLength*(1-vecSquaredLength(baseToVertex))+
                edgeDotBaseToVertex*edgeDotBaseToVertex;
            if (getLowestRoot(a,b,c, t, &newT))
            {
                float f=(edgeDotVelocity*newT-edgeDotBaseToVertex)/
                        edgeSquaredLength;
                if (f >= 0.0 && f <= 1.0)
                {
                    t = newT;
                    foundCollison = true;
                    collisionPoint = p3 + f*edge;
                }
            }
        }
        if (foundCollison == true)
        {
            float distToCollision = t*vecLength(colPackage->velocity);
            if (colPackage->foundCollision == false ||
                    distToCollision < colPackage->nearestDistance)
            {
                colPackage->nearestDistance = distToCollision;
                colPackage->intersectionPoint=collisionPoint;
                colPackage->foundCollision = true;
            }
        }
    }
}

bool SYNTH3D::checkPointInTriangle(const sf::Vector3f& point, const sf::Vector3f& pa,const sf::Vector3f& pb, const sf::Vector3f& pc)
{
    sf::Vector3f e10=pb-pa;
    sf::Vector3f e20=pc-pa;
    float a = dotProduct(e10,e10);
    float b = dotProduct(e10,e20);
    float c = dotProduct(e20,e20);
    float ac_bb=(a*c)-(b*b);
    sf::Vector3f vp(point.x-pa.x, point.y-pa.y, point.z-pa.z);
    float d = dotProduct(vp,e10);
    float e = dotProduct(vp,e20);
    float x = (d*c)-(e*b);
    float y = (e*a)-(d*b);
    float z = x+y-ac_bb;
    return (( ((unsigned int&)z)& ~(((unsigned int&)x)|((unsigned int&)y)) ) & 0x80000000);
}

bool SYNTH3D::getLowestRoot(float a, float b, float c, float maxR, float* root)
{
    float determinant = b*b - 4.0f*a*c;
    if (determinant < 0.0f) return false;
    float sqrtD = sqrtf(determinant);
    float r1 = (-b - sqrtD) / (2*a);
    float r2 = (-b + sqrtD) / (2*a);
    if (r1 > r2)
    {
        float temp = r2;
        r2 = r1;
        r1 = temp;
    }
    if (r1 > 0 && r1 < maxR)
    {
        *root = r1;
        return true;
    }
    if (r2 > 0 && r2 < maxR)
    {
        *root = r2;
        return true;
    }
    return false;
}

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
        if((file[0] >= '0' and file[0] <= '9') or file[0] == '-')
            index = 1;
        if(index < a)
            for(int i=0; i<file.size() - 1; i++)
            {
                if(file[i] == '\t' and ((file[i+1] >= '0' and file[i+1] <= '9') or file[i+1] == '-'))
                    index++;
                if(index == a)
                    {
                        adress = i+1;
                        break;
                    }
            }
        bool dotIndicator = false;
        float decimalCounter = 10;
        bool belowZero = false;
        if(file[adress] == '-')
        {
            belowZero = true;
            adress++;
        }
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
        if(belowZero)
        {
            result = -result;
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

                        world[world.size()-1].calcStartAndFinish();
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
    level.close();
}

void SYNTH3D::saveMap(std::string path)
{
    std::ofstream level;
    level.open("files/maps/synth3d/"+path+".dat");
    for(int i=0; i<world.size(); i++)
    {
        if(i>0)
            level << "\n\n";
        level << world[i].name;
        level << "\n\nopt:\t" << world[i].OptAllowed;
        if(world[i].size() > 0)
        {
            level << "\n\nVertex:";
            int start = 2000000000;
            int finish = 0;
            for(int j=0; j<world[i].size(); j++)
            {
                for(int k=0; k<world[i].wallie[j].size(); k++)
                {
                    if(world[i].wallie[j].coord[k] > finish)
                        finish = world[i].wallie[j].coord[k];
                    if(world[i].wallie[j].coord[k] < start)
                        start = world[i].wallie[j].coord[k];
                }
            }
            for(int j=start; j<=finish; j++)
            {
                level << "\n" << terrain[j].x << "\t" << terrain[j].y << "\t" << terrain[j].z;
            }
            for(int j=0; j<world[i].size(); j++)
            {
                for(int k=0; k<world[i].wallie[j].lineStrip.size(); k++)
                {
                    for(int m=0; m<world[i].wallie[j].lineStrip[k].size()-1; m++)
                    {
                        level << "\n" << terrain[world[i].wallie[j].lineStrip[k][m]].x << "\t" <<
                                         terrain[world[i].wallie[j].lineStrip[k][m]].y << "\t" <<
                                         terrain[world[i].wallie[j].lineStrip[k][m]].z;
                    }
                    if(world[i].wallie[j].lineStrip[k][0] != world[i].wallie[j].lineStrip[k][world[i].wallie[j].lineStrip[k].size()-1])
                        level << "\n" << terrain[world[i].wallie[j].lineStrip[k][world[i].wallie[j].lineStrip[k].size()-1]].x << "\t" <<
                                         terrain[world[i].wallie[j].lineStrip[k][world[i].wallie[j].lineStrip[k].size()-1]].y << "\t" <<
                                         terrain[world[i].wallie[j].lineStrip[k][world[i].wallie[j].lineStrip[k].size()-1]].z;
                }
            }
            level << "\nend\n\nWall:";

            for(int j=0; j<world[i].size(); j++)
            {
                if(world[i].wallie[j].pSize != 100)
                    level << "\nsize:\t" << world[i].wallie[j].pSize;
                if(world[i].wallie[j].grid > 0)
                    level << "\ngrid:\t" << world[i].wallie[j].grid;
                if(world[i].wallie[j].OptAllowed == false)
                    level << "\nopt:\t0";
                if(world[i].wallie[j].color != sf::Color::Green)
                    level << "\ntrans:\t" << int(world[i].wallie[j].trans.r) << "\t" <<
                                             int(world[i].wallie[j].trans.g) << "\t" <<
                                             int(world[i].wallie[j].trans.b) << "\t" <<
                                             int(world[i].wallie[j].trans.a);
                if(world[i].wallie[j].trans != sf::Color::Transparent)
                    level << "\ntrans:\t" << int(world[i].wallie[j].trans.r) << "\t" <<
                                             int(world[i].wallie[j].trans.g) << "\t" <<
                                             int(world[i].wallie[j].trans.b) << "\t" <<
                                             int(world[i].wallie[j].trans.a);
                level << "\nvert:";
                for(int k=0; k<world[i].wallie[j].size(); k++)
                {
                    level << "\t" << world[i].wallie[j].coord[k] - start;
                }
                bool lMask = false;
                for(int k=0; k<world[i].wallie[j].drawable.size(); k++)
                {
                    if(world[i].wallie[j].drawable[k] == false)
                    {
                        lMask = true;
                        break;
                    }
                }
                if(lMask)
                {
                    level << "\nlMask:";
                    for(int k=0; k<world[i].wallie[j].drawable.size(); k++)
                    {
                        level << "\t" << world[i].wallie[j].drawable[k];
                    }
                }
                bool dMask = false;
                for(int k=0; k<world[i].wallie[j].dotDraw.size(); k++)
                {
                    if(world[i].wallie[j].dotDraw[k] == false)
                    {
                        dMask = true;
                        break;
                    }
                }
                if(dMask)
                {
                    level << "\ndMask:";
                    for(int k=0; k<world[i].wallie[j].dotDraw.size(); k++)
                    {
                        level << "\t" << world[i].wallie[j].dotDraw[k];
                    }
                }
                if(!world[i].wallie[j].lineStrip.empty())
                {
                    for(int k=0; k<world[i].wallie[j].lineStrip.size(); k++)
                    {
                        level << "\nlStrip:";
                        for(int m=0; m<world[i].wallie[j].lineStrip[k].size(); m++)
                        {
                            level << "\t" << world[i].wallie[j].lineStrip[k][m] - start;
                        }
                    }
                }
                level << "\n###";
            }
            level << "\nend\nobjectEnd";
        }
    }
    level.close();
}

void SYNTH3D::loadEntities(std::string path)
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
    ///wall tmpWall; ///Uwaga zeby przy wpisywaniu kilku scian nie bylo nienadpisanych wartosci z poprzedniego
    int indicator = 0;
    int beginning = 0;
    int ending = 0;
    std::string name = "Invalid Name";
    sf::Vector3f eRadius = sf::Vector3f(1, 1, 1);
    sf::Vector3f positionGiven = sf::Vector3f(0, 0, 0);
    std::vector <std::pair <int, sf::Vector3f> > leftLeg;
    std::vector <std::pair <int, sf::Vector3f> > rightLeg;
    if(level.is_open())
        while(getline(level, line))
            if(!line.empty() and line[0] != '/' and line[1] != '/')
            {
                switch(indicator)
                {
                case 0:
                    name = line;
                    indicator = 1;
                    break;
                case 1:
                    if(line != "end")
                    {
                        if(first(line, "begin"))
                        {
                            for(int i=0; i<world.size();i++)
                            {
                                if(line.substr(7) == world[i].name)
                                {
                                    beginning = i;
                                    break;
                                }
                            }
                        }
                        else if(first(line, "finish"))
                        {
                            for(int i=0; i<world.size();i++)
                            {
                                if(line.substr(8) == world[i].name)
                                {
                                    ending = i;
                                    break;
                                }
                            }
                        }
                        else if(first(line, "eRadius"))
                        {
                            eRadius.x = value(line, 0).first; ///tomabycfloat
                            eRadius.y = value(line, 1).first;
                            eRadius.z = value(line, 2).first;
                        }
                        else if(first(line, "setPos"))
                        {
                            positionGiven.x = value(line, 0).first; ///tomabycfloat
                            positionGiven.y = value(line, 1).first;
                            positionGiven.z = value(line, 2).first;
                        }
                        else if(first(line, "leftLeg"))
                        {
                            int i = 0;
                            bool endOfFile = false;
                            while(!endOfFile)
                            {
                                int objectNumber = value(line, i++).first;
                                int wallNumber = value(line, i).first;
                                for(int j=0; j<world[objectNumber].wallie[wallNumber].size(); j++)
                                {
                                    leftLeg.push_back({world[objectNumber].wallie[wallNumber].coord[j],
                                                       defTerrain[world[objectNumber].wallie[wallNumber].coord[j]]});
                                }
                                endOfFile = value(line, i).second;
                                i++;
                            }
                        }
                        else if(first(line, "rightLeg"))
                        {
                            int i = 0;
                            bool endOfFile = false;
                            while(!endOfFile)
                            {
                                int objectNumber = value(line, i++).first;
                                int wallNumber = value(line, i).first;
                                for(int j=0; j<world[objectNumber].wallie[wallNumber].size(); j++)
                                {
                                    rightLeg.push_back({world[objectNumber].wallie[wallNumber].coord[j],
                                                       defTerrain[world[objectNumber].wallie[wallNumber].coord[j]]});
                                }
                                endOfFile = value(line, i).second;
                                i++;
                            }
                        }
                    }
                    else
                    {
                        Entity tmpEntity(this);
                        tmpEntity.name = name;
                        for(int i=beginning; i<=ending; i++)
                            tmpEntity.importObject(i);
                        tmpEntity.calcRotationCenter();
                        tmpEntity.setPos(positionGiven);
                        tmpEntity.setEllipsoidRadius(eRadius);
                        tmpEntity.leftLeg = leftLeg;
                        tmpEntity.rightLeg = rightLeg;
                        character.push_back(tmpEntity);
                        indicator = 0;
                        beginning = 0;
                        ending = 0;
                        eRadius = sf::Vector3f(1, 1, 1);
                        positionGiven = sf::Vector3f(0, 0, 0);
                        leftLeg.clear();
                        rightLeg.clear();
                        name = "Invalid Name";
                    }
                    break;
                }
            }
    level.close();
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
        if(args[0].size() >= 5 and args[0].substr(0, 5) == "print")
        {
                if(args[0].size() >= 11 and args[0].substr(6, 5) == "graph")
                {
                    c.printGraph();
                    return true;
                }
                else if(args[0].size() >= 11 and args[0].substr(6, 5) == "cycle")
                {
                    c.printCycle();
                    return true;
                }
                else if(args[0].size() >= 10 and args[0].substr(6, 4) == "info")
                {
                    showInfo();
                    return true;
                }
        }
        else if(args[0].size() >= 6 and args[0].substr(0, 6) == "enable")
        {
            if(args.size() == 2)
            {
                if(args[0].size() >= 21 and args[0].substr(7, 14) == "cycleReduction")
                {
                    c.enableCycleReduction(bool(atoi(args[1].c_str())));
                    return true;
                }
                else if(args[0].size() >= 20 and args[0].substr(7, 13) == "wallIntersect")
                {
                    c.enableWallIntersect(bool(atoi(args[1].c_str())));
                    return true;
                }
                else if(args[0].size() >= 17 and args[0].substr(7, 10) == "randomSort")
                {
                    c.enableRandomSort(bool(atoi(args[1].c_str())));
                    return true;
                }
                else if(args[0].size() >= 20 and args[0].substr(7, 13) == "identifyWalls")
                {
                    c.enableIdentifyWalls(bool(atoi(args[1].c_str())));
                    return true;
                }
                else if(args[0].size() >= 20 and args[0].substr(7, 13) == "distanceCheck")
                {
                    c.enableDistanceCheck(bool(atoi(args[1].c_str())));
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
    vEye({0, 0, -5}),
    screenSize(3),
    scale(100),
    minDotSize(2),
    topologicalError(false),
    sinceLastTopologicalError(0),
    cycleReductionEnabled(false),
    wallIntersectEnabled(true),
    randomSortEnabled(false),
    identifyWallsEnabled(false),
    distanceCheckEnabled(false),
    wallIntersectionOffset(0.95f),
    straightWallCheck(false)
{
    initCircleDef(32);
}

void Camera::printCycle()
{
    findCycle(wallSortGraph, wallSortGraphLevel);
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

void Camera::enableDistanceCheck(bool value)
{
    distanceCheckEnabled = value;
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
        bool beenThereLeft = false;
        bool beenThereRight = false;

        for(int i=0; i<p->world.size(); i++)
            for(int j=0; j<p->world[i].size(); j++)
            {
                if(left == counter and !beenThereLeft)
                {
                    leftie = &(p->world[i].wallie[j]);
                    leftWallFound = true;
                    p->printToConsole("First object: " + p->world[i].name);
                    beenThereLeft = true;
                }
                if(right == counter and !beenThereRight)
                {
                    rightie = &(p->world[i].wallie[j]);
                    rightWallFound = true;
                    p->printToConsole("Second object: " + p->world[i].name);
                    beenThereRight = true;
                }
                if(activeWall(&(p->world[i].wallie[j])))
                    counter++;
            }
        if(leftWallFound and rightWallFound)
        {
            std::pair <bool, bool> order = wallSortingAlgorythm(leftie, rightie);
            if(order.second)
            {
                if(!order.first)
                    p->printToConsole("Wall " + stringify(right) + " is in front of wall " + stringify(left), sf::Color::Cyan);
                else
                    p->printToConsole("[error] Both walls seem to be in front of each other");
            }
            else
            {
                if(order.first)
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
    calcAngle();
}

void Camera::setDirection(sf::Vector3f direction, sf::Vector3f origin)
{
    sf::Vector3f dirVec = direction - origin;
    float lengthX = sqrtf(dirVec.y*dirVec.y + dirVec.z*dirVec.z);
    float lengthY = sqrtf(dirVec.x*dirVec.x + dirVec.z*dirVec.z);
    float lengthZ = sqrtf(dirVec.x*dirVec.x + dirVec.y*dirVec.y);
    sinx = dirVec.y / lengthX;
    cosx = dirVec.z / lengthX;
    siny = dirVec.x / lengthY;
    cosy = dirVec.z / lengthY;
    sinz = dirVec.x / lengthZ;
    cosz = dirVec.y / lengthZ;
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
        spot2d[i] = flatView(planeCross(spot3d[i], vEye));
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
                spot.push_back(spot2d[wallie->coord[i]]);
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
                spot.push_back(spot2d[ wallie->coord[usefulSpot[i]] ]);
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
                spot.push_back(spot2d[wallie.coord[i]]);
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
                {
                    for(int j=0; j<wallie.lineStrip[i].size()-1; j++)
                    {
                        drawDot(spot2d[wallie.lineStrip[i][j]], dot[ wallie.lineStrip[i][j] ] * wallie.pSize, wallie.color);
                        drawLine(spot2d[wallie.lineStrip[i][j]], spot2d[wallie.lineStrip[i][j+1]], dot[ wallie.lineStrip[i][j] ] * wallie.pSize, dot[ wallie.lineStrip[i][j+1] ] * wallie.pSize, wallie.color, whatever);
                    }
                    if(wallie.lineStrip[i][0] != wallie.lineStrip[i][wallie.lineStrip[i].size()-1])
                    {
                        drawDot(spot2d[wallie.lineStrip[i][wallie.lineStrip[i].size()-1]],
                                dot[ wallie.lineStrip[i][wallie.lineStrip[i].size()-1] ] *
                                wallie.pSize, wallie.color);
                    }
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
                spot.push_back(spot2d[ wallie.coord[usefulSpot[i]] ]);
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
                {
                    for(int j=0; j<wallie.lineStrip[i].size()-1; j++)
                    {
                        if(spot3d[wallie.lineStrip[i][j]].z > 0)
                        {
                            drawDot(spot2d[wallie.lineStrip[i][j]], dot[ wallie.lineStrip[i][j] ] * wallie.pSize, wallie.color);
                            if(spot3d[wallie.lineStrip[i][j+1]].z > 0)
                                drawLine(spot2d[wallie.lineStrip[i][j]], spot2d[wallie.lineStrip[i][j+1]], dot[ wallie.lineStrip[i][j] ] * wallie.pSize, dot[ wallie.lineStrip[i][j+1] ] * wallie.pSize, wallie.color, whatever);
                            else
                            {
                                drawLine(spot2d[wallie.lineStrip[i][j]],
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
                            drawLine(spot2d[wallie.lineStrip[i][j+1]],
                                         flatView(planeCross(spot3d[wallie.lineStrip[i][j+1]], spot3d[wallie.lineStrip[i][j]])),
                                         dot[ wallie.lineStrip[i][j+1] ] * wallie.pSize,
                                         wallie.pSize,
                                         wallie.color,
                                         whatever);
                            drawDot(flatView(planeCross(spot3d[wallie.lineStrip[i][j+1]], spot3d[wallie.lineStrip[i][j]])), wallie.pSize, wallie.color);
                        }
                    }
                    if(spot3d[wallie.lineStrip[i][wallie.lineStrip[i].size()-1]].z > 0 and spot3d[wallie.lineStrip[i][0]] != spot3d[wallie.lineStrip[i][wallie.lineStrip[i].size()-1]])
                    {
                        drawDot(spot2d[wallie.lineStrip[i][wallie.lineStrip[i].size()-1]], dot[ wallie.lineStrip[i][wallie.lineStrip[i].size()-1] ] * wallie.pSize, wallie.color);
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

bool Camera::onPlane(sf::Vector3f& center, sf::Vector3f& top, sf::Vector3f& side, sf::Vector3f& point)
{
    sf::Vector3f planeTop = top - center;
    sf::Vector3f planeSide = side - center;
    sf::Vector3f planeNormal;
    planeNormal.x = planeSide.y*planeTop.z - planeTop.y*planeSide.z;
    planeNormal.y = planeSide.z*planeTop.x - planeTop.z*planeSide.x;
    planeNormal.z = planeSide.x*planeTop.y - planeTop.x*planeSide.y;
    sf::Vector3f posVector = point - center;
    float dotProduct = planeNormal.x*posVector.x + planeNormal.y*posVector.y + planeNormal.z*posVector.z;
    float epsilon = 3.0f;
    if(dotProduct < epsilon and dotProduct > -epsilon)
        return true;
    return false;
}

bool Camera::activeWall(Wall* wallie)
{
    sf::Vector3f planeTop = spot3d[wallie->coord[1]] - spot3d[wallie->coord[0]];
    sf::Vector3f planeSide = spot3d[wallie->coord[2]] - spot3d[wallie->coord[0]];
    sf::Vector3f planeNormal;
    planeNormal.x = planeSide.y*planeTop.z - planeTop.y*planeSide.z;
    planeNormal.y = planeSide.z*planeTop.x - planeTop.z*planeSide.x;
    planeNormal.z = planeSide.x*planeTop.y - planeTop.x*planeSide.y;
    sf::Vector3f posVector = vEye - spot3d[wallie->coord[0]];
    float dotProduct = planeNormal.x*posVector.x + planeNormal.y*posVector.y + planeNormal.z*posVector.z;
    if(dotProduct > 0.0f)
        return true;
    return false;
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
    float epsilon = 3.0f;
    if(dotProduct > epsilon)
        return 1;
    else if(dotProduct < -epsilon)
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

    if(straightWallCheck and leftBelowZero == 0 and rightBelowZero == 0)
        return wallIntersectOS(wallie1, wallie2);

    if(leftBelowZero == wallie1->size() or rightBelowZero == wallie2->size())
        return false;

    std::vector <sf::Vector2f> poly1 = wallToPoly(wallie1);
    std::vector <sf::Vector2f> poly2 = wallToPoly(wallie2);
    if(!straightWallCheck)
    {
        sf::Vector2f center;
        for(int i=0; i<poly1.size(); i++)
        {
            center += poly1[i];
        }
        center /= float(poly1.size());
        for(int i=0; i<poly1.size(); i++)
        {
            poly1[i] = center + (poly1[i] - center)*wallIntersectionOffset;
        }
        center = sf::Vector2f(0, 0);
        for(int i=0; i<poly2.size(); i++)
        {
            center += poly2[i];
        }
        center /= float(poly2.size());
        for(int i=0; i<poly2.size(); i++)
        {
            poly2[i] = center + (poly2[i] - center)*wallIntersectionOffset;
        }
    }
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
        bool firstSide = rightSide(spot2d[left->coord[i]], spot2d[left->coord[(i+1)%left->size()]], spot2d[right->coord[0]]);
        bool allOnOneSide = true;
        for(int j=1; j<right->size(); j++)
            if(firstSide != rightSide(spot2d[left->coord[i]], spot2d[left->coord[(i+1)%left->size()]], spot2d[right->coord[j]]))
            {
                allOnOneSide = false;
                break;
            }
        if(allOnOneSide)
        {
            sf::Vector2f opositePoint;
            for(int j = (i+2)%left->size(); j<left->size() + (i+2)%left->size(); j++)
                if(spot2d[left->coord[j%left->size()]] != spot2d[left->coord[i]] and
                   spot2d[left->coord[j%left->size()]] != spot2d[left->coord[(i+1)%left->size()]])
                {
                   opositePoint = spot2d[left->coord[j%left->size()]];
                   break;
                }
            if(firstSide != rightSide(spot2d[left->coord[i]], spot2d[left->coord[(i+1)%left->size()]], opositePoint))
                return false;
        }
    }

    for(int i=0; i<right->size(); i++)
    {
        bool firstSide = rightSide(spot2d[right->coord[i]], spot2d[right->coord[(i+1)%right->size()]], spot2d[left->coord[0]]);
        bool allOnOneSide = true;
        for(int j=1; j<left->size(); j++)
            if(firstSide != rightSide(spot2d[right->coord[i]], spot2d[right->coord[(i+1)%right->size()]], spot2d[left->coord[j]]))
            {
                allOnOneSide = false;
                break;
            }
        if(allOnOneSide)
        {
            sf::Vector2f opositePoint;
            for(int j = (i+2)%right->size(); j<right->size() + (i+2)%right->size(); j++)
                if(spot2d[right->coord[j%right->size()]] != spot2d[right->coord[i]] and
                   spot2d[right->coord[j%right->size()]] != spot2d[right->coord[(i+1)%right->size()]])
                {
                   opositePoint = spot2d[right->coord[j%right->size()]];
                   break;
                }
            if(firstSide != rightSide(spot2d[right->coord[i]], spot2d[right->coord[(i+1)%right->size()]], opositePoint))
                return false;
        }
    }

    return true;
}

std::pair<bool, bool> Camera::wallSortingAlgorythm(Wall* lhs, Wall* rhs)
{
    std::vector <int> leftWall;
    std::vector <int> rightWall;
    for(int i=0; i<lhs->size(); i++)
    {
        int value = planeSide(spot3d[rhs->coord[0]], spot3d[rhs->coord[1]], spot3d[rhs->coord[2]], spot3d[lhs->coord[i]]);
        if(value != 0)
            leftWall.push_back(value);
    }
    if(leftWall.empty())
        return {false, false};
    for(int i=0; i<rhs->size(); i++)
    {
        int value = planeSide(spot3d[lhs->coord[0]], spot3d[lhs->coord[1]], spot3d[lhs->coord[2]], spot3d[rhs->coord[i]]);
        if(value != 0)
            rightWall.push_back(value);
    }
    if(rightWall.empty())
        return {false, false};

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
        if(leftWall[0] == 1)
            leftWallFirst = true;
        else
            rightWallFirst = true;
    if(rightAllOnOneSide)
        if(rightWall[0] == 1)
            rightWallFirst = true;
        else
            leftWallFirst = true;

    if(leftWallFirst != rightWallFirst)
        return {leftWallFirst, rightWallFirst};
    else if(distanceCheckEnabled and !leftWallFirst)
    {
        sf::Vector3f leftCenter;
        sf::Vector3f rightCenter;
        for(int i=0; i<lhs->size(); i++)
            leftCenter += spot3d[lhs->coord[i]];
        leftCenter /= float(lhs->size());
        leftCenter -= vEye;
        for(int i=0; i<rhs->size(); i++)
            rightCenter += spot3d[rhs->coord[i]];
        leftCenter /= float(lhs->size());
        leftCenter -= vEye;
        if( leftCenter.x* leftCenter.x +  leftCenter.y* leftCenter.y +  leftCenter.z* leftCenter.z <
           rightCenter.x*rightCenter.x + rightCenter.y*rightCenter.y + rightCenter.z*rightCenter.z)
            return {true, false};
        else
            return {false, true};
    }
    return {false, false};
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
    graph.resize(tempOrder.size());
    for(int i=0; i<tempOrder.size(); i++)
        for(int j=i+1; j<tempOrder.size(); j++)
            if(!wallIntersectEnabled or wallIntersect(tempOrder[i], tempOrder[j]))
            {
                std::pair <bool, bool> order = wallSortingAlgorythm(tempOrder[i], tempOrder[j]);
                if(order.first)
                {
                    graph[j].push_back(i);
                    graphLevel[i]++;
                }
                if(order.second)
                {
                    graph[i].push_back(j);
                    graphLevel[j]++;
                }
            }
}

void Camera::printGraph()
{
    displayGraph(wallSortGraph, wallSortGraphLevel);
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

            ///##############################
            int pSide = 0;
            pSide = planeSide(spot3d[wallie[j]->coord[0]], spot3d[wallie[j]->coord[1]], spot3d[wallie[j]->coord[2]], vEye);
            ///##############################

            sf::Text t;
            int characterSize = 30; //roundf(dotSize(300, centerPosition3d));
            if(characterSize > 0)
            {
                if(characterSize > 50)
                    characterSize = 50;
                t.setCharacterSize(characterSize);
                t.setFont(Common::Font::Carnevalee_Freakshow);
                t.setOutlineThickness(2);
                if(pSide == 1)
                    t.setColor(sf::Color::Blue);
                if(pSide == -1)
                    t.setColor(sf::Color::Red);
                t.setString(stringify(j));
                //t.setString(stringify(pSide));
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
    if(counter < tempOrder.size() and !topologicalError and sinceLastTopologicalError > 5000.0d)
    {
        std::cout << "\nERROR: Topological sort failure";
        p->printToConsole("[error] Topological sort failure");
        sinceLastTopologicalError = 0.0d;
        topologicalError = true;
    }
    else
    {
        sinceLastTopologicalError += p->dt;
    }
    if(counter == tempOrder.size())
    {
        topologicalError = false;
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
    for(int i=0; i<graph.size(); i++)
    {
        cmdOutput += "\nWall " + stringify(i) + " results:";
        for(int j=0; j<graph[i].size(); j++)
            cmdOutput += " " + stringify(graph[i][j]);
    }
}

void Camera::findCycle(std::vector <std::vector <int> >& graph, std::vector <int>& graphLevel)
{
    for(int i=0; i<graph.size(); i++)
    {
        bool visited[graph.size()];
        memset(visited, 0, sizeof(bool) * graph.size());
        std::stack <int> s;
        s.push(i);
        while(!s.empty())
        {
            int n = s.top();
            s.pop();
            for(int j=0; j<graph[n].size(); j++)
            {
                if(!visited[graph[n][j]])
                {
                    s.push(graph[n][j]);
                    visited[graph[n][j]] = true;
                }
                if(graph[n][j] == i)
                {
                    p->printToConsole("Found cycle starting: " + stringify(i), sf::Color::Cyan);
                }
            }
        }
    }
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
            if(activeWall(&(p->world[i].wallie[j])))
                tempOrder.push_back(&(p->world[i].wallie[j]));
        }
    std::vector <int> graphLevel;
    graphLevel.resize(tempOrder.size());

    if(!randomSortEnabled)
    {
        createGraph(graph, graphLevel, tempOrder);

        if(cycleReductionEnabled)
        {
            cycleReduction(graph, graphLevel);
        }

        wallSortGraph = graph;
        wallSortGraphLevel = graphLevel;

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
    calcTerrain();
    wallSort();
    sf::Vector3f place = sf::Vector3f(100, 50, 300);
    //place = vecTransform(place);
    place = vecTransform(place);
    drawSphere(place, 10, sf::Color::Magenta);


    for(int i=0; i<wallOrder.size(); i++)
        drawWall(*wallOrder[i]);

    /*
    place = vecTransform(p->character[0].ellipsoidCenter - position);
    sf::Color color = sf::Color::Yellow;
    color.a = 120;
    drawSphere(place, 80, color);
    */


    p->window->clear();
    p->window->draw(&quadArray[0], quadArray.size(), sf::Quads);
    p->window->draw(&debugArray[0], debugArray.size(), sf::Quads);
    for(int i=0; i<textArray.size(); i++)
        p->window->draw(textArray[i]);
    quadArray.clear();
    debugArray.clear();
    textArray.clear();
}

Entity::Entity(SYNTH3D* gameBase):
    position({0,0,0}), directionSet(false), tick(1), offsetLimit(15.0f),
    sinx(0.0f), siny(0.0f), sinz(0.0f),
    cosx(1.0f), cosy(1.0f), cosz(1.0f),
    offset(0.0f), dt(0), veryCloseDistanceVelocity(0.0005f),
    veryCloseDistanceGravity(0.005f),
    velocityVector({0, 0, 0}), gravityVector({0, 0, 0}),
    speedVector({0, 0, 0}), gravitySpeedVector({0, 0, 0}),
    maxSpeed(15.0f), maxSpeedSquared(225.0f),
    standingOnSurface(false), velocitySet(false)
{
    synth = gameBase;
}

void Entity::collideAndSlide(const sf::Vector3f& vel, const sf::Vector3f& gravity)
{
    collisionPackage.R3Position = position;
    collisionPackage.R3Position.y += rotationCenter.y;
    collisionPackage.R3Velocity = vel;
    sf::Vector3f eSpacePosition = collisionPackage.R3Position / collisionPackage.eRadius;
    sf::Vector3f eSpaceVelocity = collisionPackage.R3Velocity / collisionPackage.eRadius;
    collisionRecursionDepth = 0;
    sf::Vector3f finalPosition = collideWithWorld(eSpacePosition, eSpaceVelocity, veryCloseDistanceVelocity);

    collisionPackage.R3Position = finalPosition*collisionPackage.eRadius;
    collisionPackage.R3Velocity = gravity;
    eSpaceVelocity = gravity/collisionPackage.eRadius;
    collisionRecursionDepth = 0;
    ///collisionPackage.foundCollision = false;
    finalPosition = collideWithWorld(finalPosition, eSpaceVelocity, veryCloseDistanceGravity);
    if(collisionPackage.foundCollision)
        standingOnSurface = true;

    finalPosition = finalPosition * collisionPackage.eRadius;
    finalPosition.y -= rotationCenter.y;

    position = finalPosition;
}

sf::Vector3f Entity::collideWithWorld(const sf::Vector3f& pos, const sf::Vector3f& vel, float veryCloseDistance)
{
    if (collisionRecursionDepth>5)
        return pos;
    collisionPackage.velocity = vel;
    collisionPackage.normalizedVelocity = vel;
    normalize3f(collisionPackage.normalizedVelocity);
    collisionPackage.basePoint = pos;
    collisionPackage.foundCollision = false;
    synth->checkCollision(&collisionPackage, element[0], element[element.size() - 1]);
    if (collisionPackage.foundCollision == false)
    {
        return pos + vel;
    }
    sf::Vector3f destinationPoint = pos + vel;
    sf::Vector3f newBasePoint = pos;
    if (collisionPackage.nearestDistance >= veryCloseDistance)
    {
        sf::Vector3f V = vel;
        vecSetLength(V, collisionPackage.nearestDistance - veryCloseDistance);
        newBasePoint = collisionPackage.basePoint + V;
        normalize3f(V);
        //collisionPackage.intersectionPoint = collisionPackage.intersectionPoint - veryCloseDistance * V;
        collisionPackage.intersectionPoint -= veryCloseDistance * V;
    }
    sf::Vector3f slidePlaneOrigin = collisionPackage.intersectionPoint;
    sf::Vector3f slidePlaneNormal = newBasePoint - collisionPackage.intersectionPoint;
    normalize3f(slidePlaneNormal);
    Plane slidingPlane(slidePlaneOrigin, slidePlaneNormal);
    sf::Vector3f newDestinationPoint = destinationPoint -
        float(slidingPlane.signedDistanceTo(destinationPoint)) * slidePlaneNormal;
    sf::Vector3f newVelocityVector = newDestinationPoint - collisionPackage.intersectionPoint;
    if (vecLength(newVelocityVector) < veryCloseDistance)
    {
        return newBasePoint;
    }
    collisionRecursionDepth++;
    return collideWithWorld(newBasePoint, newVelocityVector, veryCloseDistance);
}

void Entity::setAcceleration(sf::Vector3f acceleration)
{
    velocityVector += acceleration;
    velocitySet = true;
}

void Entity::setGravity(sf::Vector3f acceleration)
{
    gravityVector = acceleration;
}

void Entity::moveLeft(float speed)
{

}

void Entity::moveRight(float speed)
{

}

void Entity::moveForward(float speed)
{

}

void Entity::moveBackward(float speed)
{

}

void Entity::jump(float speed)
{
    if(standingOnSurface)
        gravitySpeedVector = sf::Vector3f(0, speed, 0);
}

void Entity::calcVelocity(sf::Vector3f& velocity)
{
    if(standingOnSurface)
    {
        sf::Vector3f deleteSpeed = speedVector;
        normalize3f(deleteSpeed);
        deleteSpeed *= 0.7f * float(dt);
        if(vecSquaredLength(speedVector) > vecSquaredLength(deleteSpeed))
            speedVector -= deleteSpeed;
        else
            speedVector = sf::Vector3f(0, 0, 0);

        if(velocitySet)
        {
            if(speedVector.x*speedVector.x + speedVector.z*speedVector.z <= maxSpeedSquared)
            {
                velocity = (speedVector + velocityVector);
                if(velocity.x*velocity.x + velocity.z*velocity.z > maxSpeedSquared)
                {
                    float length = vecLength(velocity);
                    velocity.x *= (maxSpeed / length);
                    velocity.z *= (maxSpeed / length);
                }
                velocity *= float(dt);
            }
            else
            {
                velocity = speedVector * float(dt);
            }
        }
        else
        {
            velocity = speedVector * float(dt);
        }
    }
    else
    {
        velocity = speedVector * float(dt);
    }
    standingOnSurface = false;
    velocitySet = false;
}

void Entity::update(double deltaTime)
{
    dt = deltaTime;
    positionBuffor = position;
    sf::Vector3f velocity;
    sf::Vector3f gravity;

    calcVelocity(velocity);
    gravity = (gravitySpeedVector + gravityVector) * float(dt);

    velocityVector = sf::Vector3f(0, 0, 0);
    gravityVector =  sf::Vector3f(0, 0, 0);

    collideAndSlide(velocity, gravity);

    speedVector = sf::Vector3f(position.x - positionBuffor.x, 0, position.z - positionBuffor.z) / float(dt);
    gravitySpeedVector = sf::Vector3f(0, position.y - positionBuffor.y, 0) / float(dt);

    if(!directionSet)
    {
        if(position != positionBuffor)
        {
            setDirection(position, positionBuffor);
            calcAngle();
        }
    }
    else
    {
        directionSet = false;
        calcAngle();
    }
    movement();
    for(int i=0; i<element.size(); i++)
        for(int j=0; j<synth->world[element[i]].size(); j++)
            for(int k=0; k<synth->world[element[i]].wallie[j].size(); k++)
            {
                sf::Vector3f flatRotationCenter = {rotationCenter.x, 0, rotationCenter.z};
                synth->terrain[synth->world[element[i]].wallie[j].coord[k]] = vecTransform(synth->defTerrain[synth->world[element[i]].wallie[j].coord[k]]);
                synth->terrain[synth->world[element[i]].wallie[j].coord[k]] += position - flatRotationCenter;
                for(int l=0; l<synth->world[element[i]].wallie[j].lineStrip.size(); l++)
                    for(int m=0; m<synth->world[element[i]].wallie[j].lineStrip[l].size(); m++)
                    {
                        synth->terrain[synth->world[element[i]].wallie[j].lineStrip[l][m]] = vecTransform(synth->defTerrain[synth->world[element[i]].wallie[j].lineStrip[l][m]]);
                        synth->terrain[synth->world[element[i]].wallie[j].lineStrip[l][m]] += position - flatRotationCenter;
                    }
            }
    ellipsoidCenter = position;
    ellipsoidCenter.y += rotationCenter.y;
}

void Entity::importObject(int newElement)
{
    element.push_back(newElement);
}

void Entity::setDirection(sf::Vector3f vec, sf::Vector3f beginning)
{
    origin = beginning;
    direction = vec;
    directionSet = true;
}

void Entity::setPos(sf::Vector3f vec)
{
    positionBuffor = vec;
    position = vec;
}

void Entity::setSpeed(sf::Vector3f vec)
{
    speedVector = sf::Vector3f(vec.x, 0, vec.z);
    gravitySpeedVector = sf::Vector3f(0, vec.y, 0);
}

void Entity::setRotationCenter(sf::Vector3f vec)
{
    rotationCenter = vec;
}

void Entity::setEllipsoidRadius(sf::Vector3f radius)
{
    collisionPackage.eRadius = radius;
}

void Entity::movement()
{
    if(position != positionBuffor)
    {
        sf::Vector3f movementVec = position - positionBuffor;
        float distance = sqrtf(movementVec.x*movementVec.x + movementVec.y*movementVec.y + movementVec.z*movementVec.z);
        offset += distance*tick;
        if(offset > offsetLimit)
        {
            tick = -tick;
            offset = offsetLimit - fmodf(offset, offsetLimit);
        }
        else if(offset < -offsetLimit)
        {
            tick = -tick;
            offset = -offsetLimit - fmodf(offset, -offsetLimit);
        }
    }
    else
    {
        if(offset > 0)
        {
            if(offset < dt)
                offset = 0;
            else
                offset-= dt;
        }
        else
        {
            if(offset > -dt)
                offset = 0;
            else
                offset+= dt;
        }
    }
    for(int i=0; i<leftLeg.size(); i++)
        synth->defTerrain[leftLeg[i].first] = leftLeg[i].second + sf::Vector3f(0, 0, offset);
    for(int i=0; i<rightLeg.size(); i++)
        synth->defTerrain[rightLeg[i].first] = rightLeg[i].second - sf::Vector3f(0, 0, offset);
}

void Entity::calcAngle()
{
    ///CIALO OBRACA SIE TYLKO W PLASZCZYZNIE BEZ Y, GLOWA EWENTUALNIE BARDZIEJ
    //dirVec = direction - position;
    dirVec = direction - origin;
    float length = sqrtf(dirVec.x*dirVec.x + dirVec.z*dirVec.z);
    siny = dirVec.x / length;
    cosy = dirVec.z / length;
    /*
    float lengthX = sqrtf(dirVec.y*dirVec.y + dirVec.z*dirVec.z);
    float lengthY = sqrtf(dirVec.x*dirVec.x + dirVec.z*dirVec.z);
    float lengthZ = sqrtf(dirVec.x*dirVec.x + dirVec.y*dirVec.y);
    sinx = dirVec.z / lengthX;
    cosx = dirVec.y / lengthX;
    siny = dirVec.z / lengthY;
    cosy = dirVec.x / lengthY;
    sinz = dirVec.x / lengthZ;
    cosz = dirVec.y / lengthZ;
    */
}

void Entity::calcRotationCenter()
{
    if(!element.empty() and synth->world[element[0]].size() > 0 and synth->world[element[0]].wallie[0].size() > 0)
    {
        sf::Vector3f first = synth->defTerrain[synth->world[element[0]].wallie[0].coord[0]];
        float minx = first.x, maxx = first.x;
        float miny = first.y, maxy = first.y;
        float minz = first.z, maxz = first.z;
        for(int i=0; i<element.size(); i++)
            for(int j=0; j<synth->world[element[i]].size(); j++)
                for(int k=0; k<synth->world[element[i]].wallie[j].size(); k++)
                {
                    sf::Vector3f temp = synth->defTerrain[synth->world[element[i]].wallie[j].coord[k]];
                    maxx = std::max(maxx, temp.x);
                    minx = std::min(minx, temp.x);
                    maxy = std::max(maxy, temp.y);
                    miny = std::min(miny, temp.y);
                    maxz = std::max(maxz, temp.z);
                    minz = std::min(minz, temp.z);
                }
        rotationCenter = {(maxx+minx)/2, (maxy+miny)/2, (maxz+minz)/2};
    }
}

sf::Vector3f Entity::vecTransform(sf::Vector3f vec)
{
    vec -= rotationCenter;
    sf::Vector3f target = vec;
    //Yaw: (y axis)
    target.x = -vec.x*cosy - vec.z*siny;
    target.z = -vec.z*cosy + vec.x*siny;
    /*
    //Pitch; (x axis)
    vec = target;
    target.y = vec.y*cosx - vec.z*sinx;
    target.z = vec.y*sinx + vec.z*cosx;
    //Roll: (z axis)
    vec = target;
    target.x = vec.x*cosz - vec.y*sinz;
    //target.y = vec.x*sinz + vec.y*cosz;
    target.y = -vec.x*sinz - vec.y*cosz;
    */
    target += rotationCenter;
    return target;
}


#endif //SYNTH3D_HPP
