#include "../include/Utils.hpp"

namespace Utils{

sf::FloatRect getBiggerBoundingBox(sf::Sprite& sp){
    sf::FloatRect retBox = sp.getGlobalBounds();
    retBox.left -= retBox.width/2;
    retBox.width *= 2;
    retBox.top -= retBox.height;
    retBox.height *= 2;
    return retBox;
}

void drawBoundingBox(sf::Sprite& sp, sf::RenderTarget* target){
    sf::RectangleShape shp(sf::Vector2f(sp.getGlobalBounds().width, sp.getGlobalBounds().height));
    shp.setPosition(sf::Vector2f(sp.getGlobalBounds().left, sp.getGlobalBounds().top));
    shp.setOutlineColor(sf::Color::Green);
    shp.setFillColor(sf::Color(0,0,0,0));
    shp.setOutlineThickness(1);
    target->draw(shp);
}

float randFloat(float from, float to) {
    return ((float)rand()/RAND_MAX*(to-from)+from);
}

int randInt(int from, int to) {
    return (rand()%(to-from))+from;
}

sf::Vector2i randVector2i (sf::IntRect i) {
    return sf::Vector2i(randInt(i.left, i.left+i.width), randInt(i.top, i.top+i.height));
}

sf::Vector2f randVector2f (sf::FloatRect i) {
    return sf::Vector2f(randFloat(i.left, i.left+i.width), randFloat(i.top, i.top+i.height));
}

sf::Color randColor (int from, int to) {
    return sf::Color(randInt(from, to), randInt(from, to), randInt(from, to));
}

bool isMouseOnSprite(sf::Sprite& sp, sf::RenderWindow* window) {
    if (sf::Mouse::getPosition(*window).x >= sp.getPosition().x && sf::Mouse::getPosition(*window).x <= sp.getPosition().x+sp.getGlobalBounds().width &&
        sf::Mouse::getPosition(*window).y >= sp.getPosition().y && sf::Mouse::getPosition(*window).y <= sp.getPosition().y+sp.getGlobalBounds().height)
        return true;
    return false;
}

std::string stringify(int x){
    std::ostringstream o;
    o << x;
    return o.str();
}

std::string stringifyf(float x){
    std::ostringstream o;
    o << x;
    return o.str();
}

float det3f(sf::Vector3f top, sf::Vector3f mid, sf::Vector3f bot)
{
    // top.x mid.x bot.x
    // top.y mid.y bot.y
    // top.z mid.z bot.z
    return top.x*(mid.y*bot.z - bot.y*mid.z) - mid.x*(top.y*bot.z - bot.y*top.z) + bot.x*(top.y*mid.z - mid.y*top.z);
}

int sgn(double x)
{
    if (x==0) return 0;
    if (x>0) return 1;
    if (x<0) return -1;
}

std::string getDate() {
    std::time_t t = std::time(0);
    tm *ltm = std::localtime(&t);
    std::string s;
    s  = stringify(1900+ltm->tm_year) + "-";
    s += stringify(1 + ltm->tm_mon) + "-";
    s += stringify(ltm->tm_mday) + "_";
    s += stringify(ltm->tm_hour) + "-";
    s += stringify(ltm->tm_min) + "-";
    s += stringify(1 + ltm->tm_sec);
    return s;
}

void setOriginInCenter(sf::Sprite& s) {
    s.setOrigin(s.getTextureRect().width/2, s.getTextureRect().height/2);
};

bool chance(float c) {
    return (randFloat(0, 1) <= c);
}

double getAngle (sf::Vector2f p1, sf::Vector2f p2) {
    return atan2(p2.y-p1.y, p2.x-p1.x);
}

float getMagnitude(sf::Vector2f p1, sf::Vector2f p2) {
    return (p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y);
}

sf::Vector2f rotatedPoint(sf::Vector2f p, sf::Vector2f center, float d) {
    float a = atan2(p.y-center.y, p.x-center.x);
    sf::Vector2f r;
    r.x = p.x+d*sin(a);
    r.y = p.y-d*cos(a);
    return r;
}

void normalize(sf::Vector2f &v) {
    float length = sqrt(v.x*v.x+v.y*v.y);
    v = sf::Vector2f(v.x/length, v.y/length);
}

float dotProduct(sf::Vector3f a, sf::Vector3f b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

sf::Vector3f crossProduct(sf::Vector3f a, sf::Vector3f b)
{
    return {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
}

void normalize3f(sf::Vector3f& vec)
{
    vec /= sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

void vecSetLength(sf::Vector3f& vec, float length)
{
    vec *= (length / sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z));
}

float vecSquaredLength(sf::Vector3f vec)
{
    return vec.x*vec.x + vec.y*vec.y + vec.z*vec.z;
}

float vecLength(sf::Vector3f vec)
{
    return sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

bool fileExists(const std::string& filename)
{
    struct stat buf;
    if (stat(filename.c_str(), &buf) != -1)
    {
        return true;
    }
    return false;
}

sf::Vector2f getViewOffset(sf::RenderTarget* rt) {
    return rt->mapPixelToCoords(sf::Vector2i(0,0));
}

float getArea(sf::FloatRect r) {
    return r.width*r.height;

}

}
