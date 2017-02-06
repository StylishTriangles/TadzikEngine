#ifndef COMMON_HPP
#define COMMON_HPP

#include <SFML/Graphics.hpp>

#ifndef M_PI
#define M_PI 3.1415926535897
#endif

    // Portable helpers
    static int   Stricmp(const char* str1, const char* str2)         { int d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
    static int   Strnicmp(const char* str1, const char* str2, int n) { int d = 0; while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; n--; } return d; }
    static char* Strdup(const char *str)                             { size_t len = strlen(str) + 1; void* buff = malloc(len); return (char*)memcpy(buff, (const void*)str, len); }

namespace Common {
    namespace Font {
        sf::Font Carnevalee_Freakshow;
        sf::Font Days_Later;
        sf::Font Comic_Sans;
        sf::Font Digital_7;
    }
    void loadFonts() {
        Font::Carnevalee_Freakshow.loadFromFile("files/fonts/Carnevalee_Freakshow.ttf");
        Font::Days_Later.loadFromFile("files/fonts/28_Days_Later.ttf");
        Font::Comic_Sans.loadFromFile("files/fonts/Comic_Sans.ttf");
        Font::Digital_7.loadFromFile("files/fonts/Digital_7.ttf");
    }
}

#endif //COMMON_HPP
