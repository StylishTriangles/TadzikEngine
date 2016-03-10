#pragma once
#include <Windows.h>
#include <cmath>
#include "display.h"
namespace Tadzik
{
struct Object
{
    int x;
    int y;
    double posx;
    double posy;
    double vx;
    double vy;
};

class Game
{
public:
    Game();
    virtual void onTick();
protected:
    void logic(Object& ob);
    HANDLE hStdOut;
    Tadzik::Display display;
    std::pair <short, short> dimensions; // XY dimensions
    std::vector<std::wstring> mapChar;
    std::vector<std::vector<BYTE> > mapColor;
    std::vector<std::vector<unsigned short> > mapStruct;
    static const constexpr double g=10; // m/s^2
    static const constexpr double dt=0.03; // milliseconds
};

Game::Game() :
    hStdOut(GetStdHandle(STD_OUTPUT_HANDLE)),
    dimensions({80,25})
{
    display.resize(dimensions.first,dimensions.second);
}

void Game::logic(Object &ob)
{
    double bufy,bufx;
    bufy=ob.posy;
    ob.posy+=ob.vy*dt;
    if(mapStruct[static_cast<int>(ob.posy)][static_cast<int>(ob.posx)] == 1)
    {
        ob.vy=0;
        ob.posy=static_cast<int>(bufy);
    }
    bufx=ob.posx;
    ob.posx+=ob.vx*dt;
    if(mapStruct[static_cast<int>(ob.posy)][static_cast<int>(ob.posx)] == 1)
    {
        ob.vx=0;
        ob.posy=static_cast<int>(bufx);
    }
    ob.vy+=g*dt;
}

void Game::onTick()
{
    //wczytaj klawisze

    //logika

    //display
    display.render();
}

} // namespace Tadzik
