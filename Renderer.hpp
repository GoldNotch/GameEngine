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
#include <vector>
#include <array>
#include <memory>
//#include <memory_resource>
#include <cassert>
#include <map>

using ShaderProgramID = unsigned char;
using RenderableObjectTypeID = unsigned int;
/// @brief stores data for context (thread-local global data for objects group)
struct IShaderProgram
{
    IShaderProgram(ShaderProgramID id) : id(id){};
    virtual ~IShaderProgram() = default;
    virtual void Bind() const = 0;
    inline ShaderProgramID GetID() const noexcept { return id; }

private:
    ShaderProgramID id;
    IShaderProgram(const IShaderProgram &) = delete;
    IShaderProgram &operator=(const IShaderProgram &) = delete;
};

struct IGPUStaticBuffer
{
    explicit IGPUStaticBuffer(size_t elem_size, size_t count, const void *data = nullptr)
        : size(elem_size * count){};
    virtual ~IGPUStaticBuffer() = default;
    virtual void Realloc(size_t elem_size, size_t count, const void *data = nullptr)
    {
        size = elem_size * count;
    };
    virtual void Upload(const void *data, size_t elem_size, size_t count, size_t offset = 0) = 0;

protected:
    size_t size; ///< allocated size in bytes
private:
    IGPUStaticBuffer(const IGPUStaticBuffer &) = delete;
    IGPUStaticBuffer &operator=(const IGPUStaticBuffer &) = delete;
};

struct IRenderer;

/// @brief object class which can be rendered.
/// UPD: renderable object doesn't have any GL objects. It's just object which will be rendered.
///     It's like decoration object on theatre scene
struct IRenderable
{
    virtual void Render(const IRenderer &renderer, double timestamp) const = 0;
    virtual RenderableObjectTypeID GetTypeID() const = 0;
};

/// container for objects on scene sorted and grouped by ShaderProgramID
struct RenderableScene
{
    using RenderableGroup = std::vector<std::unique_ptr<IRenderable>>;
    friend class IRenderer;
    /// constructor
    explicit RenderableScene(const IRenderer &renderer) : context(renderer) {}
    /// destructor
    ~RenderableScene() = default;
    // void SetCamera();
    /// Place object on scene. Uses to add object to render
    template <class T, typename... Args>
    T &PlaceObject(Args &&...args);

private:
    const IRenderer &context;     ///< rendering context
    std::vector<RenderableGroup> scene_objects; ///< objects grouped by ShaderProgramID
    
    /// actions executed when new shader program created
    void OnShaderProgramCreated(ShaderProgramID id) noexcept;
    /// Get Objects with the same shader program
    const RenderableGroup &GetObjectsByShaderProgram(ShaderProgramID id) const;

    RenderableScene(const RenderableScene &) = delete;
    RenderableScene &operator=(const RenderableScene &) = delete;
};

template <class T, typename... Args>
T & RenderableScene::PlaceObject(Args &&...args)
{
    std::unique_ptr<T> obj = std::make_unique<T>(args);
}

struct IRenderableSceneBuilder
{
    virtual void InitObjectTypes(IRenderer& renderer) const = 0;
    virtual void BuildScene(RenderableScene & scene) const = 0;
};

/// own context data and begin rendering. thread-local rendering interface
struct IRenderer
{
    friend struct RenderableScene;

    static void InitRenderingSystem();
    //-------------- API ---------------------
    explicit IRenderer() = default;
    virtual ~IRenderer() = default;
    /// render frame function
    virtual void RenderPass(double timestamp) = 0;
    /// create new shader program (build must be in constructor)
    template <typename T>
    T &NewShaderProgram(RenderableObjectTypeID id)
    {
        ShaderProgramID new_id = programs.size();
        T* program = new T(new_id);
        cached_scene.OnShaderProgramCreated(new_id);
        return *program;
    }

    void BindSceneBuilder(const IRenderableSceneBuilder* scene_builder)
    { 
        this->scene_builder = scene_builder; 
        scene_builder->InitObjectTypes(*this);
    }

protected:
    /// traverse scene, form vbos and send it to GPU
    // virtual void PrepareRenderData(const RenderableScene& scene) = 0;
    void RenderWithShaderProgram(ShaderProgramID id, double timestamp) const;

protected:
    RenderableScene cached_scene{*this}; ///< cache of objects in scene
    const IRenderableSceneBuilder* scene_builder = nullptr;   ///< client model to build scene
private:
    using ContextID = unsigned int;
    ContextID RequestNewContextID() const;
    ContextID ctx_id = RequestNewContextID();

    std::vector<std::unique_ptr<IShaderProgram>> programs;  ///< shader programs
    std::map<ShaderProgramID, RenderableObjectTypeID> object_types_by_shader;
};


// ------------------------- Scene object types -----------------------

namespace scene3d
{

    enum ETypeID : RenderableObjectTypeID
    {
        STATIC_MESH = 0,
        TOTAL
    };

    /// group - MeshObject
    struct StaticMeshObject : public IRenderable
    {
        //------------ Types ---------------
        struct Vertex
        {
            float pos[3];
        };

        struct Geometry
        {
            const Vertex *vertices;
            size_t vertices_count;
            Geometry(const Vertex *vertices, size_t vertices_count) : vertices(vertices), vertices_count(vertices_count) {}
        };

        // ----------- API ------------------
        StaticMeshObject(const Geometry &geometry);
        virtual void Render(const IRenderer &renderer, double timestamp) const override;
        virtual RenderableObjectTypeID GetTypeID() const override 
        { return static_cast<RenderableObjectTypeID>(ETypeID::STATIC_MESH); }
    protected:
        // transform
        struct Impl;
        std::unique_ptr<Impl> impl;
        struct ShaderProgram;
    };

}
