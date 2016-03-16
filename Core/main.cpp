#include <iostream>
#include <conio.h>
#include <fstream>
#include <windows.h>
#include <vector>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <cmath>
#include <unistd.h>
#include "game.h"
#include "display.h"
using namespace std;
/*
struct pixel
{
    int x;
    int y;
};

vector <vector <int> > map_1;
vector <vector <pixel> > forward_map;
vector <vector <pixel> > backward_map;
int x_init, y_init, buf_start, bufx, bufy, start;
const int Size=384;
char t[Size];
double g;


void setcolor(char tc, char bc)
{
    SetConsoleTextAttribute (GetStdHandle (STD_OUTPUT_HANDLE), tc+(16*bc));
}

void gotoxy(int x, int y)
{
    COORD c= {x,y};
    SetConsoleCursorPosition (GetStdHandle (STD_OUTPUT_HANDLE), c);
}

class Player
{
    public:
        double x;
        double y;
        double vx;
        double vy;
        void setPos(double, double);
        void display_1(int);
        void display_2(int);
};

void Player::setPos(double x1, double y1)
{
    x=x1;
    y=y1;
}
void init_display_1(int start)
{
    for(int y=0;y<25;y++)
    {
        gotoxy(0,y);
        for(int x=start;x<80+start;x++)
        {
            setcolor(map_1[x][y],map_1[x][y]);
            putc(' ', stdout);
        }
    }
    gotoxy(0,0);
}

void display_1(int start)
{
    int color;
    if(start>buf_start)
        for(int i=0;i<forward_map[start].size();i++)
        {
            gotoxy(forward_map[start][i].x-start+1,forward_map[start][i].y);
            color=map_1[forward_map[start][i].x+1][forward_map[start][i].y];
            setcolor(color,color);
            putc(' ', stdout);
        }
    else if(start<buf_start)
        for(int i=0;i<backward_map[start].size();i++)
        {
            gotoxy(backward_map[start][i].x-start-1,backward_map[start][i].y);
            color=map_1[backward_map[start][i].x-1][backward_map[start][i].y];
            setcolor(color,color);
            putc(' ', stdout);
        }
    buf_start=start;
}

void Player::display_1(int start)
{
    int color;
    gotoxy(bufx,bufy);
    color=map_1[round(x)][round(y-1)];
    setcolor(color,color);
    putc(' ', stdout);
    color=12;
    gotoxy(round(x)-start,round(y));
    setcolor(color,color); // Napraw Y
    putc(' ', stdout);
    bufx=round(x)-start;
    bufy=round(y);
}

void ForwardAnalysis()
{
    int length=x_init-79;
    for(int str=1;str<length;str++)
        for(int y=0;y<25;y++)
            for(int x=str-1;x<79+str;x++)
                if(map_1[x][y]!=map_1[x+1][y])
                    forward_map[str].push_back({x, y});
}

void BackwardAnalysis()
{
    for(int str=x_init-81;str>=0;str--)
        for(int y=0;y<25;y++)
            for(int x=str+1;x<81+str;x++)
                if(map_1[x][y]!=map_1[x-1][y])
                    backward_map[str].push_back({x, y});
}

void map_read()
{
    ifstream inFile;
    inFile.open("mapa.txt");
    inFile >> x_init;
    inFile >> y_init;
    inFile.getline(t,x_init+10);
    map_1.resize(x_init+10);
    forward_map.resize(x_init+10);
    backward_map.resize(x_init+10);
    system("color B9");
    int v=0;
    int tab_init[x_init][y_init];
    while (inFile.getline(t, Size))
    {
        for (int i=0; i<x_init; i++)
        {

            switch ((int)t[i])
            {
            case 111:
                tab_init[i][v]=7; //7
                break;
            case '0':
                tab_init[i][v]=14; //0 (dodaj na mapie gwiazdy (.), kolor 15
                break;
            case '2':
                tab_init[i][v]=4; //14
                break;
            }
        }
        v++;
    }
    for(int x=0;x<x_init;x++)
        for(int y=0;y<y_init;y++)
            map_1[x].push_back(tab_init[x][y]);
    delete tab_init;
    delete t;
}
*/
wstring byteToWide(string const& src)
{
    wstring dest;
    dest.resize(src.size());
    for(unsigned int i = 0; i < src.size(); i++)
    {
        dest[i] = static_cast<int>(src[i]);
    }
    return dest;
}

void convertOldMap(const char* filenameSource, const char* filenameDestination)
{
    ifstream sourceStream;
    sourceStream.open(string(filenameSource));
    int width, height;
    sourceStream >> width;
    sourceStream >> height;
    vector<string> inBuff(height);
    vector<string> outBuff(height);
    char inputLine[width+1];
    for (int i = 0; i < height; i++)
    {
        sourceStream >> inputLine;
        inBuff[i] = string(inputLine);
        //cout << inBuff[i].c_str() << endl;
    }
    sourceStream.close();
    ofstream destStream;
    destStream.open(filenameDestination);
    destStream << width << ' ';
    destStream << height << '\n';
    for (int i = 0; i < height; i++)
    {
        outBuff[i] = inBuff[i];
        for (int j = 0; j < width; j++)
        {
            destStream << ' ';
        }
        destStream << '\n';
    }
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (inBuff[i][j] == '0')
                destStream << static_cast<unsigned short>(BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY);
            else if (inBuff[i][j] == '2')
                destStream << static_cast<unsigned short>(BACKGROUND_RED);
            else if (inBuff[i][j] == 'o')
                destStream << static_cast<unsigned short>(BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_BLUE);
            destStream << ' ';
        }
        destStream << '\n';
    }
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (inBuff[i][j] == '2')
                destStream << static_cast<unsigned>(1);
            else
                destStream << static_cast<unsigned>(0);
            destStream << ' ';
        }
        destStream << '\n';
    }
    destStream.close();
}

//#define MODE_MAP_CONVERSION

int main()
{
#ifdef MODE_MAP_CONVERSION
    convertOldMap("mapa.txt","nowaMapa.txt");
    return 0;
#endif
    Tadzik::Game theGame;
    theGame.run();
    /*
    Player player;
    map_read();
    ForwardAnalysis();
    BackwardAnalysis();
    start=45;
    init_display_1(0);
    float dt=0.03;
    player.setPos(start,5);
    player.vx=0;
    player.vy=0;
    g=10;
    char znak;
    short aKey,dKey,wKey,sKey;
    while (1)
    {
            dKey=GetAsyncKeyState(0x44);
            if(aKey<0)
                player.vx=player.vx-0.1;
            aKey=GetAsyncKeyState(0x41);
            if(dKey<0)
                player.vx=player.vx+0.1;
            wKey=GetAsyncKeyState(0x57);
            if(wKey<0 and map_1[(int)round(player.x)][(int)round(player.y)-1]==4)
                player.vy=player.vy+20;
            sKey=GetAsyncKeyState(0x53);
            if(sKey<0)
            {
                //player.vx=player.vx+0.1;
            }
            player.x=player.x+dt*player.vx;
            if(map_1[(int)round(player.x)][(int)round(player.y)+1]!=4)
            {
            player.vy=player.vy+dt*g; //Napisz od nowa / napraw
            }
            else
            player.vy=0;
            player.y=player.y+dt*player.vy;
            display_1((int)round(player.x)-start);
            player.display_1((int)round(player.x)-start);
        Sleep(5);
    }
    */
    return 0;
}
