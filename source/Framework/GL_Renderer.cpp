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

#include "Scene2D.hpp"
#include <string>
#include <fstream>
#include <streambuf>
#include "../containers/Cache.hpp"
extern "C"
{
#include <GL/glew.h>
}

using namespace Framework;

void IRenderer::InitRenderingSystem()
{
    if (GLenum glew_init_error = glewInit() != GLEW_OK)
    {
        const GLubyte *msg = glewGetErrorString(glew_init_error);
        std::printf("Failed to init glew - %s\n", msg);
    }
}

// ============================= Shaders.cpp =================================

static inline std::string read_shader_file(const wchar_t *path)
{
    std::ifstream f(path);
    return std::string((std::istreambuf_iterator<char>(f)),
                       std::istreambuf_iterator<char>());
}

static inline GLint compile_shader(const wchar_t *path, GLenum type)
{
    std::string src = read_shader_file(path);
    const char *src_ptr = src.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src_ptr, NULL);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLint info_log_size;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_size);
        std::vector<char> info_log(info_log_size);
        glGetShaderInfoLog(shader, info_log_size, NULL, info_log.data());
        glDeleteShader(shader);
        shader = 0;
    }
    return shader;
}

static inline GLint build_shaders(const wchar_t *vertex_shader_path, const wchar_t *fragment_shaders_path)
{
    if (GLuint vertex_shader = compile_shader(vertex_shader_path, GL_VERTEX_SHADER))
        if (GLuint fragment_shader = compile_shader(fragment_shaders_path, GL_FRAGMENT_SHADER))
        {
            GLuint program = glCreateProgram();
            glAttachShader(program, vertex_shader);
            glAttachShader(program, fragment_shader);
            glLinkProgram(program);
            GLint success;
            glGetProgramiv(program, GL_LINK_STATUS, &success);
            if (!success)
            {
                GLint info_log_size;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_size);
                std::vector<char> info_log(info_log_size);
                glGetProgramInfoLog(program, info_log_size, NULL, info_log.data());
                glDeleteProgram(program);
                program = 0;
            }

            glDeleteShader(vertex_shader);
            glDeleteProgram(fragment_shader);
            return program;
        }

    return 0;
}

// ========================== Buffers.cpp =========================

template <GLint target>
struct StaticBufferObject final : public IGPUStaticBuffer
{
    explicit StaticBufferObject(size_t elem_size, size_t count, const void *data)
        : IGPUStaticBuffer(elem_size, count, data)
    {
        glCreateBuffers(1, &id);
        glBindBuffer(target, id);
        glBufferData(target, size, data, GL_STATIC_DRAW);
        glBindBuffer(target, 0);
    }

    virtual ~StaticBufferObject() override
    {
        glDeleteBuffers(1, &id);
    }

    inline void Bind() const
    {
        glBindBuffer(target, id);
    }

    virtual void Realloc(size_t elem_size, size_t count, const void *data = nullptr) override
    {
        IGPUStaticBuffer::Realloc(elem_size, count, data);
        glBindBuffer(target, id);
        glBufferData(target, elem_size * count, data, GL_STATIC_DRAW);
        glBindBuffer(target, 0);
    }

    virtual void Upload(const void *data, size_t elem_size, size_t count, size_t offset = 0) override
    {
        assert(offset < size && offset + elem_size * count < size);
        glBindBuffer(target, id);
        glBufferSubData(target, offset, elem_size * count, data);
        glBindBuffer(target, 0);
    }

protected:
    GLuint id;
    // TODO: make vbo cached
    //static LRUCache<GLuint, const void*> cache;
};

/*struct StaticTexture final : public IGPUStaticBuffer
{
    explicit StaticTexture(size_t elem_size, size_t count, const void *data)
        : IGPUStaticBuffer(elem_size, count, data)
    {
    }

    virtual ~StaticTexture() override
    {
    }

    void SetUniform(GLint location)
    {}

    virtual void Realloc(size_t elem_size, size_t count, const void *data = nullptr) override
    {
        IGPUStaticBuffer::Realloc(elem_size, count, data);
    }

    virtual void Upload(const void *data, size_t elem_size, size_t count, size_t offset = 0) override
    {
        assert(offset < size && offset + elem_size * count < size);
    }

protected:
    GLuint id;
};*/




// =========================== Shader programs ======================



namespace scene2d
{
// ========================= StaticMesh =============================

struct StaticMeshBuilder::GPUData
{
    StaticBufferObject<GL_ARRAY_BUFFER> vbo;
};

struct StaticMeshBuilder::Program : public IShaderProgram
{
    Program()
    {
        glGenVertexArrays(1, &vao);
        /*glBindVertexArray(vao);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);  */   
    }

    virtual ~Program() override
    {
        glDeleteProgram(gl_program);
        glDeleteVertexArrays(1, &vao);
    };

    virtual ObjectsTypeID GetObjectsTypeID() const override
    { return static_cast<ObjectsTypeID>(ObjectTypes::STATIC_MESH); }

    virtual void Bind() const override
    {
        glUseProgram(gl_program);
        glBindVertexArray(vao);
    }

    void RenderObject(double timestamp, ObjectsTypeID type_id, const IRenderableScene& scene)
    {
        
    }

private:
    GLint gl_program = build_shaders(L"Shaders/triangle.vert", L"Shaders/triangle.frag");
    GLuint vao;
    //std::unordered_map<StaticMesh, StaticMesh::GPUData> gpu_buffers; // TODO: use cache
};




// ============================= Renderer ===============================

Renderer::Renderer(const IRenderableSceneBuilder& scene_builder)
    : IRenderer(scene_builder)
    , static_mesh_program(new StaticMeshBuilder::Program())
{
}

void Renderer::RenderPass(double timestamp) 
{
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    auto scene = scene_builder.BuildScene();

    //set camera in 3d
    //render scene
    scene->RenderGroup(static_cast<ObjectsTypeID>(ObjectTypes::STATIC_MESH), timestamp);
}

}

template<>
void InitShaders<StaticMeshObject>(IRenderer& renderer)
{
    renderer.NewShaderProgram<StaticMeshObject::ShaderProgram>(STATIC_MESH);
}
