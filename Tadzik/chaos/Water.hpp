#ifndef WATER_HPP
#define WATER_HPP

#define GLEW_STATIC
#include "../GL/glew.h"

#include "GameObject.hpp"
#include "Utils.hpp"
#include <chrono>
namespace chaos{

template <typename T>
std::string CHAOS_EXPORT toString ( T Number )
{
    std::ostringstream ss;
    ss << Number;
    return ss.str();
}

class Water: public DrawableGameObject
{
public:
    Water(Renderer* ren, int width, int height, GLfloat dx, GLfloat dy):DrawableGameObject(ren,"Cube:Vao_Pos","Shader_Pos"){
        std::string vaoId = "Terrain:Vao_Pos#" + chaos::toString<GLint>(width)+"x"+chaos::toString<GLint>(height)
                                       + "("+ chaos::toString<GLfloat>(dx)+"x"+chaos::toString<GLfloat>(dy)+")";
        std::cout << "id: " << vaoId << "\n";
        if(renderer->getVAO(vaoId) == nullptr){
            std::vector<GLfloat> vec;
            for(GLfloat i = 0; i <= width; i+=dx){
                for(GLfloat j = 0; j <= height; j+=dy){
                    //triangle1
                    vec.insert(vec.end(), {i,j,0.0});
                    vec.insert(vec.end(), {i+dx,j,0.0});
                    vec.insert(vec.end(), {i,j+dy,0.0});

                    //triangle2
                    vec.insert(vec.end(), {i+dx,j+dy,0.0});
                    vec.insert(vec.end(), {i,j+dy,0.0});
                    vec.insert(vec.end(), {i+dx,j,0.0});
                }
            }
            renderer->addVAO(3,0,0,0,&vec,vaoId);
        }
        setVertexArray(vaoId);

        std::string shrId = "Shader_Water";
        if(renderer->getShader(shrId) == nullptr){
            renderer->addShader({ std::make_pair("files/shaders/Water.vs", GL_VERTEX_SHADER),
                    std::make_pair("files/shaders/Water.fs", GL_FRAGMENT_SHADER)}, shrId);
        }
        setShader(shrId);
    }

    void draw(){
        shader->run();
        shader->setUniform("mx",renderer->getCamCombined()*getGlobalTransformMatrix());
        shader->setUniform("uniColor", color);
        using namespace std::chrono;
        milliseconds ms = duration_cast< milliseconds >(
        system_clock::now().time_since_epoch());
        int millis = (ms.count()-renderer->startTime);
        shader->setUniform("time",(GLfloat)(millis/1000.0));
        vao->bind();
        glDrawArrays(renderType, 0, vao->countVertices());
        vao->unbind();
    }

    GLuint renderType = GL_LINES;
};

}

#endif // WATER_HPP

