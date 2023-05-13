// Copyright 2023 JohnCorn
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include <map>
#include <cassert>
#include "Renderer.hpp"
#include "App.hpp"
#include "GLShader.hpp"

// ---------------------- Implementation data --------------------

 struct StaticMeshObject::ContextData : public IShaderData
 {
    friend class StaticMeshObject::Impl;
     GLuint program = build_shaders(L"Shaders/triangle.vert", L"Shaders/triangle.frag");
     GLuint VAO;
     //uniform locations
     //...

    ContextData()
    {
        glGenVertexArrays(1, &VAO);
        //here it finds locations of all uniform variables 
    }

    virtual ~ContextData() override
    {
        glDeleteVertexArrays(1, &VAO);
    }

    virtual void Bind() const override
    {
        glUseProgram(program);
        glBindVertexArray(VAO);
    }
};

/// @brief OpenGL data
struct StaticMeshObject::Impl
{
    // all settings for rendering
    GLuint VBO = 0;
    //IBO
    //some textures

    Impl(const Geometry& geometry)
    {
        VBO = IRenderer::GetBufferCache().Get(geometry.vertices);
        if (VBO == 0)
        {
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, geometry.vertices.GetTotalSize(), geometry.vertices.GetBuffer(), GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            IRenderer::GetBufferCache().Put(geometry.vertices, VBO);
        }
    }
};

// ------------------ Static API ----------------------

void StaticMeshObject::InitForContext(IRenderer& renderer)
{
    auto& data = renderer.NewContextData<ContextData>(GroupID());
}

// ------------------- API --------------------------

StaticMeshObject::StaticMeshObject(const Geometry& geometry)
{}

void StaticMeshObject::Render(const IRenderer& renderer, double timestamp) const
{
}