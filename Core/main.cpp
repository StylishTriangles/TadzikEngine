#include <iostream>
#include <Windows.h>
#include <clocale>
#include <locale>
#include "display.h"
#include "io.h"

using namespace std;

int main()
{
    setlocale(LC_ALL,"Polish_Poland.1250");
    const char* c = u8"ł";
    wchar_t w[23] = L"ąęćżźł♥";
    char32_t c32[10] = U"ł";
    cout << c << endl;
    wcout << w << endl;
    wcout << c32 << endl;
    Tadzik::IO io;
    io.open(L"kappa");
    return 0;
}

