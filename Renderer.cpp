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
#include "Cache.hpp"
#include <atomic>
extern "C"
{
#include <GL/glew.h>
}

// -------------------- Static data ----------------

IRenderer::TCache IRenderer::BufferCache(10);

// ------------------- Buffers -------------------

GPUBuffer::GPUBuffer(const CPUBufferPtr& buffer)
{
    
}

GPUBuffer::~GPUBuffer()
{

}

void GPUBuffer::Bind() const
{

}

IRenderer::ContextID IRenderer::RequestNewContextID()
{
    static std::atomic<ContextID> last_context_id = 0;
    return last_context_id++;
}

void IRenderer::InitRenderingSystem()
{
    if (GLenum glew_init_error = glewInit() != GLEW_OK)
    {
        const GLubyte *msg = glewGetErrorString(glew_init_error);
        std::printf("Failed to init glew - %s\n", msg);
    }
}

IRenderer::TCache& IRenderer::GetBufferCache()
{
    return IRenderer::BufferCache;
}

//here must be uploaders
