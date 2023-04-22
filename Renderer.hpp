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

enum ERenderableObjectType : size_t
{
    MESH_OBJECT = 0,
    TOTAL
};

/// @brief Thread-local object to keep some settings
struct RenderContext
{
    explicit RenderContext();
    ~RenderContext() = default;
    void BeginRenderGroup();
    void EndRenderGroup();

private:
    struct RenderContextPrivateData;
    std::unique_ptr<RenderContextPrivateData> impl = nullptr;
    RenderContext(const RenderContext&) = delete;
    RenderContext& operator=(const RenderContext&) = delete;
};


struct IRenderableObject
{
    virtual void Render() const = 0;
};

struct RenderSceneData
{
    //camera data
    std::vector<IRenderableObject*> objects;
};

struct MeshObject : public IRenderableObject
{
    MeshObject(const float vertices_data[], size_t vertices_count);
    static void BuildContext();
    static void BuildShaders(/*pass settings*/); // build shaders
    virtual void Render() const override;
protected:
    struct ImplData;
    std::unique_ptr<ImplData> impl_data;
    size_t vertices_count = 0;
};

static inline void BuildShaders()
{
    MeshObject::BuildShaders();
}



