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
#include <atomic>

// -------------------- Static data ----------------

IRenderer::ContextID IRenderer::RequestNewContextID() const
{
    static std::atomic<ContextID> last_context_id = 0;
    return last_context_id++;
}

void IRenderer::RenderWithShaderProgram(ShaderProgramID id, double timestamp) const
{
    programs[id]->Bind();
    const auto &objs = cached_scene.GetObjectsByShaderProgram(id);
    for (auto &&obj : objs)
        obj->Render(*this, timestamp);
}

// here must be uploaders

const RenderableScene::RenderableGroup &RenderableScene::GetObjectsByShaderProgram(ShaderProgramID id) const
{
    return scene_objects[id];
}

void RenderableScene::OnShaderProgramCreated(ShaderProgramID id) noexcept
{
    scene_objects.emplace_back();
}