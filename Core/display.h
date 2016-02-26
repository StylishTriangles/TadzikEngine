#pragma once
#include <iostream>

namespace tadzik
{
class display
{
public:
    display();
    void operator () ();

protected:

};

void display::operator ()()
{
    std::cout << "kappa";
}
}
