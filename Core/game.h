#include <Windows.h>
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
    void logic(Object ob);
protected:
    Tadzik::Display display;
    std::pair <short, short> dimensions;
    std::vector<std::wstring> mapChar;
    std::vector<std::vector<BYTE> > mapColor;
    std::vector<std::vector<unsigned short> > mapStruct;
    double g=10;
    double dt=0.03;
};

Game::Game() :
    dimensions({80,25}),
    hStdOut(GetStdHandle(HSTDOUT))
{
    display.resize()
}

void logic(Object ob)
{
    double bufy,bufx;
    bufy=ob.posy;
    ob.posy+=ob.vy*dt;
    if(mapStruct[int(ob.posy)][int(ob.posx)] == 1)
    {
        ob.vy=0;
        ob.posy=int(bufy);
    }
    bufx=ob.posx;
    ob.posx+=ob.vx*dt;
    if(mapStruct[int(ob.posy)][int(ob.posx)] == 1)
    {
        ob.vx=0;
        ob.posy=int(bufx);
    }
    ob.vy+=g*dt;
}

void Game::onTick()
{
    //wczytaj klawisze

    //logika

    //display
    display();
}
}
