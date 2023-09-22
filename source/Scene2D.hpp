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
#pragma once
#include "Renderer.hpp"

namespace scene2d
{

/// @brief identifiers for objects. Uses for scene objects storage
enum class ObjectTypes : ObjectsTypeID
{
    STATIC_MESH,
    TOTAL
};


using vec2 = std::array<float, 2>;
using mat3 = std::array<float, 9>;

/// @brief object's placement on scene 
struct Placement
{
    vec2 pos;
    vec2 rotation;
    vec2 scale;
};

/// @brief object placed on scene
struct SceneObject : public IRenderable
{
    SceneObject(ObjectTypes type, Placement placement, const Geometry* geometry)
        : type(static_cast<ObjectsTypeID>(type))
        , placement(placement)
        , geometry(geometry)
    {}
    virtual ~SceneObject() = default;
    const Geometry * GetGeometry() const { return geometry; }
    constexpr ObjectsTypeID GetTypeID() const { return type; }
private:
    const Geometry* geometry = nullptr;
    Placement placement;
    ObjectsTypeID type;
};

struct StaticMeshBuilder
{
    friend struct Renderer3d;
    struct Program;
    struct Vertex
    {
        float pos[2];
    };

private:
    struct GPUData;
};

/// container for objects on scene sorted and grouped by type
class Scene final : public IRenderableScene 
{
public:
    explicit Scene() = default;
    /// destructor
    virtual ~Scene() override = default;
    virtual void RenderGroup(ObjectsTypeID type_id, double timestamp) const override
    {  for(auto && obj : objects[type_id]) obj.Render(timestamp); }

    /// add new geometry
    template <typename VertexT>
    const Geometry &NewGeometry(CPUBufferPtr<VertexT> vertices,
                                 Geometry::IndexData indices = Geometry::IndexData())
    { return geometry.emplace_back(sizeof(VertexT), vertices.template ReinterpretCast<std::byte>(), indices); }
    /// place object
    inline void PlaceObject(const SceneObject& object)
    {  objects[object.GetTypeID()].push_back(object);  }

private:
    std::vector<Geometry> geometry;
    std::array<std::vector<SceneObject>, static_cast<std::size_t>(ObjectTypes::TOTAL)> objects;
};

/// @brief renderer for 2d scene.
struct Renderer : public IRenderer
{
    Renderer(const IRenderableSceneBuilder& scene_builder);
    virtual ~Renderer() override = default;

    /// @brief main function for rendering 
    /// @param timestamp 
    virtual void RenderPass(double timestamp) override;
private:
    std::unique_ptr<IShaderProgram> static_mesh_program;
    // other programs
};

}