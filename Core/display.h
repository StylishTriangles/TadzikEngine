#pragma once
#include <iostream>
#include <stdio.h>
#include <cwchar>
#include <windows.h>
#include <vector>
#include <string>
#include <cinttypes>
//#include <cassert>
#include <stdexcept>
namespace Tadzik
{
class Display
{
public:
    Display();
    inline void operator () () {this->render();}
    void resize(int x, int y);
    void addText(wchar_t* txt, short length, COORD pos, BYTE color);
    void updateFromMap(std::vector<std::wstring> const& rMap, std::vector<std::vector<BYTE> > const& rColorMap, COORD anchor);
    void render();

protected:
    inline void color(BYTE col) {SetConsoleTextAttribute (hStdOut, col);}
    std::vector<std::wstring> bufferOld;
    std::vector<std::wstring> buffer;
    std::vector<std::vector<BYTE> > bufferColorOld;
    std::vector<std::vector<BYTE> > bufferColor;
    std::pair<int, int> bufferSize;
    int bufferWidth,bufferHeight;
    HANDLE hStdOut;
};
Display::Display():
    hStdOut(GetStdHandle(STD_OUTPUT_HANDLE))
{}

void Display::addText(wchar_t *txt, short length, COORD pos, BYTE color)
{
    if (pos.Y >= (signed)buffer.size())
        throw std::out_of_range("Display::text, pos.Y out of range");
    for (int i = 0; i < length; i++)
    {
        buffer[pos.Y].at(pos.X+i) = *txt;
        ++txt;
        bufferColor[pos.Y].at(pos.X+i) = color;
    }
}

void Display::updateFromMap(std::vector<std::wstring> const& rCharMap, const std::vector<std::vector<BYTE> > &rColorMap , COORD anchor)
{
    for (int i = 0; i < bufferHeight; i++)
    {
        buffer[i] = rCharMap[anchor.Y+i].substr(anchor.X,bufferWidth);
    }
    for (int i = 0; i < bufferHeight; i++)
    {
        for (int j = 0; j < bufferWidth; j++)
        {
            bufferColor[i][j] = rColorMap[i+anchor.Y][j+anchor.X];
        }
    }
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
                //WriteConsoleW(hStdOut,&buffer[i][j],1,NULL,NULL);
                WriteConsoleOutputCharacterW(hStdOut,&buffer[i][j],1,{(SHORT)j,(SHORT)i},NULL);
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
    bufferWidth = x;
    bufferHeight = y;
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
} // Tadzik EOF
