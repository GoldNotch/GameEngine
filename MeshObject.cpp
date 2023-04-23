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
#include "Renderer.hpp"
#include "App.hpp"
#include "GLShader.hpp"

// ---------------------- Implementation data --------------------

/// @brief  Thread-local data for rendering
struct ContextData
{
    GLuint VAO;
    
    ContextData() noexcept
    {
        glCreateVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }

    void Bind() const noexcept
    {
        glBindVertexArray(VAO);
    }

    void Unbind() const noexcept
    {
        glBindVertexArray(0);
    }
};

/// @brief OpenGL data
struct MeshObject::ImplData
{
    static GLint program;
    static std::map<ContextID, ContextData> ctx_data;

    GLuint VBO = 0;

    static void BeginRendering(ContextID ctx_id)
    {
        glUseProgram(program);
        ctx_data[ctx_id].Bind();
    }

    static void EndRendering(ContextID ctx_id)
    {
        glUseProgram(0);
        ctx_data[ctx_id].Unbind();
    }
};

GLint MeshObject::ImplData::program = 0;
std::map<ContextID, ContextData> MeshObject::ImplData::ctx_data;

// ------------------ Static API ----------------------
 
void MeshObject::BuildShaders()
{
    MeshObject::ImplData::program = build_shaders(L"Shaders/triangle.vert", L"Shaders/triangle.frag");
    //here it finds locations of all uniform variables 
}

void MeshObject::InitForContext(ContextID ctx_id)
{
    MeshObject::ImplData::ctx_data.emplace(ctx_id, ContextData{});
}

void MeshObject::BeginRendering(ContextID ctx_id)
{
    MeshObject::ImplData::BeginRendering(ctx_id);
}

void MeshObject::EndRendering(ContextID ctx_id)
{
    MeshObject::ImplData::EndRendering(ctx_id);
}

// ------------------- API --------------------------

MeshObject::MeshObject(const float vertices[], size_t vertices_count)
{
    
}

void MeshObject::Render(ContextID ctx_id) const
{
    glBindBuffer(GL_ARRAY_BUFFER, impl_data->VBO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}