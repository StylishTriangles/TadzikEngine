#pragma once
#include <cstdio>
#include <iostream>

namespace Tadzik
{
class IO
{
public:
    void open(const wchar_t *filename);
    void open(const wchar_t *filename, const wchar_t *mode);
};

void IO::open(const wchar_t *filename)
{
    const wchar_t* it = filename;

}

void IO::open(const wchar_t *filename, const wchar_t* mode)
{
    FILE* pFile;
    pFile = _wfopen(filename, mode);
    std::cout << "Mode: " << (int)mode[0] << std::endl;
    fclose(pFile);
}

} //end of Tadzik
