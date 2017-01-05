#ifndef MODEL_HPP
#define MODEL_HPP

#include "Export.hpp"

#include "../include/Utils.hpp"

#include <string>
#include <fstream>

#include "GameObject.hpp"
#include "MeshPrefab.hpp"

namespace chaos
{

template <typename T>
std::string CHAOS_EXPORT toString ( T Number )
{
    std::ostringstream ss;
    ss << Number;
    return ss.str();
}

class CHAOS_EXPORT Model: public DrawableGameObject{
public:
    Model(Renderer*r, MeshPrefab* prefab)
    :DrawableGameObject(r)
    {
        setMesh(prefab);
        shader = renderer->getShader("Shader_Mesh3d");
        //vao = renderer->getVAO("Mesh:Vao_Pos.Uv("+toString(prefab->getMeshId())+")");
    }

    virtual void draw(){
        shader->run();
        shader->setUniform("ourColor", color);
        shader->setUniform("mx",renderer->getCamCombined()*getGlobalTransformMatrix());
        vao->bind();
        glDrawArrays(GL_TRIANGLES, 0, vao->countVertices());
        vao->unbind();
    }

    void setMesh(MeshPrefab* prefab){
        meshPrefab = prefab;
        vao = renderer->getVAO("Mesh:Vao_Pos.Uv("+toString(meshPrefab->getMeshId())+")");
    }

protected:
    MeshPrefab* meshPrefab = nullptr;
};


}

#endif //MODEL_HPP



