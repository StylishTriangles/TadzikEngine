#pragma once
#include <Windows.h>
#include <cmath>
#include <fstream>
#include "display.h"
namespace Tadzik
{
struct Object
{
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
    void io();
    void input();
    void logic(Object& ob);
    void processing(Object &ob);
    HANDLE hStdOut;
    Tadzik::Display display;
    std::pair <short, short> dimensions; // XY dimensions
    std::vector<std::wstring> mapChar; // Nie umiem ogarnac czytywania unicode jescze
    std::vector<std::vector<BYTE> > mapColor;
    std::vector<std::vector<unsigned short> > mapStruct;
    static const constexpr double g=10; // m/s^2
    static const constexpr double dt=0.03; // milliseconds
    short aKey,dKey,wKey,sKey;
    Object player;
    unsigned short x_init,y_init; // Wczytana wielkosc mapy
};

Game::Game() :
    hStdOut(GetStdHandle(STD_OUTPUT_HANDLE)),
    dimensions({80,25}),
    player ({5,5,0,0})
{
    display.resize(dimensions.first,dimensions.second);
    io();
}

void Game::io()
{
    //Wczytywanie 3 map (ta co wrzucilem to po prostu kopia z mojej gry, mozna jo na start przerobic
    ifstream myFile;
    myFile.open("mapa.txt");
    myFile >> x_init;
    myFile >> y_init;
    mapColor.resize(y_init);
    mapStruct.resize(y_init);
    char line[x_init];
    for(int lineNumber = 0; lineNumber < y_init; lineNumber++)
    {
        inFile.getline(line,x_init);
        for (int i=0; i<x_init; i++)
            mapStruct[lineNumber].push_back((unsigned short)line[i]-48); // Wczytywanie Structa z charu na inta
    }
    for(int lineNumber = 0; lineNumber < y_init; lineNumber++)
    {
        inFile.getline(line,x_init);
        for (int i=0; i<x_init; i++)
            mapColor[lineNumber].push_back((BYTE)line[i]); // Nie wiem czym som kolory i jak je ogarniac
    }
    myFile.close();
}

void Game::input()
{
    dKey=GetAsyncKeyState(0x44);  // Klawisz d
    aKey=GetAsyncKeyState(0x41);
    wKey=GetAsyncKeyState(0x57);
    sKey=GetAsyncKeyState(0x53);
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

void Game::processing(Object &ob)
{
    if(aKey<0 and ob.vx>=-5)
        ob.vx=ob.vx-0.1;
    if(dKey<0 and ob.vx<=5)
        ob.vx=ob.vx+0.1; //max 5m/s
    if(wKey<0 and ob.posy == static_cast<int> (ob.posy) and mapStruct[static_cast<int>(ob.posy)-1][static_cast<int>(ob.posx)] == 1)
        ob.vy=ob.vy+20;  //czy wsisnienty | czy calkowity | czy ponizej jest ziemia
}

void Game::onTick()
{
    //wczytaj klawisze
    input();
    //logika
    processing(player);
    logic(player);
    //jak dodamy npc etc to tu sie doda jakoms kolejke obiektow i wykona na nich logike

    //display

    display.render();
}

} // namespace Tadzik
