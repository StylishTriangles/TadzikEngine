#pragma once
#include <iostream>
#include <stdio.h>
#include <cwchar>
#include <windows.h>
#include <vector>
#include <string>
namespace Tadzik
{
class Display
{
public:
    Display();
    inline void operator () () {this->render();}
    void resize(int x, int y);
    void setText(int x, int y, char* txt, char fgcolor, char bgcolor);
    void color(char col);
    void render();

protected:
    std::vector<std::wstring> bufferOld;
    std::vector<std::wstring> buffer;
    std::vector<std::wstring> bufferColorOld;
    std::vector<std::wstring> bufferColor;
    HANDLE hStdOut;
};
Display::Display():
    hStdOut(GetStdHandle(STD_OUTPUT_HANDLE))
{}

void Display::color(char col)
{
    SetConsoleTextAttribute (hStdOut, col);
}

void Display::render()
{
    for(unsigned int i=0; i<buffer.size(); i++)
    {
        for(unsigned int j=0; j<buffer[0].size(); j++)
        {
            if(buffer[i][j] != bufferOld[i][j] or bufferColor[i][j] != bufferColorOld[i][j])
            {
                color(bufferColor[i][j]);
                WriteConsoleW(hStdOut,&buffer[i][j],1,NULL,NULL);
                //putwc(buffer[i][j], stdout);
                bufferOld[i][j]=buffer[i][j];
                bufferColorOld[i][j]=bufferColor[i][j];
            }
        }
    }


//    int length=x_init-79;
//    for(int str=1;str<length;str++)
//        for(int y=0;y<25;y++)
//            for(int x=str-1;x<79+str;x++)
//                if(map_1[x][y]!=map_1[x+1][y])
//                    forward_map[str].push_back({x, y});
}
void Display::resize(int x, int y)
{
    buffer.resize(y);
    bufferOld.resize(y);
    bufferColor.resize(y);
    bufferColorOld.resize(y);
    for (int i = 0; i < y; i++)
    {
        buffer[i].resize(x);
        bufferOld[i].resize(x);
        bufferColor[i].resize(x);
        bufferColorOld[i].resize(x);
    }
}
}
