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



#include "Renderer.hpp"
#include "App.hpp"

#include "GLShader.hpp"



struct MeshObject::ImplData
{
    static GLint program;

    GLuint VBO = 0;
};

GLint MeshObject::ImplData::program = 0;

MeshObject::MeshObject(const float vertices[], size_t vertices_count)
{
    
}

void MeshObject::BuildShaders()
{
    MeshObject::ImplData::program = build_shaders(L"Shaders/triangle.vert", L"Shaders/triangle.frag");
    glCreateVertexArrays(1, &MeshObject::ImplData::VAO);
    glBindVertexArray(MeshObject::ImplData::VAO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);
    glEnableVertexAttribArray(0);
    //here it finds locations of all uniform variables 
    glBindVertexArray(0);
}

void MeshObject::Render() const
{
    glBindVertexArray(MeshObject::ImplData::VAO);
    glBindBuffer(GL_ARRAY_BUFFER, impl_data->VBO);
    glUseProgram(MeshObject::ImplData::program);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}