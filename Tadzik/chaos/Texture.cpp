#include "Texture.hpp"

using namespace chaos;

GLuint Texture::textureCounter = 0;

Texture::Texture(std::string fpath)
:Resource(fpath)
{
    batchId = textureCounter;

    glGenTextures(1, &id);
    bind(GL_TEXTURE_2D);

    sf::Image imgData;
    if (!imgData.loadFromFile(fpath))
    {
        std::cout << "Could not load " << fpath << "\n";
    }


    width = imgData.getSize().x;
    height = imgData.getSize().y;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_BGRA,GL_UNSIGNED_BYTE,imgData.getPixelsPtr());
    glGenerateMipmap(GL_TEXTURE_2D);

    unbind();
    textureCounter++;
}

Texture::~Texture(){
    std::cout << "Destructor of Texture\n";
    glDeleteTextures(1, &id);
}

void Texture::loadFromTextureData(Texture* data){

}

void Texture::bind(GLenum t) {
    type = t;
    glBindTexture(type, id);
}

/*void Texture::bindOnSlot(GLenum slot){
    glActiveTexture(slot);
    glBindTexture(type, id);
}*/

void Texture::unbind(){
    glBindTexture(type, 0);
}

GLenum Texture::getTextureType(){
    return type;
}

GLuint Texture::getWidth(){
    return width;
}

GLuint Texture::getHeight(){
    return height;
}

GLuint Texture::getId(){
    return id;
}

GLuint Texture::getBatchId(){
    return batchId;
}
