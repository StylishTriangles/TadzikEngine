#include <Windows.h>
namespace Tadzik
{
class Game
{
public:
    Game();
    virtual void onTick();
protected:
    Tadzik::Display display;
    std::pair <short, short> dimensions;
};

Game::Game() :
    dimensions({80,25}),
    hStdOut(GetStdHandle(HSTDOUT))
{
    display.resize()

}

void Game::onTick()
{
    //wczytaj klawisze

    //
    display();
}
}
