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
#include <vector>
#include <memory>

using ContextID = unsigned int;
static ContextID InitNewContext();

struct IRenderableObject
{
    virtual void Render(ContextID ctx_id) const = 0;
};


struct RenderSceneData
{
    //camera data
    std::vector<IRenderableObject*> objects;
};

struct MeshObject : public IRenderableObject
{
    /// @brief Build shaders
    static void BuildShaders(/*pass settings*/);
    static void InitForContext(ContextID ctx_id);
    static void BeginRendering(ContextID ctx_id);
    static void EndRendering(ContextID ctx_id);

    MeshObject(const float vertices_data[], size_t vertices_count);
    virtual void Render(ContextID ctx_id) const override;

protected:
    struct ImplData;
    std::unique_ptr<ImplData> impl_data;
    size_t vertices_count = 0;
};



static inline void BuildShaders()
{
    MeshObject::BuildShaders();
    //...
}



