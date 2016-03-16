#pragma once
#include <Windows.h>
#include <cmath>
#include <fstream>
#include <ctime>
#include <chrono>
#include <thread>
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

std::wstring byteToWide(const char* in)
{
    std::wstring out;
    while (*in != '\0')
    {
        out.push_back((int)*in);
        ++in;
    }
    return out;
}

class Game
{
public:
    Game();
    virtual void run();
    virtual void onTick();
protected:
    void loadLevelA();
    void loadLevelW();
    void input();
    void logic(Object &ob);
    void processing(Object &ob);

    HANDLE hStdOut;
    SHORT keyState[0xFF];
    Tadzik::Display display;
    unsigned mapWidth, mapHeight;
    std::pair <short, short> windowSize;
    std::vector<std::wstring> mapChar; // Nie umiem ogarnac czytywania unicode jescze
    std::vector<std::vector<BYTE> > mapColor;
    std::vector<std::vector<unsigned short> > mapStruct;
    Object player;
    long long msTickDelay; // delay in milliseconds
    static const constexpr double g=10; // m/s^2
    static const constexpr double dt=0.03; // seconds
};

Game::Game() :
    hStdOut(GetStdHandle(STD_OUTPUT_HANDLE)),
    windowSize({80,25}),
    player ({5,5,0,0}),
    msTickDelay(100)
{
    display.resize(windowSize.first,windowSize.second);
    loadLevelW();
}

void Game::loadLevelA()
{
    //Wczytywanie 3 map (ta co wrzucilem to po prostu kopia z mojej gry, mozna jo na start przerobic
    std::ifstream myFile;
    myFile.open("mapa.txt");
    myFile >> mapWidth;
    myFile >> mapHeight;
    mapColor.resize(mapHeight);
    mapStruct.resize(mapHeight);
    char line[mapWidth+1];
    for(unsigned lineNumber = 0; lineNumber < mapHeight; lineNumber++)
    {
        myFile.getline(line,mapWidth);
        for (unsigned i=0; i<mapWidth; i++)
            mapStruct[lineNumber].push_back(static_cast<unsigned short>(line[i]-'0')); // Wczytywanie Structa z charu na inta // CHANGE IT!1!
    }
//    for(int lineNumber = 0; lineNumber < mapHeight; lineNumber++)
//    {
//        myFile.getline(line,mapWidth);
//        for (int i=0; i<mapWidth; i++)
//            mapColor[lineNumber].push_back((BYTE)line[i]); // Nie wiem czym sa kolory i jak je ogarniac
//    }
    myFile.close();
}

void Game::loadLevelW()
{
    std::fstream myFile;
    myFile.open("nowaMapa.txt");
    myFile >> mapWidth;
    myFile >> mapHeight;
    mapChar.resize(mapHeight);
    mapColor.resize(mapHeight);
    mapStruct.resize(mapHeight);
    char line[mapWidth+1];
    for(unsigned lineNumber = 0; lineNumber < mapHeight; lineNumber++) // wczytywanie tekstur
    {
        myFile.getline(line,mapWidth,'\n');
        mapChar[lineNumber] = byteToWide(line);
    }
    for(unsigned lineNumber = 0; lineNumber < mapHeight; lineNumber++) // wczytywanie kolorow
    {
        unsigned short tmp = 0;
        mapColor[lineNumber].resize(mapWidth);
        for (unsigned columnNumber = 0; columnNumber < mapWidth; columnNumber++)
        {
            myFile >> tmp;
            mapColor[lineNumber][columnNumber] = static_cast<BYTE>(tmp);
        }
    }
    for(unsigned lineNumber = 0; lineNumber < mapHeight; lineNumber++) // wczytywanie dodatkowych danych
    {
        unsigned short tmp = 0;
        mapStruct[lineNumber].resize(mapWidth);
        for (unsigned columnNumber = 0; columnNumber < mapWidth; columnNumber++)
        {
            myFile >> tmp;
            mapStruct[lineNumber][columnNumber] = tmp;
        }
    }
    myFile.close();
    std::cout << "Wczytano" << std::endl;
}

void Game::input()
{
    keyState[0x57]=GetAsyncKeyState(0x57);  // Klawisz W
    keyState[0x41]=GetAsyncKeyState(0x41);  // Klawisz A
    keyState[0x53]=GetAsyncKeyState(0x53);  // Klawisz S
    keyState[0x44]=GetAsyncKeyState(0x44);  // Klawisz D
}

void Game::logic(Object &ob)
{
    double bufy,bufx;
    bufy=ob.posy;
    ob.posy+=ob.vy*dt;
    if(mapStruct[static_cast<int>(ob.posy)][static_cast<int>(ob.posx)] == 1)
    {
        ob.vy=0;
        ob.posy=static_cast<int>(floor(bufy));
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
    if(keyState[0x41]<0 and ob.vx>=-5) // A
        ob.vx=ob.vx-0.1;
    if(keyState[0x44]<0 and ob.vx<=5)  // D
        ob.vx=ob.vx+0.1; //max 5m/s
    if(keyState[0x57]<0 and ob.posy == static_cast<int> (ob.posy) and mapStruct[static_cast<int>(ob.posy)-1][static_cast<int>(ob.posx)] == 1)
        ob.vy=ob.vy+20;  //czy wsisnienty W | czy calkowity | czy ponizej jest ziemia
}

void Game::run()
{
    while(1)
    {
        auto tBegin = std::chrono::high_resolution_clock::now();
        this->onTick();
        auto tEnd = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::microseconds>(tEnd-tBegin);
        auto tRemaining = std::chrono::microseconds(msTickDelay*1000) - time;
        if (tRemaining.count() > 0)
            std::this_thread::sleep_for(tRemaining);
        else
        {
            std::cout << "Timeout!" << std::endl;
            system("pause");
        }
    }
}

void Game::onTick()
{
    //wczytaj klawisze
    input();
    //logika
    processing(player);
    logic(player);
    display.updateFromMap(mapChar,mapColor,{0,0});
    //jak dodamy npc etc to tu sie doda jakas kolejke obiektow i wykona na nich logike

    //display
    display.render();
}

} // namespace Tadzik
